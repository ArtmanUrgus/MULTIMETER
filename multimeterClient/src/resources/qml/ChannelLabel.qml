import QtQuick 2.0

Item {
    objectName: "ChannelLabel"

    id: channelLabel
    width: 82; height: 36;

    property string bgcolor: "#e6e6e6"
    property string fgcolor: "#848484"

    property int channelid: -1

    Rectangle {
        id: labelBG
        anchors.fill: parent
        radius: 12

        Binding {
            target: labelBG
            property: "color"
            value: channelLabel.bgcolor
        }

        Rectangle {
            width: 16;
            x: parent.width - 16
            height: parent.height
            color: parent.color
        }
    }

    Text {
        id: channelLabelText
        width: parent.width - 24
        x: 12
        horizontalAlignment: Text.AlignHCenter
        text: channelLabel.channelid + 1
        font.family: "Ubuntu"
        font.weight: Font.DemiBold
        font.pixelSize: 28

        Binding {
            target: channelLabelText
            property: "color"
            value: channelLabel.fgcolor
        }
    }
}
