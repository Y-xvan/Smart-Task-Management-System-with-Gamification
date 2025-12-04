import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window

/**
 * Main.qml - Main application window for RPG Quest Board
 * 
 * Design Style: RPG Quest Board
 * Colors: Parchment Beige (#F5E6D3), Ink Black (#2C3E50), 
 *         Urgent Red (#E74C3C), Daily Blue (#3498DB), Gold (#F1C40F)
 */
ApplicationWindow {
    id: mainWindow
    visible: true
    width: 800
    height: 600
    minimumWidth: 600
    minimumHeight: 400
    title: qsTr("Quest Board - Smart Task Manager")
    
    // RPG Theme Colors
    readonly property color parchmentBeige: "#F5E6D3"
    readonly property color inkBlack: "#2C3E50"
    readonly property color urgentRed: "#E74C3C"
    readonly property color dailyBlue: "#3498DB"
    readonly property color goldColor: "#F1C40F"
    readonly property color darkParchment: "#D4C4A8"
    readonly property color completedGreen: "#27AE60"
    
    color: parchmentBeige
    
    // Header with user stats
    header: Rectangle {
        height: 80
        color: inkBlack
        
        RowLayout {
            anchors.fill: parent
            anchors.margins: 15
            spacing: 20
            
            // Title
            Text {
                text: "⚔️ Quest Board"
                font.pixelSize: 24
                font.bold: true
                color: goldColor
                Layout.fillWidth: true
            }
            
            // User stats panel
            Rectangle {
                Layout.preferredWidth: 200
                Layout.preferredHeight: 50
                color: Qt.rgba(255, 255, 255, 0.1)
                radius: 8
                
                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 8
                    spacing: 10
                    
                    // Level badge
                    Rectangle {
                        Layout.preferredWidth: 40
                        Layout.preferredHeight: 40
                        radius: 20
                        color: goldColor
                        
                        Text {
                            anchors.centerIn: parent
                            text: taskModel.level
                            font.pixelSize: 18
                            font.bold: true
                            color: inkBlack
                        }
                    }
                    
                    // XP info
                    Column {
                        Layout.fillWidth: true
                        spacing: 2
                        
                        Text {
                            text: taskModel.levelTitle
                            font.pixelSize: 12
                            font.bold: true
                            color: "white"
                        }
                        
                        // XP progress bar
                        Rectangle {
                            width: 100
                            height: 8
                            color: Qt.rgba(255, 255, 255, 0.2)
                            radius: 4
                            
                            Rectangle {
                                width: parent.width * taskModel.levelProgress
                                height: parent.height
                                color: goldColor
                                radius: 4
                            }
                        }
                        
                        Text {
                            text: taskModel.totalXP + " XP"
                            font.pixelSize: 10
                            color: Qt.rgba(255, 255, 255, 0.7)
                        }
                    }
                }
            }
        }
    }
    
    // Main content area
    QuestBoard {
        anchors.fill: parent
        anchors.margins: 15
    }
    
    // Completion notification popup
    Popup {
        id: completionPopup
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        width: 300
        height: 150
        modal: true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
        
        property int xpAwarded: 0
        property string questTitle: ""
        
        background: Rectangle {
            color: inkBlack
            radius: 10
            border.color: goldColor
            border.width: 3
        }
        
        contentItem: Column {
            anchors.centerIn: parent
            spacing: 15
            
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "🎉 MISSION COMPLETED! 🎉"
                font.pixelSize: 18
                font.bold: true
                color: goldColor
            }
            
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: completionPopup.questTitle
                font.pixelSize: 14
                color: "white"
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
            }
            
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "+" + completionPopup.xpAwarded + " XP"
                font.pixelSize: 20
                font.bold: true
                color: completedGreen
            }
        }
        
        // Auto-close timer
        Timer {
            id: popupTimer
            interval: 2000
            onTriggered: completionPopup.close()
        }
        
        onOpened: popupTimer.start()
    }
    
    // Connect to taskModel signals
    Connections {
        target: taskModel
        
        function onTaskCompleted(xpAwarded, taskTitle) {
            completionPopup.xpAwarded = xpAwarded
            completionPopup.questTitle = taskTitle
            completionPopup.open()
        }
    }
}
