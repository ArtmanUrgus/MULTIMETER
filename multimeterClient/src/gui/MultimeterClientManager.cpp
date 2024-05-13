#include "MultimeterClientManager.h"
#include "../domain/ChannelDispatcher.h"
#include "ChannelDataView.h"

#include <QQmlContext>
#include <QTimer>
#include <QColor>

namespace
{
    enum Colors : int
    {
        disabledColor = -1,
        enabledColor = 0,
        activColor,
        busyColor,
        errorColor,
    };

    const QHash<uint8_t, QColor> bgLabel
    {
        {disabledColor, QColor(230, 230, 230)},
        {enabledColor, QColor(230, 230, 230)},
        {activColor, QColor(230, 219, 236)},
        {busyColor, QColor(255, 204, 0)},
        {errorColor, Qt::red},
    };
    const QHash<uint8_t, QColor> fgLabel
    {
        {disabledColor, QColor(130, 130, 130)},
        {enabledColor, QColor(130, 130, 130)},
        {activColor, QColor(100, 64,  134)},
        {busyColor, QColor(168, 136, 0)},
        {errorColor, Qt::white},
    };
    const QHash<uint8_t, QColor> bgRange
    {
        {disabledColor, QColor(204, 204, 204)},
        {enabledColor, QColor(196, 167, 211)},
        {activColor, QColor(204, 204, 204)},
        {busyColor, QColor(204, 204, 204)},
        {errorColor, QColor(204, 204, 204)},
    };
    const QHash<uint8_t, QColor> fgRange
    {
        {disabledColor, QColor(128, 128, 128)},
        {enabledColor, QColor(117, 58, 142)},
        {activColor, QColor(128, 128, 128)},
        {busyColor, QColor(128, 128, 128)},
        {errorColor, QColor(128, 128, 128)},
    };
    const QHash<uint8_t, QColor> bgValue
    {
        {disabledColor, QColor(230, 230, 230)},
        {enabledColor, QColor(230, 230, 230)},
        {activColor, QColor(117, 58, 142)},
        {busyColor, QColor(255, 230, 127)},
        {errorColor, QColor(Qt::black)},
    };
    const QHash<uint8_t, QColor> fgValue
    {
        {disabledColor, QColor(130, 130, 130)},
        {enabledColor, QColor(100, 64,  134)},
        {activColor, QColor(223, 214, 243)},
        {busyColor, QColor(161, 131, 7)},
        {errorColor, QColor(Qt::red)},
    };

    const QHash<uint8_t, QString> sourceForState
    {
        {ChannelDispatcher::Disconnected, "qrc:/img/disconn.svg" },
        {ChannelDispatcher::Idle, "qrc:/img/idle.svg" },
        {ChannelDispatcher::Measure, "qrc:/img/measure.svg" },
        {ChannelDispatcher::Busy, "qrc:/img/busy.svg" },
        {ChannelDispatcher::Error, "qrc:/img/error.svg" },
    };


    constexpr int maxChannelNumber{12};
    const QString mainUrl{ "qrc:/qml/main.qml" };

    enum QmlControls : uint8_t
    {
        ChannelControl = 0,
        ChannelLabel,
        MeasureButton,
        RangeSelector,
        ValueBox,
        StateBox,
    };
}

MultimeterClientManager::MultimeterClientManager(QGuiApplication *app, QObject *parent)
    : QObject(parent)
    , guiApp{app}
{
    initChannels();
    qmlRegisterType<ChannelDataView>("org.ChannelData", 1, 0, "DataView");

    const QUrl url(mainUrl);

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, guiApp, [url](QObject *obj, const QUrl &objUrl)
    {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);

    }, Qt::QueuedConnection);

    engine.load(url);

    QTimer::singleShot(1000, this, [&]()
    {
        initControls();
    });
}

MultimeterClientManager::~MultimeterClientManager()
{
    for( auto& ch : channels )
    {
        ch->close();
    }
}

void MultimeterClientManager::init()
{}

void MultimeterClientManager::setStatusForChannel(int channel, int status)
{
    if( channel >= 0 && channel < qmlControl.size() )
    {
        qmlControl.at(channel)[ChannelLabel]->setProperty("bgcolor", bgLabel[status] );
        qmlControl.at(channel)[ChannelLabel]->setProperty("fgcolor", fgLabel[status] );

        qmlControl.at(channel)[ValueBox]->setProperty("bgcolor", bgValue[status] );
        qmlControl.at(channel)[ValueBox]->setProperty("fgcolor", fgValue[status] );

        qmlControl.at(channel)[RangeSelector]->setProperty("bgcolor", bgRange[status] );
        qmlControl.at(channel)[RangeSelector]->setProperty("fgcolor", fgRange[status] );
        qmlControl.at(channel)[RangeSelector]->setProperty("channelState", status );

        qmlControl.at(channel)[StateBox]->setProperty("source", sourceForState[status] );
        qmlControl.at(channel)[StateBox]->setProperty("bgcolor", bgLabel[status] );

        qmlControl.at(channel)[MeasureButton]->setProperty("channelState", status );
        qmlControl.at(channel)[MeasureButton]->setProperty("switched", status >= ChannelDispatcher::Measure ? true : false );
    }
}

void MultimeterClientManager::setValueForChannel(int channel, double value)
{
    if( channel >= 0 && channel < qmlControl.size() )
    {
        qmlControl.at(channel)[ValueBox]->setProperty("value",
                                                      QString::number(value, 'f',
                                                      channels.at(channel)->range() < 2 ? 1 :
                                                              (channels.at(channel)->range() == 3 ? 6 : 3) ) );
    }
}

void MultimeterClientManager::setRangeForChannel(int channel, int value)
{
    if( channel >= 0 && channel < qmlControl.size() )
    {
        qmlControl.at(channel)[RangeSelector]->setProperty("range", QString::number(value) );
    }
}

void MultimeterClientManager::submitRange(int channel, int value)
{
    if(channels.at(channel)->state() != ChannelDispatcher::Disconnected )
    {
        if( channel >= 0 && channel < channels.size() )
        {
            channels.at(channel)->submitRangeChangeRequest(value);
        }
    }
}

void MultimeterClientManager::submitMeasure(int channel, bool buttonState)
{
    if(channels.at(channel)->state() != ChannelDispatcher::Disconnected )
    {
        if( channel >= 0 && channel < channels.size() )
        {
            channels.at(channel)->submitMessureChangeRequest(buttonState);
        }
    }
}

void MultimeterClientManager::initChannels()
{
    for( int i = 0; i < maxChannelNumber; i++ )
    {
        channels.append(new ChannelDispatcher{i});

        connect(channels.last(), &ChannelDispatcher::statusChanged,
                this, &MultimeterClientManager::setStatusForChannel);

        connect(channels.last(), &ChannelDispatcher::rangeChanged,
                this, &MultimeterClientManager::setRangeForChannel);

        connect(channels.last(), &ChannelDispatcher::valueChanged,
                this, &MultimeterClientManager::setValueForChannel);
    }
}

void MultimeterClientManager::initControls()
{
    rootItems = engine.rootObjects();

    if( auto mainWin = qobject_cast<QQuickWindow*>(rootItems.at(0)) )
    {
        mainWindow = mainWin;
    }

    auto channelsList = mainWindow->findChild<QObject*>("ChannelsList");
    auto contentItem = channelsList->property("contentItem").value<QQuickItem *>();
    auto contentItemChildren = contentItem->childItems();

    for (auto childItem: contentItemChildren )
    {
        if (childItem->objectName().contains("Channel"))
        {
            QHash<int, QObject*> control{};

            control[ChannelControl] = childItem;
            control[ChannelLabel] = childItem->findChild<QObject*>("ChannelLabel");
            control[MeasureButton] = childItem->findChild<QObject*>("MeasureButton");
            control[RangeSelector] = childItem->findChild<QObject*>("RangeSelector");
            control[ValueBox] = childItem->findChild<QObject*>("ValueBox");
            control[StateBox] = childItem->findChild<QObject*>("StateBox");

            qmlControl.append(control);

            if( control[MeasureButton] )
            {
                QObject::connect( control[MeasureButton], SIGNAL( buttonReleased(int, bool) ),
                                  this, SLOT( submitMeasure(int, bool) ) );
            }

            if( control[RangeSelector] )
            {
                QObject::connect( control[RangeSelector], SIGNAL( selectorChangedRange(int, int) ),
                                  this, SLOT( submitRange(int, int) ) );
            }
        }
    }
}
