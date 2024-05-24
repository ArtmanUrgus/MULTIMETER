#pragma once

#include <iostream>
#include <unordered_map>

#define multimeter (MultimeterDispatcher::instance())

using namespace std;

class MultimeterDispatcher
{
public:
    enum ChannelState : uint8_t
    {
        Idle,
        Measuring,
        Busy,
        Error
    };

    enum class ChannelError : uint8_t
    {
        PowerOff,
        ChannelMuted,
        UndefinedValue
    };

    static MultimeterDispatcher* instance();

    void initChannels();
    int numberOfMultimeterChannel();

    string dataFromChannel( int&& id );
    string channelStatus( int&& id );
    string startMeasuringInChannel( int&& id );
    string setRangeForChannel( int&& id, int&& range );
    string stopMeasuringInChannel( int&& id );

    MultimeterDispatcher& operator=(const MultimeterDispatcher&) = delete;

private:
    MultimeterDispatcher();
    ~MultimeterDispatcher();

    class Channel;

    unordered_map<int, Channel*> channels;
};
