#include "MultimeterServer.h"
#include "MultimeterDispatcher.h"
#include "ClientHandler.h"

#include <unistd.h>
#include <netinet/in.h>

namespace
{
    constexpr int maxNumberOfPendingConnections{100};
    constexpr int maxBufferSize{65546};
    constexpr int invalideValue{-1};
    constexpr uint16_t port{11047};
}

MultimeterServer::MultimeterServer()
{}

MultimeterServer::~MultimeterServer()
{
    disconnect();
    sleep(1);
    close(serverfileDescriptor);

    for(auto const& c : clients)
    {
        c.second->disconnect();
        sleep(1);
        c.second->stop();
        delete c.second;
    }

    clients.clear();
}

void MultimeterServer::disconnect()
{
    connected = false;
}

void MultimeterServer::run()
{
    char messageBuffer[maxBufferSize];

    int clientFileDescriptor{invalideValue};
    int countOfRytesRead{invalideValue};

    struct sockaddr_in serverAddres;

    if(serverfileDescriptor = socket(AF_INET, SOCK_STREAM, 0); serverfileDescriptor <= invalideValue)
    {
        cout << "SERVER: Невозможно открыть сокет сервера " << endl;
        cout << "SERVER: Функция socket() вернула ошибку " << errno << endl;
    }

    serverAddres.sin_family = AF_INET;
    serverAddres.sin_port = htons( port );
    serverAddres.sin_addr.s_addr = htonl(INADDR_ANY);

    while( not connected )
    {
        if( bind(serverfileDescriptor, (struct sockaddr *)&serverAddres, sizeof(serverAddres)) <= invalideValue )
        {
            cout << "SERVER: Невозможно установить соединение для сокета сервера c дескриптором " << serverfileDescriptor << endl;
            cout << "SERVER: Функция bind() вернула ошибку " << errno << endl;
        }
        else
            connected = true;

        cout << "SERVER: Службы unix-domain-socket-сервера успешно запущены и ожидают соединения с клиентом" << endl;
    }

    auto bindResult = listen(serverfileDescriptor, maxNumberOfPendingConnections);
    if( bindResult <= invalideValue )
    {
        cout << "SERVER: Невозможно обеспечить соединение клентов к дескриптору " << serverfileDescriptor << endl;
        cout << "SERVER: Функция listen() вернула ошибку " << errno << endl;
    }  

    while( connected )
    {
        if( clientFileDescriptor = accept(serverfileDescriptor, nullptr, nullptr );
            clientFileDescriptor > invalideValue )
        {
            clients[ clientFileDescriptor ] = new ClientHandler{ clientFileDescriptor };
        }
    }

    close(serverfileDescriptor);
}
