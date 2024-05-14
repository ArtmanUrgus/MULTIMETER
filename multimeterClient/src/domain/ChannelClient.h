#pragma once

#include "ChannelDispatcher.h"
#include <QTimer>
#include <QThread>

using namespace std;

class CommandHandler;

class ChannelClient
{
public :
    explicit ChannelClient(ChannelDispatcher* parent, int channelId);
    ~ChannelClient();

    void sendCommandToServer( QString const& );
    void stop();

    void request(QString const& command, QString const& argumens);

public slots:
    void start();

private:
    void waitForResponse();
    void run();

    ChannelDispatcher* dispatcher{nullptr};
    CommandHandler* commandHandler{nullptr};

    QThread* clientThread{nullptr};

    QTimer connectionTimer;
    QTimer statusRequestTimer;
    QTimer dataRequestTimer;

    bool connected{false};

    int clientFileDescriptor{-1};
    int channelId{-1};
};
