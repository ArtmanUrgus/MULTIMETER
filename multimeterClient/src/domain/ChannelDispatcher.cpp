#include "ChannelDispatcher.h"
#include "ChannelClient.h"

#include <QRunnable>
#include <QDebug>
#include <QObject>
#include <QStringList>

namespace
{
    constexpr int awaitingTime{500};
    constexpr int requestState{0};
    constexpr int requestFirstParameter{1};
    constexpr int requestHasAtLeastOneParameter{2};

    static QHash<QString, int> status{
        {"idle_state", 0},
        {"measure_state", 1},
        {"busy_state", 2},
        {"error_state", 3},
    };
}

ChannelDispatcher::ChannelDispatcher(int id, QObject *parent)
    : QObject{parent}
    , chId{id}
    , channelClient{ new ChannelClient{this, id} }
{
    statusRequestTimer.setInterval(3000);
    connect(&statusRequestTimer, &QTimer::timeout, this, [&]{
        lastCommandRequested = QStringLiteral("get_status");
        channelClient->sendCommandToServer( QString("%1 channel%2\n").arg(lastCommandRequested).arg(chId) );
    });
    statusRequestTimer.start();

    dataRequestTimer.setInterval(1000);
    connect(&dataRequestTimer, &QTimer::timeout, this, [&]{
        lastCommandRequested = QStringLiteral("get_result");
        channelClient->sendCommandToServer( QString("%1 channel%2\n").arg(lastCommandRequested).arg(chId) );
    });
}

ChannelDispatcher::~ChannelDispatcher()
{
    disconectChannel();
}

void ChannelDispatcher::disconectChannel()
{
    if( channelClient )
    {
        channelClient->sendCommandToServer(QString("exit\n"));

        QTimer::singleShot(awaitingTime, nullptr, [&](){
            channelClient->stop();
        } );

        delete channelClient;
        channelClient = nullptr;
    }
}

void ChannelDispatcher::handleMessageFromChannel(const QString &msg)
{
    auto messageList = msg.trimmed().split(" ");

    if( not messageList.isEmpty() )
    {
        for(auto& data : messageList)
        {
            data.remove(",");
            data.remove("\n");
        }

        if( lastCommandRequested == "start_measure" )
        {
            if( messageList.at(requestState) == "fail" )
            {
                setStatus(ChannelDispatcher::Idle);
            }
            else
                setStatus(ChannelDispatcher::Measure);
        }
        else if( lastCommandRequested == "stop_measure" )
        {
            if( messageList.at(requestState) == "ok" )
                setStatus(ChannelDispatcher::Idle);
            else
                setStatus(ChannelDispatcher::Measure);
        }
        else if( lastCommandRequested == "get_status" )
        {
            if( messageList.at(requestState) == "ok" )
                setStatus( status[messageList.at(1)] );
        }
        else if( lastCommandRequested == "set_range" )
        {
            if(messageList.size() == requestHasAtLeastOneParameter)
            {
                if( messageList.at(requestState) == "ok" &&
                    messageList.at(requestFirstParameter).contains("range") )
                    setRange( messageList[1].remove("range").toInt() );
            }
        }
        else if( lastCommandRequested == "get_result" )
        {
            if( messageList.at(requestState) == "ok" )
            {
                messageList.pop_front();

                QVector<float> result;
                for( auto& value : messageList )
                {
                    bool converted{false};
                    if( auto v = value.toFloat(&converted); converted )
                        result.append( v );
                }

                setValue(result);
            }
        }

        lastCommandRequested = QStringLiteral("");
    }
}

int ChannelDispatcher::channelId()
{
    return chId;
}

int ChannelDispatcher::range()
{
    return channelRange;
}

int ChannelDispatcher::state()
{
    return channelStatus;
}

float ChannelDispatcher::value()
{
    return channelResultValue.last();
}

void ChannelDispatcher::submitRangeChangeRequest(int range)
{
    lastCommandRequested = QStringLiteral("set_range");
    channelClient->sendCommandToServer( QString("%1 channel%2, range%3\n")
                                        .arg(lastCommandRequested).arg(chId).arg(range) );

    QTimer::singleShot(500, this, [&]{
        lastCommandRequested = QStringLiteral("get_status");
        channelClient->sendCommandToServer( QString("%1 channel%2\n")
                                            .arg(lastCommandRequested).arg(chId) );
    });
}

void ChannelDispatcher::submitMessureChangeRequest(bool measureState)
{
    lastCommandRequested = QString("%1_measure").arg( measureState ? "start" : "stop");
    channelClient->sendCommandToServer( QString("%1 channel%2\n")
                                        .arg( lastCommandRequested ).arg(chId) );

    QTimer::singleShot(500, this, [&]{
        lastCommandRequested = QStringLiteral("get_status");
        channelClient->sendCommandToServer( QString("%1 channel%2\n")
                                            .arg(lastCommandRequested).arg(chId) );
    });

    if(measureState)
    {
        dataRequestTimer.start();
        return;
    }

    dataRequestTimer.stop();
}

void ChannelDispatcher::setRange(int v)
{
    if( v >= Range::Mega && v <= Range::Micro )
    {
        channelRange = v;
        emit rangeChanged( chId, channelRange );
    }
}

void ChannelDispatcher::setStatus(int v)
{
    if( v >= State::Idle && v <= State::Error )
    {
        channelStatus = v;
        emit statusChanged( chId, channelStatus );
    }
}

void ChannelDispatcher::setValue(const QVector<float> &v)
{
    channelResultValue = v;
    emit valueChanged( chId, channelResultValue.last() );
}

void ChannelDispatcher::setDisconneted()
{
    setRange(Kilo);
    setValue(QVector<float>{0.0});
    channelStatus = Disconnected;
    emit statusChanged( chId, channelStatus );
    QTimer::singleShot(1000, this, [&]{ channelClient->start(); } );
}

void ChannelDispatcher::close()
{
    channelClient->stop();
}

#include "ChannelDispatcher.moc"
#include "moc_ChannelDispatcher.cpp"
