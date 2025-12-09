import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Window {
    id: window
    width: 450
    height: 850
    visible: true
    title: "SmartTask RPG"
    color: "#121212" // 深色背景

    // 全局信号槽：处理 XP 获取弹窗
    Connections {
        target: gameController
        function onXpGained(amount, reason) {
            xpPopupText.text = "+" + amount + " XP\n" + reason
            xpPopup.open()
        }
    }

    // === 1. 顶部 HUD (Head-Up Display) ===
    Rectangle {
        id: hudBar
        width: parent.width
        height: 90
        color: "#1e1e1e"
        z: 100

        RowLayout {
            anchors.fill: parent
            anchors.margins: 16
            spacing: 12

            // 头像
            Rectangle {
                width: 54; height: 54; radius: 27
                color: "#FFD700"
                border.color: "#fff"; border.width: 2
                Text { 
                    text: "🧙‍♂️"
                    font.pixelSize: 32
                    anchors.centerIn: parent 
                }
            }

            // 状态栏 (等级 & XP)
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 4

                Text { 
                    text: "Lv." + gameController.currentLevel + " " + gameController.currentTitle
                    color: "#ffffff"; font.bold: true; font.pixelSize: 16 
                }

                // XP 条
                Rectangle {
                    Layout.fillWidth: true; height: 8; radius: 4
                    color: "#333"
                    Rectangle {
                        width: parent.width * gameController.levelProgress
                        height: parent.height; radius: 4
                        color: "#00E676"
                        Behavior on width { NumberAnimation { duration: 500; easing.type: Easing.OutQuad } }
                    }
                }

                Text {
                    text: gameController.currentXP + " / " + gameController.nextLevelXP + " XP"
                    color: "#888"; font.pixelSize: 11
                }
            }

            // 连胜火焰
            Column {
                spacing: 0
                Text { text: "🔥"; font.pixelSize: 24; anchors.horizontalCenter: parent.horizontalCenter }
                Text { 
                    text: gameController.streakDays
                    color: "#FF5722"; font.bold: true; font.pixelSize: 14 
                    anchors.horizontalCenter: parent.horizontalCenter
                }
            }
        }
    }

    // === 2. 内容区域 ===
    StackLayout {
        id: contentStack
        anchors.top: hudBar.bottom
        anchors.bottom: navBar.top
        anchors.left: parent.left
        anchors.right: parent.right
        currentIndex: navBar.currentIndex

        // 按顺序对应 TabBar
        QuestView { }     // Index 0
        ProjectView { }   // Index 1
        FocusView { }     // Index 2
        StatsView { }     // Index 3
        ReminderView { }  // Index 4
    }

    // === 3. 底部导航栏 ===
    TabBar {
        id: navBar
        anchors.bottom: parent.bottom
        width: parent.width
        height: 65
        background: Rectangle { color: "#1e1e1e" }

        // 自定义 TabButton 样式
        component NavBtn: TabButton {
            property string iconText
            contentItem: Column {
                spacing: 2
                Text { text: iconText; font.pixelSize: 20; anchors.horizontalCenter: parent.horizontalCenter; color: parent.checked ? "#FFD700" : "#888" }
                Text { text: parent.text; font.pixelSize: 10; anchors.horizontalCenter: parent.horizontalCenter; color: parent.checked ? "#FFD700" : "#888" }
            }
            background: Rectangle { color: "transparent" }
        }

        NavBtn { text: "Quests"; iconText: "⚔️" }
        NavBtn { text: "Projects"; iconText: "📁" }
        NavBtn { text: "Focus"; iconText: "🍅" }
        NavBtn { text: "Stats"; iconText: "📊" }
        NavBtn { text: "Alerts"; iconText: "🔔" }
    }

    // === 4. XP 获取弹窗 ===
    Popup {
        id: xpPopup
        anchors.centerIn: parent
        width: 200; height: 100
        modal: false
        focus: false
        closePolicy: Popup.NoAutoClose

        background: Rectangle {
            color: "#cc000000"
            radius: 10
            border.color: "#FFD700"; border.width: 2
        }
        contentItem: Column {
            spacing: 5
            Text { text: "⭐ XP GAINED!"; color: "#FFD700"; font.bold: true; anchors.horizontalCenter: parent.horizontalCenter }
            Text { id: xpPopupText; color: "#fff"; horizontalAlignment: Text.AlignHCenter; anchors.horizontalCenter: parent.horizontalCenter }
        }
        
        // 自动关闭
        Timer {
            interval: 2000; running: xpPopup.visible; repeat: false
            onTriggered: xpPopup.close()
        }
    }
}