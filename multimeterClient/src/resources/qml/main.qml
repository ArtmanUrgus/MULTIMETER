import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15

Window {
    width: 954
    height: 674
    maximumHeight: height
    maximumWidth: width
    minimumHeight: height
    minimumWidth: width
    visible: true
    title: qsTr("Multimeter")


    Header{
        objectName: "Header"
        x: 30; y: 24;
    }

    Item {
        x: 30; y: 48;
        width: parent.width - 50
        height: parent.height - 160

        ListModel {
            id: actionModel

            ListElement { name: "Channel_1"; channel_id: "0" }
            ListElement { name: "Channel_2"; channel_id: "1" }
            ListElement { name: "Channel_3"; channel_id: "2" }
            ListElement { name: "Channel_4"; channel_id: "3" }
            ListElement { name: "Channel_5"; channel_id: "4" }
            ListElement { name: "Channel_6"; channel_id: "5" }
            ListElement { name: "Channel_7"; channel_id: "6" }
            ListElement { name: "Channel_8"; channel_id: "7" }
            ListElement { name: "Channel_9"; channel_id: "8" }
            ListElement { name: "Channel_10"; channel_id: "9" }
            ListElement { name: "Channel_11"; channel_id: "10" }
            ListElement { name: "Channel_12"; channel_id: "11" }
        }

        ListView {
            id: list
            objectName: "ChannelsList"

            anchors.fill: parent
            x: 0; y: 0;
            spacing: 6

            clip: true
            model: actionModel

            displayMarginBeginning : 10
            displayMarginEnd : 10

            boundsBehavior: Flickable.StopAtBounds

            ScrollBar.vertical: ScrollBar {
                active: true
            }

            delegate: Channel {
                id: channelDelegate

                required property string name
                required property int channel_id

                channelid: channel_id
                objectName: name

                x: 0; y: 48;
            }
        }
    }

    MenuBox{
        x: 30; y: 590;
    }
}
