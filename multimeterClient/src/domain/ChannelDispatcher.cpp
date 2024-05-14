#include "ChannelDispatcher.h"
#include "ChannelClient.h"

#include <QRunnable>
#include <QDebug>
#include <QObject>
#include <QStringList>

ChannelDispatcher::ChannelDispatcher(int id, QObject *parent)
    : QObject{parent}
    , chId{id}
    , channelClient{ new ChannelClient{this, id} }
{}

ChannelDispatcher::~ChannelDispatcher()
{
    disconectChannel();
}

void ChannelDispatcher::disconectChannel()
{
    if( channelClient )
    {
        delete channelClient;
        channelClient = nullptr;
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
    channelClient->request(QStringLiteral("set_range"), QString("channel%2, range%3")
                           .arg(chId).arg(range) );
}

void ChannelDispatcher::submitMessureChangeRequest(bool measureState)
{
    auto command = QString("%1_measure").arg( measureState ? "start" : "stop");
    channelClient->request(command, QString("channel%2").arg(chId) );
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

void ChannelDispatcher::setValue(const float& v)
{
    emit valueChanged( chId, v );
}

void ChannelDispatcher::setDisconneted()
{
    setRange(Kilo);
    setValue(0.0);
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
