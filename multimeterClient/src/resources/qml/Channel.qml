import QtQuick 2.0

Item {
    id: channel
    property int channelid: -1
    width: 886; height: 36;

    ChannelLabel{
        id: channelBox
        channelid: channel.channelid
        x: 0; y: 0;
    }

    MeasureButton{
        id: measureButton
        channelId: channel.channelid
        x: channelBox.width + 6; y: 0;
    }

    RangeSelector{
        id: rangeSelector
        channelId: channel.channelid
        x: measureButton.x + measureButton.width + 6; y: 0
    }

    ValueBox{
        id: valueBox
        channelId: channel.channelid
        x: rangeSelector.x + rangeSelector.width + 6; y: 0
    }

    StateBox{
        id: stateBox
        x: valueBox.x + valueBox.width + 6; y: 0
    }
}
