import QtQuick
import QtQuick.Controls

Item {
    id: root
    property string name
    property string desc
    property string colorCode
    property double progress
    property string taskCount
    signal delete()

    Rectangle {
        anchors.fill: parent
        color: "#1e1e1e"
        radius: 8
        border.color: "#333"

        // ÑÕÉ«Ìõ
        Rectangle {
            width: parent.width; height: 6
            anchors.top: parent.top
            color: colorCode === "" ? "#888" : colorCode
            radius: 2
        }

        Column {
            anchors.fill: parent; anchors.margins: 12
            anchors.topMargin: 18
            spacing: 6

            Text { text: name; color: "#fff"; font.bold: true; elide: Text.ElideRight; width: parent.width }
            Text { text: desc; color: "#888"; font.pixelSize: 10; elide: Text.ElideRight; width: parent.width; maximumLineCount: 2 }
            
            Item { height: 10; width: 1 } // Spacer

            Text { text: taskCount + " Tasks"; color: "#aaa"; font.pixelSize: 10 }
            
            // Progress Bar
            Rectangle {
                width: parent.width; height: 4; color: "#333"; radius: 2
                Rectangle {
                    width: parent.width * progress
                    height: parent.height; color: colorCode === "" ? "#aaa" : colorCode
                    radius: 2
                }
            }
        }
        
        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.RightButton
            onClicked: root.delete()
        }
    }
}