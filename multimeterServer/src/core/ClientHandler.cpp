#include "ClientHandler.h"
#include "RequestHandler.h"

#include <sys/socket.h>
#include <errno.h>
#include <assert.h>
#include <iostream>
#include <unistd.h>
#include <string.h>

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

void ClientHandler::terminateConnection()
{
    terminated = true;
}

void ClientHandler::run()
{
    while ( not terminated )
    {
        cout << "SERVER: Ожидание входящих сообщений от клиентов" << endl;

        char messageBuffer[65546];
        auto countOfRytesRead = recv( clientFileDescriptor, messageBuffer, 1024, 0 );

        if( countOfRytesRead <= 0 )
        {
            cout << "SERVER: Невозможно обработать сообщение от клиента с файловым дескриптором"
                 << clientFileDescriptor << endl;
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

            cout << "SERVER: Клиенту отправлено сообщение " << messageBuffer << endl;

            send( clientFileDescriptor, messageBuffer, countOfRytesRead, 0 );
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
    std::cout << "SERVER: Клиент с файловым дескриптором " << clientFileDescriptor << " завершил сеанс" << std::endl;

    char message[] = "exit\n";
    send( clientFileDescriptor, message, sizeof(message), 0 );
    close( clientFileDescriptor );
}
