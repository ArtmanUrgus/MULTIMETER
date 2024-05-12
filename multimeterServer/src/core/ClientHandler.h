#pragma once

#include <iostream>
#include <thread>

using namespace std;

class ClientHandler
{
public:
    ClientHandler(int id);
    ~ClientHandler();

    void terminateConnection();
    void run();
    void stop();

private:
    std::thread clientThread;
    int clientFileDescriptor{-1};
    bool terminated{false};
};
