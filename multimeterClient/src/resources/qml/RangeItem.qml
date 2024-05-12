import QtQuick 2.0

Item
{
    id: rangeListDelegate

    width: 60; height: 36;

    required property int ownRange
    required property string basisLabel
    required property string factorLabel
    required property string bgcolor
    required property string fgcolor

    Text {
        id: rangeItemValue

        width: 54
        x: 0
        horizontalAlignment: Text.AlignHCenter
        text: rangeListDelegate.basis
        font.family: "Ubuntu Condensed"
        font.weight: Font.Bold
        font.pixelSize: 28
        color: "#808080"

        Binding{
            target: rangeItemValue
            property: "color"
            value: ( rangeListDelegate.ownRange === rangeSelector.range ||
                     rangeListDelegate.ownRange === rangeSelector.range + 1 )
                     ? rangeListDelegate.bgcolor : rangeListDelegate.fgcolor
        }
    }

    Text {
        id: rangeItemFactor

        property int ownRange: 0

        x: 40
        text: rangeListDelegate.factor
        font.family: "Ubuntu Condensed"
        font.weight: Font.Bold
        font.pixelSize: 16
        color: "#808080"
        visible: rangeListDelegate.factor === "0" ? false: true

        Binding{
            target: rangeItemFactor
            property: "color"
            value: ( rangeListDelegate.ownRange === rangeSelector.range ||
                     rangeListDelegate.ownRange === rangeSelector.range + 1 )
                     ? rangeListDelegate.bgcolor : rangeListDelegate.fgcolor
        }
    }

    MouseArea {
        id: rangeItemMouseArea
        x: 0; y: 0; // 0, 62, 126, 182
        width: 62; height: parent.height;
        cursorShape: Qt.PointingHandCursor

        onClicked: {
            if(rangeSelector.channelState === 0){
//                rangeSelector.range = ownRange < 4 ? ownRange : 3
                rangeSelector.selectorChangedRange( rangeSelector.channelId, rangeListDelegate.ownRange )
            }
        }
    }
}
