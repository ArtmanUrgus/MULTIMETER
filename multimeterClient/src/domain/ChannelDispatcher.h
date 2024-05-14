#pragma once

#include <QObject>
#include <QVector>
#include <QColor>
#include <QHash>

class ChannelClient;

class ChannelDispatcher : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int channelId READ channelId )
    Q_PROPERTY(int range READ range WRITE setRange)
    Q_PROPERTY(int state READ state WRITE setStatus)
    Q_PROPERTY(float value READ value)

public:
    enum Range : int
    {
        Mega = 0,
        Kilo,
        Unus,
        Mili,
        Micro
    };

    enum State : int
    {
        Disconnected = -1,
        Idle = 0,
        Measure,
        Busy,
        Error,
    };

    ChannelDispatcher(int id, QObject *parent = nullptr);
    ~ChannelDispatcher();

    /* Q_PROPERTIES */
    Q_INVOKABLE int channelId();
    Q_INVOKABLE int range();
    Q_INVOKABLE int state();
    Q_INVOKABLE float value();

    void submitRangeChangeRequest(int);
    void submitMessureChangeRequest(bool);

public slots:
    void setRange(int);
    void setStatus(int);
    void setValue(const float &);
    void setDisconneted();
    void close();

signals:
    void rangeChanged(int channel, int range);
    void statusChanged(int channel, int state);
    void valueChanged(int channel, float value);

private slots:
    void disconectChannel();

private:
    ChannelClient* channelClient{nullptr};

    QVector<float> channelResultValue;

    int chId{-1};
    int channelRange{Kilo};
    int channelStatus{Disconnected};
};

