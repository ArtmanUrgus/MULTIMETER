#include "MultimeterServer.h"
#include "MultimeterDispatcher.h"
#include "ClientHandler.h"

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
#include <iostream>
#include <string.h>
#include <errno.h>

namespace
{
    constexpr int maxNumberOfPendingConnections{100};
    constexpr int invalideValue{-1};
    constexpr uint16_t port{11047};
}

MultimeterServer::MultimeterServer()
{

}

MultimeterServer::~MultimeterServer()
{
    terminate();
    sleep(1);
    close(serverfileDescriptor);

    for(auto const& c : clients)
    {
        c.second->terminateConnection();
        sleep(1);
        c.second->stop();
        delete c.second;
    }

    clients.clear();
}

void MultimeterServer::terminate()
{
    termianted = true;
}

void MultimeterServer::run()
{
    char messageBuffer[1024];

    int clientFileDescriptor{invalideValue};
    int countOfRytesRead{invalideValue};

    struct sockaddr_in serverAddres;

    if(serverfileDescriptor = socket(AF_INET, SOCK_STREAM, 0); serverfileDescriptor <= invalideValue)
    {
        cout << "SERVER: Невозможно открыть сокет сервера " << endl;
        cout << "SERVER: Функция socket() вернула ошибку " << errno << endl;
        exit(1);
    }

    serverAddres.sin_family = AF_INET;
    serverAddres.sin_port = htons( port );
    serverAddres.sin_addr.s_addr = htonl(INADDR_ANY);

    if( bind(serverfileDescriptor, (struct sockaddr *)&serverAddres, sizeof(serverAddres)) <= invalideValue )
    {
        cout << "SERVER: Невозможно установить соединение для сокета сервера " << serverfileDescriptor << endl;
        cout << "SERVER: Функция bind() вернула ошибку " << errno << endl;
        exit(1);
    }

    if( listen(serverfileDescriptor, maxNumberOfPendingConnections) <= invalideValue )
    {
        cout << "SERVER: Невозможно обеспечить соединение клентов к дескриптору " << serverfileDescriptor << endl;
        cout << "SERVER: Функция listen() вернула ошибку " << errno << endl;
        exit(1);
    }

    cout << "SERVER: Службы unix-domain-socket-сервера успешно запущены и ожидают соединения с клиентом" << endl;

    while( not termianted )
    {
        if( clientFileDescriptor = accept(serverfileDescriptor, nullptr, nullptr );
            clientFileDescriptor > invalideValue )
        {
            clients[ clientFileDescriptor ] = new ClientHandler{ clientFileDescriptor };
        }
    }

    close(serverfileDescriptor);
}
