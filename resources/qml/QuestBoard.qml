import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

/**
 * QuestBoard.qml - The list view displaying tasks as quest cards
 * 
 * Design: Styled as a medieval quest board with "Wanted Poster" style cards
 */
Item {
    id: questBoard
    
    // RPG Theme Colors
    readonly property color parchmentBeige: "#F5E6D3"
    readonly property color inkBlack: "#2C3E50"
    readonly property color urgentRed: "#E74C3C"
    readonly property color dailyBlue: "#3498DB"
    readonly property color goldColor: "#F1C40F"
    readonly property color darkParchment: "#D4C4A8"
    
    // Board header
    Rectangle {
        id: boardHeader
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 50
        color: "transparent"
        
        RowLayout {
            anchors.fill: parent
            spacing: 15
            
            Text {
                text: "📜 Available Quests"
                font.pixelSize: 20
                font.bold: true
                color: inkBlack
                Layout.fillWidth: true
            }
            
            // Quest count
            Rectangle {
                Layout.preferredWidth: 100
                Layout.preferredHeight: 30
                color: inkBlack
                radius: 15
                
                Text {
                    anchors.centerIn: parent
                    text: taskModel.rowCount() + " Quests"
                    font.pixelSize: 12
                    font.bold: true
                    color: "white"
                }
            }
            
            // Refresh button
            Button {
                Layout.preferredWidth: 40
                Layout.preferredHeight: 40
                
                contentItem: Text {
                    text: "🔄"
                    font.pixelSize: 18
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                
                background: Rectangle {
                    color: parent.pressed ? darkParchment : "transparent"
                    border.color: inkBlack
                    border.width: 2
                    radius: 8
                }
                
                onClicked: taskModel.refreshTasks()
            }
        }
    }
    
    // Quest list
    ListView {
        id: questListView
        anchors.top: boardHeader.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.topMargin: 15
        
        model: taskModel
        spacing: 15
        clip: true
        
        // Empty state
        Label {
            anchors.centerIn: parent
            visible: questListView.count === 0
            text: "No quests available.\nTime to add some adventures!"
            font.pixelSize: 16
            color: inkBlack
            opacity: 0.6
            horizontalAlignment: Text.AlignHCenter
        }
        
        delegate: QuestCard {
            width: questListView.width
            
            questTitle: model.title
            questDescription: model.description
            questPriority: model.priority
            questDueDate: model.dueDate
            questXPReward: model.xpReward
            questCompleted: model.isCompleted
            
            onQuestAccepted: {
                taskModel.markTaskCompleted(index)
            }
        }
        
        // Scroll behavior
        ScrollBar.vertical: ScrollBar {
            active: true
            policy: ScrollBar.AsNeeded
            
            contentItem: Rectangle {
                implicitWidth: 8
                radius: 4
                color: inkBlack
                opacity: 0.5
            }
        }
    }
}
