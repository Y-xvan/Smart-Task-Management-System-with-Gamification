import QtQuick
import QtQuick.Controls

Item {
    // Background
    Rectangle {
        anchors.fill: parent
        color: "#121212"
    }

    // Task list
    ListView {
        id: taskList
        anchors.fill: parent
        anchors.margins: 12
        spacing: 12
        clip: true
        
        model: taskModel

        delegate: QuestCard {
            width: taskList.width
            
            // Quest card bindings
            title: model.title
            desc: model.description
            priority: model.priority
            xp: model.xpReward
            projectName: model.projectName
            dueDate: model.dueDate

            // Signal handlers
            onComplete: {
                taskModel.completeTask(index)
                gameController.refresh()
            }
            onDelete: {
                taskModel.deleteTask(index)
                gameController.refresh()
            }
        }

        // Footer spacing
        footer: Item { height: 80 }
    }

    // New task button (FAB)
    Button {
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 20
        width: 60; height: 60
        
        background: Rectangle {
            radius: 30
            color: "#FFD700"
            layer.enabled: true
        }
        
        contentItem: Text {
            text: "+"
            font.pixelSize: 32
            color: "#000"
            anchors.centerIn: parent
        }

        onClicked: newTaskDialog.open()
    }

    NewTaskDialog { id: newTaskDialog }
}
