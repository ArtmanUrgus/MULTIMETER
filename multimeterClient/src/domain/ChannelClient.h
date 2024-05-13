#pragma once

#include "ChannelDispatcher.h"
#include <QTimer>
#include <QThread>

using namespace std;

class ChannelClient
{
public :
    explicit ChannelClient(ChannelDispatcher* parent, int channelId);
    ~ChannelClient();

    void sendCommandToServer( QString const& );
    void stop();

public slots:
    void start();

private:
    void waitForResponse();
    void run();

    ChannelDispatcher* dispatcher{nullptr};

    QThread* clientThread{nullptr};
    QTimer connectionTimer;

    bool connected{false};

    int clientFileDescriptor{-1};
    int channelId{-1};
};
