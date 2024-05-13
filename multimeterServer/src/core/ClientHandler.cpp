#include "ClientHandler.h"
#include "RequestHandler.h"

#include <sys/socket.h>
#include <unistd.h>
#include <string.h>

namespace
{
    constexpr uint16_t port{11047};
    constexpr int maxBufferSize{65546};
    constexpr int invalideValue{-1};
}

ClientHandler::ClientHandler(int id)
    : clientFileDescriptor{id}
{
    clientThread = std::thread( [this]() { run(); } );
    pthread_setname_np( clientThread.native_handle(), "ClientHandler" );
}

ClientHandler::~ClientHandler()
{
    stop();
}

void ClientHandler::disconnect()
{
    connected = false;
}

void ClientHandler::run()
{
    char messageBuffer[maxBufferSize];

    while ( connected )
    {
        cout << "SERVER: Ожидание входящих сообщений от клиентов" << endl;

        auto countOfRytesRead = recv( clientFileDescriptor, messageBuffer, maxBufferSize, 0 );
        if( countOfRytesRead <= 0 )
        {
            cout << "SERVER: Невозможно обработать сообщение от клиента с файловым дескриптором"
                 << clientFileDescriptor << endl;
            stop();
            break;
        }

        RequestHandler command;
        if( auto response = command.handleCommand( messageBuffer ); not response.empty() )
        {
            if(response == "join")
            {
                stop();
                break;
            }

            strncpy( messageBuffer, response.c_str(), sizeof(response) );
            send( clientFileDescriptor, messageBuffer, countOfRytesRead, 0 );

            cout << "SERVER: Клиенту отправлено сообщение " << messageBuffer << endl;
        }
        else
        {
            std::cout << "SERVER: Сервер не смог сформировать ответ на запрос: "
                      << messageBuffer << std::endl;
        }
    }
}

void ClientHandler::stop()
{
    if( connected ) connected = false;

    char message[] = "exit\n";

    send( clientFileDescriptor, message, sizeof(message), 0 );
    close( clientFileDescriptor );

    std::cout << "SERVER: Клиент с файловым дескриптором " << clientFileDescriptor << " завершил сеанс" << std::endl;
}
