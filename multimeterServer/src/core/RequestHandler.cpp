#include "RequestHandler.h"
#include "MultimeterDispatcher.h"

#include <bits/stdc++.h>

using  messageHandler = string (*)(int);

namespace
{
    constexpr int invalidValue{-1};

    void printArguments(vector<string> const& argumets)
    {
        for( auto const& v : argumets )
        {
            cout << "SERVER: список аргументов команды: " << v << " ";
        }
    }

    string clearData( string const& value )
    {
        string v{value};

        v.erase(remove(v.begin(), v.end(), '\n'), v.cend());
        v.erase(remove(v.begin(), v.end(), ','), v.cend());

        return v;
    }

    int separateNumberValueFromParameter(string const& parameter, string const& pattern)
    {
        string ch{ pattern };
        string v{ clearData( parameter ) };

        size_t start_pos = v.find(ch);

        if(start_pos == string::npos)
            return -1;

        v.replace(start_pos, ch.length(), "");

        try
        {
            auto number = stoi(v);
            cout << "SERVER: Запрос выполнен для номера канала " << number << endl;
            return number;

        }  catch (const exception& e) {
            cout << "EXCEPTION: ошибка преобразования string to int во время получения номера канала: " <<
                         e.what() << endl;
        }

        return -1;
    }

    string start_measure(int channelId)
    {
        return multimeter->startMeasuringInChannel(move(channelId));
    }

    string stop_measure(int channelId)
    {
        return multimeter->stopMeasuringInChannel(move(channelId));
    }

    string get_status(int channelId)
    {
        return multimeter->channelStatus(move(channelId));
    }

    string get_result(int channelId)
    {
        return multimeter->dataFromChannel(move(channelId));
    }

    static unordered_map<string, messageHandler> commands{
        {"start_measure", start_measure},
        {"stop_measure", stop_measure},
        {"get_status", get_status},
        {"get_result", get_result},
    };
}

RequestHandler::RequestHandler()
{}

RequestHandler::~RequestHandler()
{}

string RequestHandler::handleCommand(string&& msg)
{
    splitMessage( move(msg) );

    auto command = data[0];
    data.erase( data.begin() );

    if(command == "exit\n")
    {
       return "join";
    }

    if( auto it = commands.find( command ); it != commands.end() )
    {
        cout << "SERVER: получена команда: " << command << endl;

        if( data.size() )
        {
            auto channelId = separateNumberValueFromParameter(data.at(0), "channel");

            if( channelId != invalidValue)
            {
               return commands.at(command)(channelId);
            }
        }
    }
    else if( command == "set_range" )
    {
        if( data.size() )
        {
            auto channelId = separateNumberValueFromParameter(data.at(0), "channel");
            auto rangeId = separateNumberValueFromParameter(data.at(1), "range");

            if( channelId != invalidValue && rangeId != invalidValue )
            {
                return multimeter->setRangeForChannel(move(channelId), move(rangeId) );
            }
        }
    }

    return "fail\n";
}

void RequestHandler::splitMessage(string&& value)
{
    data.clear();

    auto pos = value.find( ' ' );
    auto currentPos{0};

    while( pos != string::npos )
    {
        data.push_back( clearData( value.substr( currentPos, pos - currentPos ) ) );
        currentPos = pos + 1;
        pos = value.find( ' ', currentPos );
    }

    data.push_back( clearData( value.substr( currentPos, std::min( pos, value.size() ) - currentPos + 1 ) ) );
}
