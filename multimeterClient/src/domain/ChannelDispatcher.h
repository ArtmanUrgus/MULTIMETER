#pragma once

#include <QObject>
#include <QVector>
#include <QColor>
#include <QTimer>

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

    void handleMessageFromChannel(QString const& msg);

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
    void setValue(QVector<float> const&);

    void testSlot();

signals:
    void rangeChanged(int channel, int range);
    void statusChanged(int channel, int state);
    void valueChanged(int channel, float value);

    void termiante();

private slots:
    void disconectChannel();

private:
    int chId{-1};
    int channelRange{Unus};
    int channelStatus{Disconnected};
    QVector<float> channelResultValue;
    ChannelClient* channelClient{nullptr};
    QString lastCommandRequested{""};
    QTimer statusRequestTimer;
    QTimer dataRequestTimer;
};

