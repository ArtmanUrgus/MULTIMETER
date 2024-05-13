#pragma once

#include "ClientHandler.h"

#include <thread>
#include <unordered_map>

using namespace std;

class MultimeterServer
{
public:
    explicit MultimeterServer();
    virtual ~MultimeterServer();

    void run();
    void disconnect();

private:
    unordered_map< int, ClientHandler* > clients;

    bool connected{false};

    int serverfileDescriptor{-1};
};
