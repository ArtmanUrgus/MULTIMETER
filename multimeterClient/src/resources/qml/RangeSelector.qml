import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    objectName: "RangeSelector"

    id: rangeSelector
    width: 308; height: 36;
    clip: true

    signal selectorChangedRange(int channelId, int rangeId)

    property int channelId: -1
    property int channelState: 5
    property int range: 1
    property string bgcolor: "#cccccc"
    property string fgcolor: "#808080"

    color: bgcolor

    Rectangle{
        id: rangeHighlight
        width: 124;
        height: parent.height;
        x: 62 * rangeSelector.range; // 0, 62, 126, 182
        y: 0;
        color: rangeSelector.fgcolor

        Rectangle{
            id: hyphen
            width: 12; height: 4;
            x: 56;  y: 15;
            color: "#cccccc"

            Binding{
                target: hyphen
                property: "color"
                value: rangeSelector.bgcolor
            }
        }

        Binding{
            target: rangeHighlight
            property: "color"
            value: rangeSelector.fgcolor
        }
    }

    Item {
        anchors.fill: parent

        ListModel {
            id: rangeModel

            ListElement { name: "mega"; basis: "10"; factor: "6" }
            ListElement { name: "kilo"; basis: "10"; factor: "3" }
            ListElement { name: "unos"; basis: "1.0"; factor: "0" }
            ListElement { name: "mili"; basis: "10"; factor: "-3" }
            ListElement { name: "micro"; basis: "10"; factor: "-6" }
        }

        ListView {
            id: rangeList
            objectName: "RangeList"
            orientation: ListView.Horizontal

            anchors.fill: parent
            spacing: 2

            clip: true
            model: rangeModel

            displayMarginBeginning : 10
            displayMarginEnd : 10

            boundsBehavior: Flickable.StopAtBounds

            delegate: RangeItem {
                id: rangeItemDelegate

                required property int index
                required property string name
                required property string basis
                required property string factor

                objectName: name
                basisLabel: basis
                factorLabel: factor
                ownRange: index
                fgcolor: rangeSelector.fgcolor
                bgcolor: rangeSelector.bgcolor

                x: 0; y: 0;
            }
        }
    }
}
