#include "ChannelClient.h"
#include "ChannelDispatcher.h"

#include <sys/eventfd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <unistd.h>
#include <poll.h>
#include <netinet/in.h>
#include <cassert>
#include <vector>
#include <unordered_map>
#include <string.h>
#include <errno.h>

#include <QDebug>
#include <QObject>
#include <QCoreApplication>

namespace
{
    constexpr uint16_t port{11047};
    constexpr int invalideValue{-1};
}

ChannelClient::ChannelClient(ChannelDispatcher *parent, int id)
    : dispatcher{parent}
    , channelId(id)
{
    connectionTimer.setInterval(2000);
    QObject::connect(&connectionTimer, &QTimer::timeout, &connectionTimer, [&]()
    {
        if(not connected)
        {
            run();
        }
    });

    connectionTimer.start();
}

ChannelClient::~ChannelClient()
{
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
            std::string msg = command.toStdString();

            char buf[sizeof(msg)];
            memset(buf, 0, sizeof(msg));

            strncpy(buf, msg.c_str(), sizeof(msg));

            if( send(clientFileDescriptor, buf, sizeof(buf), 0) <= invalideValue )
            {
                qDebug() << "CLIENT: Невозможно отправить сообщение серверу" << errno;
                qDebug() << "CLIENT: Функция send() вернула ошибку " << errno;
                return;
            }

        } catch (const std::exception& e) {
            qDebug() << "EXCEPTION: ошибка отправки сообщения серверу: " << e.what();
        }
    }
}

void ChannelClient::disconnect()
{
    connected = false;
}

void ChannelClient::waitForResponse()
{
    char* messageBuffer = new char[65546];

    while ( connected )
    {
        std::fill(messageBuffer, messageBuffer + 65546, 0);

        auto countOfBytesRead = recv( clientFileDescriptor, messageBuffer, 65536, 0 );
        if( countOfBytesRead <= 0 )
        {
            qDebug() << "CLIENT: Невозможно обработать сообщение от cервера";
        }
        else
        {
            if( QString(messageBuffer) == "exit\n" )
            {
                qDebug() << "CLIENT: Сервер подтвердил завершение работы клиента";
                break;
            }

            dispatcher->handleMessageFromChannel(QString(messageBuffer));
        }
    }
}

void ChannelClient::stop()
{
    qDebug() << "CLIENT: Клиент с файловым дескриптором " << clientFileDescriptor << " завершил сеанс";
    close(clientFileDescriptor);
}
