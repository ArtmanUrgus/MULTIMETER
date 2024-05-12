import QtQuick 2.0

Item{
    width: 886; height: 16;

    Text {
        x: 10
        id: channelLabel
        text: "CHANNEL"
        font.family: "Ubuntu"
        font.weight: Font.Bold
        font.pixelSize: 14
        color: "#9e9e9e"
    }

    Text {
        x: 90
        id: measureLabel
        text: "MEASURE"
        font.family: "Ubuntu"
        font.weight: Font.Bold
        font.pixelSize: 14
        color: "#9e9e9e"
    }

    Text {
        x: 290
        id: rangeLabel
        text: "RANGE"
        font.family: "Ubuntu"
        font.weight: Font.Bold
        font.pixelSize: 14
        color: "#9e9e9e"
    }

    Text {
        x: 580
        id: valueLabel
        text: "CURRENT VALUE"
        font.family: "Ubuntu"
        font.weight: Font.Bold
        font.pixelSize: 14
        color: "#9e9e9e"
    }

    Text {
        x: 820
        id: stateLabel
        text: "STATUS"
        font.family: "Ubuntu"
        font.weight: Font.Bold
        font.pixelSize: 14
        color: "#9e9e9e"
    }
}
