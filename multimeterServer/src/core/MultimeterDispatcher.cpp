#include "MultimeterDispatcher.h"

#include <bits/stdc++.h>

namespace
{
    static unordered_map< uint8_t, string > channelStateMap{
        {MultimeterDispatcher::Idle, "idle_state"},
        {MultimeterDispatcher::Busy, "busy_state"},
        {MultimeterDispatcher::Error, "error_state"},
        {MultimeterDispatcher::Measuring, "measure_state"}
    };

    constexpr int numberOfChannels{12};
    constexpr int rangeMicro{3};
    constexpr double Pi{3.14159265358979323846264338327950288419717};
    constexpr double Deg2Rad = Pi / 180;
    constexpr int pointCount{380};
    constexpr int teiler{90};
    constexpr int middleSinValue{500};
    constexpr int unos{2};
    constexpr int mili{3};
    constexpr int noDecimals{0};
    constexpr int decimalsForMili{6};
    constexpr int decimalsForMicro{3};
    constexpr int e{10};
    constexpr int invalidValue{-1};

    inline float roundValue(float value, int decimalPlaces)
    {
        return roundf(value * pow(e, decimalPlaces)) / pow(e, decimalPlaces);
    }
}

class MultimeterDispatcher::Channel
{
public:

    Channel( int&& id )
        : channelId{ id }
    {
        srand(static_cast<unsigned int>( std::time(nullptr) * channelId ));
        futureObj = exitSignal.get_future();
        channelThread = thread( &Channel::threadProzess, this );
        pthread_setname_np( channelThread.native_handle(), "MeasuringThread" );
        changeStatusDelay = rand() % 100;
    }

    void threadProzess()
    {
        while (futureObj.wait_for(std::chrono::milliseconds(1)) == future_status::timeout)
        {
            if( measuring )
            {
                measuringData.clear();
                uint64_t xOffset{middleSinValue};
                uint64_t yOffset{middleSinValue};

                for (int i = 0; i < pointCount; i++)
                {
                    if(not measuring)
                    {
                        setStatus();
                        break;
                    }

                    float v = std::sin((i - teiler) * Deg2Rad) * middleSinValue + yOffset;
                    v = v * measuringRangeFactor.at(selectedRange);
                    auto rounding = selectedRange >= unos ? (selectedRange == mili ? decimalsForMili : decimalsForMicro) : 0;
                    v = roundValue(v, rounding);

                    measuringData.push_back( v );
                    this_thread::sleep_for(std::chrono::milliseconds(250));

                    if( i == changeStatusDelay )
                    {
                        setStatus();
                        changeStatusDelay = rand() % 7;
                    }
                }

                measuring = false;
                state = Idle;
            }
        }
    }

    void setStatus()
    {
        state = ChannelState((rand() % 3) + 1);
    }

    vector<float> data()
    {
        return measuringData;
    }

    bool setRange( int r )
    {
        if( state == Idle )
        {
            selectedRange = r;
            return true;
        }

        return false;
    }

    void setChannelState(ChannelState s)
    {
        state = s;
    }

    ChannelState channelState()
    {
       return state;
    }

    bool setMeasuring( bool v )
    {
        if( measuring != v )
        {
           measuring = v;
           state = measuring ? Measuring : Idle;
           return true;
        }
        return false;
    }

    void stop()
    {
        setMeasuring(false);
    }

private:

    vector<float> measuringData;
    vector<float> measuringRangeFactor{ 1000.0, 1.0, 0.001, 0.000001 };
    ChannelState state{ Idle };
    thread channelThread;
    int channelId{ -1 };
    int selectedRange{ 1 };
    int counter{0};
    bool measuring{ false };
    promise <void> exitSignal;
    future<void> futureObj;
    int changeStatusDelay{ -1 };
};

MultimeterDispatcher::MultimeterDispatcher()
{
    initChannels();
}

MultimeterDispatcher::~MultimeterDispatcher()
{
    for( auto const& ch : channels )
    {
        ch.second->stop();
        delete ch.second;
    }

    channels.clear();
}

MultimeterDispatcher *MultimeterDispatcher::instance()
{
    static MultimeterDispatcher inst;
    return &inst;
}

void MultimeterDispatcher::initChannels()
{
    for( int i = 0; i < numberOfChannels; i++ )
    {
        channels[i] = new Channel(move(i));
    }
}

int MultimeterDispatcher::numberOfMultimeterChannel()
{
    return numberOfChannels;
}

string MultimeterDispatcher::dataFromChannel(int&& id)
{
    if ( channels.find( id ) != channels.end() )
    {
        if( auto data = channels.at(id)->data(); data.size() > 1 )
        {
            string convertedData{ "ok"};

            for( auto it = data.end() - 1; it != data.begin(); it--)
            {
                convertedData.append( ", " );
                convertedData.append( to_string( *it ) );
            }

            convertedData.append("\n");

            return convertedData;
        }
    }

    return "fail\n";
}

string MultimeterDispatcher::channelStatus(int&& id)
{ 
    if ( channels.find( id ) != channels.end() )
    {
        string response = "ok, " + channelStateMap.at( channels.at(id)->channelState() );
        response.append("\n");
        return response;
    }
    return "fail\n";
}

string MultimeterDispatcher::startMeasuringInChannel(int&& id)
{
    if( channels.at( id )->setMeasuring( true ) )
    {
        return "ok\n";
    }

    return "fail\n";
}

string MultimeterDispatcher::setRangeForChannel(int&& id, int&& value)
{
    auto range = value > rangeMicro ? rangeMicro : value;

    if(  channels.at( id )->setRange( range ) )
    {
        string request{"ok, range"};
        request.append(to_string(range));
        return request;
    }

    return "fail\n";
}

string MultimeterDispatcher::stopMeasuringInChannel(int&& id)
{
    if( channels.at( id )->setMeasuring( false ) )
        return "ok\n";

    return "fail\n";
}
