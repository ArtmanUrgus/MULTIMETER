import QtQuick 2.0

Item {
    objectName: "StateBox"

    id: stateBox
    width: 92; height: 36;
    property string bgcolor: "#e6e6e6"
    property string source: "qrc:/img/disconn.svg"

    Rectangle {
        id: stateBG
        anchors.fill: parent
        radius: 12

        Binding {
            target: stateBG
            property: "color"
            value: stateBox.bgcolor
        }

        Rectangle {
            width: 16;
            x: 0
            height: parent.height
            color: parent.color
        }
    }

    Image {
        x: parent.width - 36 ; y: 4;
        source: stateBox.source
    }
}
