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
        color: "transparent" // 使用透明背景以显示纹理
        border.color: "#D7CCC8"; border.width: 1
        clip: true

        // 羊皮纸纹理背景
        Image {
            anchors.fill: parent
            source: "qrc:/images/texture_parchment.png"
            fillMode: Image.Tile
            z: 0
        }

        // 优先级色条
        Rectangle {
            width: 6; height: parent.height - 16
            anchors.left: parent.left; anchors.verticalCenter: parent.verticalCenter
            anchors.leftMargin: 6; radius: 3
            z: 1
            color: {
                if(priority === 2) return "#FF5252" // High
                if(priority === 1) return "#FFB74D" // Med
                return "#66BB6A" // Low
            }
        }

        // 完成印章 (任务完成时显示)
        Image {
            id: completedStamp
            source: "qrc:/images/icon_stamp_completed.png"
            width: 100
            height: 100
            anchors.centerIn: parent
            visible: false
            rotation: -15
            z: 50
            opacity: 0.9
        }

        // 标题与描述
        Column {
            anchors.left: parent.left; anchors.leftMargin: 24
            anchors.right: parent.right; anchors.rightMargin: 70
            anchors.verticalCenter: parent.verticalCenter
            spacing: 4
            z: 2

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
            z: 2
            
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
            z: 2
        }

        // === 长按交互 ===
        Rectangle {
            id: progressOverlay
            anchors.fill: parent
            color: "#4CAF50"
            opacity: 0.3
            width: 0 // Animated
            visible: width > 0
            z: 10
        }

        MouseArea {
            anchors.fill: parent
            z: 100
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
                // 显示完成印章
                completedStamp.visible = true
                // 延迟一下再触发完成信号，让用户看到印章
                stampTimer.start()
            }
        }

        // 延迟触发完成信号
        Timer {
            id: stampTimer
            interval: 500
            onTriggered: root.complete()
        }
    }
}