import QtQuick 2.0

Item {
    objectName: "MeasureButton"
    id: measuringButton

    property int channelState: -1
    property int channelId: -1
    property bool switched: false
    property string stateColor: "#cccccc"

    width: 74; height: 36;

    signal buttonReleased(int chId, bool buttonState)

    Rectangle{
        id: buttonBackground

        anchors.fill: parent
        radius: 18
        color: switched ? "#000" : "#808080"

        Rectangle{
            id: indicator
            x: measuringButton.switched ? 40 : 4
            y: 4
            width: 28; height: 28;
            radius: 14

            Binding{
                target: indicator
                property: "color"
                value: measuringButton.stateColor
            }
        }

        Text {
            id: statusLabel
            x: measuringButton.switched ? 12 : 36
            y:8
            text: measuringButton.switched ?  qsTr("ON") : qsTr("OFF")
            font.family: "Ubuntu Condensed"
            font.weight: Font.Bold
            font.pixelSize: 18

            Binding{
                target: statusLabel
                property: "color"
                value: measuringButton.stateColor
            }
        }

        MouseArea {
            id: measuringButtonMouseArea
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor

            acceptedButtons: Qt.LeftButton | Qt.RightButton
            hoverEnabled: true

            onReleased: {
                if(channelState >= 0)
                {
                    measuringButton.buttonReleased( measuringButton.channelId, !measuringButton.switched )
                }
            }
        }
    }
}
