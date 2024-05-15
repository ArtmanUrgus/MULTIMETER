#pragma once

#include <QObject>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QVector>
#include <QQuickWindow>

class ChannelDispatcher;

class MultimeterClientManager : public QObject
{
    Q_OBJECT

public:
    explicit MultimeterClientManager(QGuiApplication *app, QObject* parent = nullptr);
    ~MultimeterClientManager();

    void init();

    void setStatusForChannel(int channel, int state);
    void setValueForChannel(int channel, double value);
    void setRangeForChannel(int channel, int value);

public slots:
    void submitRange(int channel, int value);
    void submitMeasure(int channel, bool buttonState);

private:
    void initChannels();
    void initControls();

    QGuiApplication* guiApp{nullptr};
    QQuickWindow* mainWindow{nullptr};
    QQmlApplicationEngine engine;

    QVector<ChannelDispatcher*> channels;

    // Агрегаты следующих контейнеров не требуют явного удаления в деструкторе
    // потому, что являются дочерними объектами QQuickWindow и будут удалены по зарытию окна.

    QVector<QHash<int, QObject*>> qmlControl;
    QList<QObject*> rootItems;
};
