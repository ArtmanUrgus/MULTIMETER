#pragma once

#include <string>
#include <vector>

using namespace std;

class RequestHandler
{
public:
    RequestHandler();
    ~RequestHandler();

    string handleCommand( string&& value );

private:
    void splitMessage( string&& value );

    vector<string> data;
};

