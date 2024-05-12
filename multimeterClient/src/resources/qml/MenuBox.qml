import QtQuick 2.0

Item {
    id: menu
    property int channelid: -1
    width: 890; height: 42;

    Rectangle {
        anchors.fill: parent
        radius: 10
        color: "#e6e6e6"
    }

    Text {
        width: 160
        height: parent.height
        x: 0
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        text: "Add Channel"
        font.family: "Ubuntu Condensed"
        font.weight: Font.Bold
        font.pixelSize: 26
        color: "#cccccc"
    }

    Rectangle {
        width: 6
        x: 160; y: 0;
        height: parent.height
        color: "#fff"
    }

    Rectangle {
        x: 166; y: 0;
        width: 122
        height: parent.height
        color: "#c3a9d6"

        Text {
            width: 122
            height: parent.height
            x: 0
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            text: "Reset all"
            font.family: "Ubuntu Condensed"
            font.weight: Font.Bold
            font.pixelSize: 26
            color: "#743a8f"
        }
    }

    Rectangle {
        width: 6
        x: 288; y: 0;
        height: parent.height
        color: "#fff"
    }

}
