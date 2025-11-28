/**
 * TaskListPage.qml
 * @brief Main task list page with Quest Tracker design
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import "../components"

Page {
    id: root

    background: Rectangle {
        color: "#F7F7F7"
    }

    // Header
    header: Rectangle {
        height: 60
        color: "#FFFFFF"

        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 16
            anchors.rightMargin: 16
            spacing: 12

            Text {
                text: "📜"
                font.pixelSize: 24
            }

            Text {
                text: qsTr("Active Quests")
                font.pixelSize: 20
                font.bold: true
                color: "#3C3C3C"
                Layout.fillWidth: true
            }

            // Task count badge
            Rectangle {
                width: countText.implicitWidth + 16
                height: 28
                radius: 14
                color: "#E3F2FD"
                border.color: "#1CB0F6"
                border.width: 1

                Text {
                    id: countText
                    anchors.centerIn: parent
                    text: taskListModel.pendingCount + qsTr(" pending")
                    font.pixelSize: 12
                    color: "#1565C0"
                }
            }

            // Filter button
            RoundButton {
                text: "⚙️"
                font.pixelSize: 16
                Material.background: "#F5F5F5"
                implicitWidth: 40
                implicitHeight: 40

                onClicked: filterMenu.open()

                Menu {
                    id: filterMenu
                    
                    MenuItem {
                        text: qsTr("Show All")
                        onTriggered: taskListModel.setFilter(true, true)
                    }
                    MenuItem {
                        text: qsTr("Pending Only")
                        onTriggered: taskListModel.setFilter(false, true)
                    }
                    MenuItem {
                        text: qsTr("Completed Only")
                        onTriggered: taskListModel.setFilter(true, false)
                    }
                }
            }
        }

        // Bottom border
        Rectangle {
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            height: 1
            color: "#E0E0E0"
        }
    }

    // Main content
    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // Empty state
        Item {
            visible: taskListModel.count === 0
            Layout.fillWidth: true
            Layout.fillHeight: true

            Column {
                anchors.centerIn: parent
                spacing: 16

                // [需素材] Empty state illustration
                // source: "qrc:/images/empty_quest.png"
                // 需要一个 200x200 的空状态插画，显示冒险者在休息或者空的任务卷轴
                Rectangle {
                    width: 150
                    height: 150
                    radius: 75
                    color: "#E8F5E9"
                    anchors.horizontalCenter: parent.horizontalCenter

                    Text {
                        anchors.centerIn: parent
                        text: "📜"
                        font.pixelSize: 64
                        opacity: 0.5
                    }
                }

                Text {
                    text: qsTr("No quests yet!")
                    font.pixelSize: 20
                    font.bold: true
                    color: "#3C3C3C"
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                Text {
                    text: qsTr("Create your first quest to begin the adventure")
                    font.pixelSize: 14
                    color: "#777777"
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                Button {
                    text: qsTr("➕ Create Quest")
                    Material.background: "#58CC02"
                    Material.foreground: "white"
                    font.bold: true
                    anchors.horizontalCenter: parent.horizontalCenter

                    onClicked: createTaskDialog.open()
                }
            }
        }

        // Task list
        ListView {
            id: taskListView
            visible: taskListModel.count > 0
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.margins: 12
            spacing: 12
            clip: true

            model: taskListModel

            // Pull to refresh
            onAtYBeginningChanged: {
                if (atYBeginning && dragging) {
                    taskListModel.refresh()
                }
            }

            delegate: TaskCard {
                width: taskListView.width

                taskId: model.taskId
                taskTitle: model.title
                taskDescription: model.description || ""
                taskPriority: model.priority
                priorityColor: model.priorityColor
                priorityIcon: model.priorityIcon
                taskCompleted: model.completed
                dueDate: model.dueDate || ""
                pomodoroCount: model.pomodoroCount
                estimatedPomodoros: model.estimatedPomodoros
                pomodoroProgress: model.pomodoroProgress

                onCompleteClicked: function(id) {
                    taskController.completeTask(id)
                }

                onEditClicked: function(id) {
                    editTaskDialog.taskId = id
                    var task = taskListModel.getTask(id)
                    editTaskDialog.taskTitle = task.title || ""
                    editTaskDialog.taskDescription = task.description || ""
                    editTaskDialog.taskPriority = task.priority || 1
                    editTaskDialog.open()
                }

                onDeleteClicked: function(id) {
                    deleteConfirmDialog.taskId = id
                    deleteConfirmDialog.open()
                }

                onPomodoroClicked: function(id) {
                    taskController.addPomodoro(id)
                }
            }

            // Add animation
            add: Transition {
                NumberAnimation { property: "opacity"; from: 0; to: 1; duration: 300 }
                NumberAnimation { property: "y"; from: -20; duration: 300; easing.type: Easing.OutQuad }
            }

            // Remove animation
            remove: Transition {
                NumberAnimation { property: "opacity"; to: 0; duration: 200 }
                NumberAnimation { property: "x"; to: width; duration: 200; easing.type: Easing.InQuad }
            }

            // Displaced animation
            displaced: Transition {
                NumberAnimation { property: "y"; duration: 300; easing.type: Easing.OutQuad }
            }
        }
    }

    // Floating Action Button
    RoundButton {
        id: fab
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 20
        width: 60
        height: 60
        text: "➕"
        font.pixelSize: 24
        Material.background: "#58CC02"
        Material.foreground: "white"

        // Shadow
        layer.enabled: true
        layer.effect: Item {
            Rectangle {
                anchors.fill: parent
                anchors.topMargin: 4
                color: "#40000000"
                radius: 30
            }
        }

        // Hover animation
        scale: pressed ? 0.9 : (hovered ? 1.1 : 1.0)
        Behavior on scale {
            NumberAnimation { duration: 100; easing.type: Easing.OutQuad }
        }

        onClicked: createTaskDialog.open()
    }

    // Create Task Dialog
    Dialog {
        id: createTaskDialog
        title: qsTr("📝 New Quest")
        modal: true
        anchors.centerIn: parent
        width: parent.width * 0.9
        standardButtons: Dialog.Ok | Dialog.Cancel

        ColumnLayout {
            anchors.fill: parent
            spacing: 16

            TextField {
                id: newTaskTitle
                Layout.fillWidth: true
                placeholderText: qsTr("Quest title...")
                font.pixelSize: 16
            }

            TextField {
                id: newTaskDesc
                Layout.fillWidth: true
                placeholderText: qsTr("Description (optional)")
                font.pixelSize: 14
            }

            // Priority selector
            RowLayout {
                Layout.fillWidth: true
                spacing: 8

                Text {
                    text: qsTr("Priority:")
                    font.pixelSize: 14
                    color: "#777777"
                }

                Repeater {
                    model: [
                        { label: "🟢 Low", value: 0, color: "#4CAF50" },
                        { label: "🟡 Medium", value: 1, color: "#FF9800" },
                        { label: "🔴 High", value: 2, color: "#F44336" }
                    ]

                    delegate: Rectangle {
                        width: priorityLabel.implicitWidth + 16
                        height: 32
                        radius: 16
                        color: newTaskPriority === modelData.value ? modelData.color : "#F5F5F5"
                        border.color: modelData.color
                        border.width: 1

                        Text {
                            id: priorityLabel
                            anchors.centerIn: parent
                            text: modelData.label
                            font.pixelSize: 12
                            color: newTaskPriority === modelData.value ? "white" : "#3C3C3C"
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: newTaskPriority = modelData.value
                        }
                    }
                }
            }
        }

        property int newTaskPriority: 1

        onAccepted: {
            if (newTaskTitle.text.trim() !== "") {
                taskController.createTask(
                    newTaskTitle.text,
                    newTaskDesc.text,
                    newTaskPriority
                )
                newTaskTitle.text = ""
                newTaskDesc.text = ""
                newTaskPriority = 1
            }
        }

        onRejected: {
            newTaskTitle.text = ""
            newTaskDesc.text = ""
            newTaskPriority = 1
        }
    }

    // Edit Task Dialog
    Dialog {
        id: editTaskDialog
        title: qsTr("✏️ Edit Quest")
        modal: true
        anchors.centerIn: parent
        width: parent.width * 0.9
        standardButtons: Dialog.Save | Dialog.Cancel

        property int taskId: 0
        property string taskTitle: ""
        property string taskDescription: ""
        property int taskPriority: 1

        ColumnLayout {
            anchors.fill: parent
            spacing: 16

            TextField {
                id: editTitleField
                Layout.fillWidth: true
                text: editTaskDialog.taskTitle
                font.pixelSize: 16
            }

            TextField {
                id: editDescField
                Layout.fillWidth: true
                text: editTaskDialog.taskDescription
                font.pixelSize: 14
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: 8

                Text {
                    text: qsTr("Priority:")
                    font.pixelSize: 14
                    color: "#777777"
                }

                Repeater {
                    model: [
                        { label: "🟢 Low", value: 0, color: "#4CAF50" },
                        { label: "🟡 Medium", value: 1, color: "#FF9800" },
                        { label: "🔴 High", value: 2, color: "#F44336" }
                    ]

                    delegate: Rectangle {
                        width: editPriorityLabel.implicitWidth + 16
                        height: 32
                        radius: 16
                        color: editTaskDialog.taskPriority === modelData.value ? modelData.color : "#F5F5F5"
                        border.color: modelData.color
                        border.width: 1

                        Text {
                            id: editPriorityLabel
                            anchors.centerIn: parent
                            text: modelData.label
                            font.pixelSize: 12
                            color: editTaskDialog.taskPriority === modelData.value ? "white" : "#3C3C3C"
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: editTaskDialog.taskPriority = modelData.value
                        }
                    }
                }
            }
        }

        onAccepted: {
            taskController.updateTask(
                taskId,
                editTitleField.text,
                editDescField.text,
                taskPriority
            )
        }
    }

    // Delete Confirmation Dialog
    Dialog {
        id: deleteConfirmDialog
        title: qsTr("🗑️ Delete Quest")
        modal: true
        anchors.centerIn: parent
        standardButtons: Dialog.Yes | Dialog.No

        property int taskId: 0

        Text {
            text: qsTr("Are you sure you want to abandon this quest?")
            font.pixelSize: 14
            color: "#3C3C3C"
        }

        onAccepted: {
            taskController.deleteTask(taskId)
        }
    }
}
