#pragma once

#include "ChannelDispatcher.h"
#include <QTimer>
#include <QThread>
#include <thread>

class ChannelClient
{
public :
    explicit ChannelClient(ChannelDispatcher* parent, int channelId);
    ~ChannelClient();

    void sendCommandToServer( QString const& );
    void disconnect();
    void stop();

private:
    void waitForResponse();
    void run();

    QThread* clientThread{nullptr};
    bool connected{false};
    int clientFileDescriptor{-1};
    int channelId{-1};
    QTimer connectionTimer;
    ChannelDispatcher* dispatcher{nullptr};
};
