import QtQuick 2.0

Item {
    objectName: "ValueBox"

    id: valueVox
    width: 310; height: 36;

    property string bgcolor: "#e6e6e6"
    property string fgcolor: "#e6e6e6"
    property string value: "0.0"
    property int channelId: -1

    Rectangle{
        id: bg
        anchors.fill: parent
        color: "#e6e6e6"

        Binding{
            target: bg
            property: "color"
            value: valueVox.bgcolor
        }
    }

    Text {
        id: value
        anchors.fill: parent
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        text: "0.0"
        font.family: "Ubuntu Condensed"
        font.weight: Font.Bold
        font.pixelSize: 28

        Binding{
            target: value
            property: "color"
            value: valueVox.fgcolor
        }

        Binding{
            target: value
            property: "text"
            value: valueVox.value
        }
    }

    MouseArea {
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor

        onClicked: {
        }
    }
}
