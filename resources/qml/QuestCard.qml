import QtQuick
import QtQuick.Controls

Item {
    id: root
    height: 130
    
    property string title
    property string desc
    property int priority
    property int xp
    property string projectName
    property string dueDate

    signal complete()
    signal delete()

    Rectangle {
        id: cardBg
        anchors.fill: parent
        radius: 8
        color: "#FDF5E6" // Old Lace color
        border.color: "#D7CCC8"; border.width: 1

        // 优先级色条
        Rectangle {
            width: 6; height: parent.height - 16
            anchors.left: parent.left; anchors.verticalCenter: parent.verticalCenter
            anchors.leftMargin: 6; radius: 3
            color: {
                if(priority === 2) return "#FF5252" // High
                if(priority === 1) return "#FFB74D" // Med
                return "#66BB6A" // Low
            }
        }

        // 标题与描述
        Column {
            anchors.left: parent.left; anchors.leftMargin: 24
            anchors.right: parent.right; anchors.rightMargin: 70
            anchors.verticalCenter: parent.verticalCenter
            spacing: 4

            Text { 
                text: title
                font.bold: true; font.pixelSize: 16
                color: "#3E2723"; elide: Text.ElideRight; width: parent.width
            }
            Text { 
                text: desc
                font.pixelSize: 12; color: "#5D4037"
                elide: Text.ElideRight; width: parent.width
                maximumLineCount: 2
            }
            
            // 元数据行
            Row {
                spacing: 10
                visible: projectName !== "" || dueDate !== ""
                Text { 
                    visible: projectName !== ""
                    text: "📁 " + projectName
                    font.pixelSize: 10; color: "#8D6E63"
                }
                Text {
                    visible: dueDate !== ""
                    text: "📅 " + dueDate
                    font.pixelSize: 10; color: "#8D6E63"
                }
            }
        }

        // XP 徽章
        Rectangle {
            anchors.right: parent.right; anchors.top: parent.top; anchors.margins: 8
            width: 60; height: 24; radius: 12
            color: "#FFF8E1"; border.color: "#FFB74D"
            
            Row {
                anchors.centerIn: parent
                spacing: 2
                Text { text: "🏆"; font.pixelSize: 10 }
                Text { text: xp; font.bold: true; font.pixelSize: 12; color: "#F57C00" }
            }
        }
        
        // 长按提示
        Text {
            anchors.right: parent.right; anchors.bottom: parent.bottom; anchors.margins: 8
            text: "Hold to Clear"
            font.pixelSize: 10; color: "#aaa"
        }

        // === 长按交互 ===
        Rectangle {
            id: progressOverlay
            anchors.fill: parent
            color: "#4CAF50"
            opacity: 0.3
            width: 0 // Animated
            visible: width > 0
        }

        MouseArea {
            anchors.fill: parent
            onPressAndHold: pressAnim.start()
            onReleased: {
                pressAnim.stop()
                progressOverlay.width = 0
            }
            // 右键删除 (Desktop)
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            onClicked: (mouse) => {
                if(mouse.button === Qt.RightButton) root.delete()
            }
        }
        
        PropertyAnimation {
            id: pressAnim
            target: progressOverlay
            property: "width"
            to: root.width
            duration: 800
            onFinished: {
                progressOverlay.width = 0
                root.complete()
            }
        }
    }
}