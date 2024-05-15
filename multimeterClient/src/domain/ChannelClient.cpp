#include "ChannelClient.h"
#include "ChannelDispatcher.h"

#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <errno.h>

#include <QDebug>

namespace
{
    constexpr int awaitingTime{500};

    constexpr uint16_t port{11047};
    constexpr int maxBufferSize{65546};
    constexpr int invalideValue{-1};

    constexpr int requestState{0};
    constexpr int requestFirstParameter{1};
    constexpr int requestHasAtLeastOneParameter{2};

    static QHash<QString, int> status{
        {"idle_state", 0},
        {"measure_state", 1},
        {"busy_state", 2},
        {"error_state", 3},
    };

    class AbstractRequestHandler
    {
    public:
        AbstractRequestHandler(ChannelDispatcher* dsp, int ch)
            : diapatcher{dsp}
            , channelId{ch}
        {}

        virtual ~AbstractRequestHandler()
        {
            if(diapatcher){
                delete diapatcher;
                diapatcher = nullptr;
            }
        }
        virtual void handleResponse(QStringList& messageList) = 0;

        protected:
            ChannelDispatcher* diapatcher{nullptr};
            int channelId;
    };

    class StartMeasureHandler : public AbstractRequestHandler
    {
    public:
        StartMeasureHandler(ChannelDispatcher* dsp, int ch): AbstractRequestHandler{dsp, ch}{}
        void handleResponse(QStringList& messageList) override final
        {
            if( messageList.at(requestState) == "fail" )
            {
                diapatcher->setStatus(ChannelDispatcher::Idle);
            }
            else
                diapatcher->setStatus(ChannelDispatcher::Measure);
        }
    };

    class StopMeasureHandler : public AbstractRequestHandler
    {
    public:
        StopMeasureHandler(ChannelDispatcher* dsp, int ch): AbstractRequestHandler{dsp, ch}{}
        void handleResponse(QStringList& messageList) override final
        {
            if( messageList.at(requestState) == "ok" )
                diapatcher->setStatus(ChannelDispatcher::Idle);
            else
                diapatcher->setStatus(ChannelDispatcher::Measure);
        }
    };

    class GetStatusHandler : public AbstractRequestHandler
    {
    public:
        GetStatusHandler(ChannelDispatcher* dsp, int ch): AbstractRequestHandler{dsp, ch}{}
        void handleResponse(QStringList& messageList) override final
        {
            if( messageList.at(requestState) == "ok" )
                diapatcher->setStatus( status[messageList.at(1)] );
        }
    };

    class SetRangeHandler : public AbstractRequestHandler
    {
    public:
        SetRangeHandler(ChannelDispatcher* dsp, int ch): AbstractRequestHandler{dsp, ch}{}
        void handleResponse(QStringList& messageList) override final
        {
            if(messageList.size() == requestHasAtLeastOneParameter)
            {
                if( messageList.at(requestState) == "ok" &&
                    messageList.at(requestFirstParameter).contains("range") )
                    diapatcher->setRange( messageList[1].remove("range").toInt() );
            }
        }
    };

    class GetResultHandler : public AbstractRequestHandler
    {
    public:
        GetResultHandler(ChannelDispatcher* dsp, int ch): AbstractRequestHandler{dsp, ch}{}
        void handleResponse(QStringList& messageList) override final
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

                if( result.size() )
                    diapatcher->setValue(result.last());
            }
        }
    };
}

class CommandHandler
{
public:
    CommandHandler(ChannelDispatcher* dsp, int chId)
        : diapatcher{dsp}
    {
        commands["start_measure"] = new StartMeasureHandler{diapatcher, chId};
        commands["stop_measure"] = new StopMeasureHandler{diapatcher, chId};
        commands["get_status"] = new GetStatusHandler{diapatcher, chId};
        commands["set_range"] = new SetRangeHandler{diapatcher, chId};
        commands["get_result"] = new GetResultHandler{diapatcher, chId};
    }

    ~CommandHandler()
    {}

    void handleResponse(const QString &msg)
    {
        auto messageList = msg.trimmed().split(" ");

        if( not messageList.isEmpty() )
        {
            for(auto& data : messageList)
            {
                data.remove(",");
                data.remove("\n");
            }

            if(commands.contains(lastCommandRequested))
            {
                commands[lastCommandRequested]->handleResponse(messageList);
            }
        }
    }

    void setLastCommand(QString const& comm)
    {
        lastCommandRequested = comm;
    }

private:
    QHash<QString, AbstractRequestHandler*> commands;

    QString lastCommandRequested{""};
    ChannelDispatcher* diapatcher{nullptr};
};

ChannelClient::ChannelClient(ChannelDispatcher *parent, int id)
    : dispatcher{parent}
    , commandHandler{new CommandHandler{parent, id}}
    , channelId{id}
{
    connectionTimer.setInterval(2000);
    QObject::connect(&connectionTimer, &QTimer::timeout, [&]()
    {
        if(not connected) run();
    });
    connectionTimer.start();

    statusRequestTimer.setInterval(3000);
    QObject::connect(&statusRequestTimer, &QTimer::timeout, [&]
    {
        auto command = QStringLiteral("get_status");
        commandHandler->setLastCommand( command );
        sendCommandToServer( QString("%1 channel%2\n").arg( command ).arg(channelId) );
    });
    statusRequestTimer.start();

    dataRequestTimer.setInterval(1500);
    QObject::connect(&dataRequestTimer, &QTimer::timeout, [&]
    {
        auto command = QStringLiteral("get_result");
        commandHandler->setLastCommand( command );
        sendCommandToServer( QString("%1 channel%2\n").arg( command ).arg(channelId) );
    });
}

ChannelClient::~ChannelClient()
{
    sendCommandToServer(QString("exit\n"));
    stop();

    clientThread->quit();

    delete commandHandler;
    commandHandler = nullptr;
    
    delete clientThread;
    clientThread = nullptr;
}

void ChannelClient::run()
{
    struct sockaddr_in clientAddres;

    if( clientFileDescriptor = socket(AF_INET, SOCK_STREAM, 0); clientFileDescriptor <= invalideValue )
    {
        qDebug() << "CLIENT: Невозможно открыть сокет клиента ";
        qDebug() << "CLIENT: Функция socket() вернула ошибку " << errno;
        return;
    }
    clientAddres.sin_family = AF_INET;
    clientAddres.sin_port = htons( port );
    clientAddres.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if( connect(clientFileDescriptor, (struct sockaddr *)&clientAddres, sizeof(clientAddres)) <= invalideValue )
    {
        qDebug() << "CLIENT: Невозможно установить соединение с сервером ";
        qDebug() << "CLIENT: Функция connect() вернула ошибку " << errno;
        return;
    }

    connected = true;
    connectionTimer.stop();
    dispatcher->setStatus(ChannelDispatcher::Idle);

    clientThread = QThread::create( [this]() { waitForResponse(); } );
    clientThread->start();

    qDebug() << "CLIENT: Cоединение с сервером установленно";
}

void ChannelClient::sendCommandToServer(const QString & command)
{
    if(connected)
    {
        try {
            string msg = command.toStdString();

            char buf[sizeof(msg)];
            memset(buf, 0, sizeof(msg));

            strncpy(buf, msg.c_str(), sizeof(msg));

            if( send(clientFileDescriptor, buf, sizeof(buf), 0) <= invalideValue )
            {
                qDebug() << "CLIENT: Невозможно отправить сообщение серверу" << errno;
                qDebug() << "CLIENT: Функция send() вернула ошибку " << errno;
                return;
            }

        } catch (const exception& e) {
            qDebug() << "EXCEPTION: ошибка отправки сообщения серверу: " << e.what();
        }
    }
}

void ChannelClient::start()
{
    connectionTimer.start();
}

void ChannelClient::waitForResponse()
{
    char* messageBuffer = new char[maxBufferSize];

    while ( connected )
    {
        fill(messageBuffer, messageBuffer + maxBufferSize, 0);

        auto countOfBytesRead = recv( clientFileDescriptor, messageBuffer, maxBufferSize, 0 );
        if( countOfBytesRead <= 0 )
        {
            qDebug() << "CLIENT: Невозможно обработать сообщение от cервера";
            connected = false;
            dispatcher->setDisconneted();
        }
        else
        {
            if( QString(messageBuffer) == "exit\n" )
            {
                qDebug() << "CLIENT: Сервер подтвердил завершение работы клиента";
                break;
            }

            commandHandler->handleResponse( QString(messageBuffer) );
        }
    }

    delete [] messageBuffer;
}

void ChannelClient::stop()
{
    connected = false;
    qDebug() << "CLIENT: Клиент с файловым дескриптором " << clientFileDescriptor << " завершил сеанс";

    QTimer::singleShot(10, nullptr, [&]{ close(clientFileDescriptor); } );
}

void ChannelClient::request(const QString &command, const QString &argumens)
{
    commandHandler->setLastCommand(command);
    auto message = QString("%1 %2\n").arg(command).arg(argumens);
    sendCommandToServer( message );

    QTimer::singleShot(150, nullptr, [message, command, this]{
    if(command == "start_measure")
    {
        dataRequestTimer.start();
    }
    else if( command == "stop_measure" )
        dataRequestTimer.stop();
    });
}
