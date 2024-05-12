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
    void terminate();

private:

    bool termianted{false};

    unordered_map< int, ClientHandler* > clients;
    int serverfileDescriptor{-1};
};
