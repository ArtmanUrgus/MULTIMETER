#pragma once

#include <QQuickPaintedItem>
#include <QQuickItem>

class ChannelDataView : public QQuickPaintedItem
{
public:
    ChannelDataView(QQuickItem *parent = nullptr);

     virtual void paint(QPainter *painter) override final;
};

