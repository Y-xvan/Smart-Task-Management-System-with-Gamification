/**
 * TaskCard.qml
 * @brief Task card component with Duolingo-style design and animations
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts

Rectangle {
    id: root
    
    // Properties from model
    property int taskId: 0
    property string taskTitle: ""
    property string taskDescription: ""
    property int taskPriority: 1
    property string priorityColor: "#FF9800"
    property string priorityIcon: "🟡"
    property bool taskCompleted: false
    property string dueDate: ""
    property int pomodoroCount: 0
    property int estimatedPomodoros: 0
    property real pomodoroProgress: 0

    // Signals
    signal completeClicked(int id)
    signal editClicked(int id)
    signal deleteClicked(int id)
    signal pomodoroClicked(int id)

    width: parent ? parent.width : 300
    height: contentColumn.implicitHeight + 24
    radius: 16
    color: taskCompleted ? "#F5F5F5" : "#FFFFFF"
    border.color: taskCompleted ? "#E0E0E0" : priorityColor
    border.width: 2

    // Shadow effect
    layer.enabled: true
    layer.effect: Item {
        Rectangle {
            anchors.fill: parent
            anchors.topMargin: 4
            anchors.leftMargin: 2
            color: "#1A000000"
            radius: 16
        }
    }

    // Press animation
    scale: mouseArea.pressed ? 0.98 : 1.0
    Behavior on scale {
        NumberAnimation {
            duration: 100
            easing.type: Easing.OutQuad
        }
    }

    // Completion animation
    opacity: completionAnimation.running ? 0 : 1
    
    SequentialAnimation {
        id: completionAnimation
        
        ParallelAnimation {
            NumberAnimation {
                target: root
                property: "scale"
                to: 1.05
                duration: 200
                easing.type: Easing.OutQuad
            }
            ColorAnimation {
                target: root
                property: "color"
                to: "#E8F5E9"
                duration: 200
            }
        }
        
        ParallelAnimation {
            NumberAnimation {
                target: root
                property: "opacity"
                to: 0
                duration: 300
            }
            NumberAnimation {
                target: root
                property: "scale"
                to: 0.8
                duration: 300
                easing.type: Easing.InBack
            }
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onClicked: {
            editClicked(taskId)
        }
    }

    ColumnLayout {
        id: contentColumn
        anchors.fill: parent
        anchors.margins: 12
        spacing: 8

        // Top row: Priority icon, Title, Due date
        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            // Priority indicator circle
            Rectangle {
                width: 12
                height: 12
                radius: 6
                color: priorityColor
                
                // Pulse animation for high priority
                SequentialAnimation on scale {
                    running: taskPriority === 2 && !taskCompleted
                    loops: Animation.Infinite
                    NumberAnimation { to: 1.2; duration: 500; easing.type: Easing.InOutSine }
                    NumberAnimation { to: 1.0; duration: 500; easing.type: Easing.InOutSine }
                }
            }

            // Task title
            Text {
                text: taskTitle
                font.pixelSize: 16
                font.bold: true
                font.strikeout: taskCompleted
                color: taskCompleted ? "#9E9E9E" : "#3C3C3C"
                Layout.fillWidth: true
                elide: Text.ElideRight
            }

            // Due date chip
            Rectangle {
                visible: dueDate !== ""
                width: dueDateText.implicitWidth + 16
                height: 24
                radius: 12
                color: isOverdue ? "#FFEBEE" : "#E3F2FD"
                border.color: isOverdue ? "#EF5350" : "#64B5F6"
                border.width: 1

                property bool isOverdue: {
                    if (dueDate === "") return false
                    var today = new Date()
                    var due = new Date(dueDate)
                    return due < today && !taskCompleted
                }

                Text {
                    id: dueDateText
                    anchors.centerIn: parent
                    text: "📅 " + dueDate
                    font.pixelSize: 10
                    color: parent.isOverdue ? "#C62828" : "#1565C0"
                }
            }
        }

        // Description (if any)
        Text {
            visible: taskDescription !== ""
            text: taskDescription
            font.pixelSize: 13
            color: "#777777"
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            maximumLineCount: 2
            elide: Text.ElideRight
        }

        // Pomodoro progress (if estimated)
        RowLayout {
            visible: estimatedPomodoros > 0
            Layout.fillWidth: true
            spacing: 8

            Text {
                text: "🍅"
                font.pixelSize: 14
            }

            // Pomodoro progress bar
            Rectangle {
                Layout.fillWidth: true
                height: 8
                radius: 4
                color: "#FFE0B2"

                Rectangle {
                    width: parent.width * Math.min(pomodoroProgress, 1.0)
                    height: parent.height
                    radius: 4
                    color: "#FF5722"
                    
                    Behavior on width {
                        NumberAnimation {
                            duration: 300
                            easing.type: Easing.OutQuad
                        }
                    }
                }
            }

            Text {
                text: pomodoroCount + "/" + estimatedPomodoros
                font.pixelSize: 12
                font.bold: true
                color: "#FF5722"
            }
        }

        // Action buttons row
        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            // Complete button
            RoundButton {
                visible: !taskCompleted
                icon.source: ""
                text: "✓"
                font.pixelSize: 16
                Material.background: "#58CC02"
                Material.foreground: "white"
                implicitWidth: 40
                implicitHeight: 40

                onClicked: {
                    completionAnimation.start()
                    Qt.callLater(function() {
                        completeClicked(taskId)
                    })
                }

                // Hover/press effect
                scale: pressed ? 0.9 : (hovered ? 1.1 : 1.0)
                Behavior on scale {
                    NumberAnimation { duration: 100 }
                }
            }

            // Already completed indicator
            Rectangle {
                visible: taskCompleted
                width: 40
                height: 40
                radius: 20
                color: "#E8F5E9"
                border.color: "#4CAF50"
                border.width: 2

                Text {
                    anchors.centerIn: parent
                    text: "✓"
                    font.pixelSize: 20
                    color: "#4CAF50"
                }
            }

            // Pomodoro button
            RoundButton {
                visible: !taskCompleted
                icon.source: ""
                text: "🍅"
                font.pixelSize: 14
                Material.background: "#FFF3E0"
                implicitWidth: 40
                implicitHeight: 40

                onClicked: pomodoroClicked(taskId)

                scale: pressed ? 0.9 : 1.0
                Behavior on scale {
                    NumberAnimation { duration: 100 }
                }
            }

            Item { Layout.fillWidth: true }

            // XP reward display
            Rectangle {
                visible: !taskCompleted
                width: xpText.implicitWidth + 12
                height: 28
                radius: 14
                color: "#FFF8E1"
                border.color: "#FFD700"
                border.width: 1

                Text {
                    id: xpText
                    anchors.centerIn: parent
                    text: "+" + gamificationController.getXPForTask(taskPriority) + " XP"
                    font.pixelSize: 11
                    font.bold: true
                    color: "#FF8F00"
                }
            }

            // Delete button
            RoundButton {
                icon.source: ""
                text: "🗑️"
                font.pixelSize: 14
                Material.background: "#FFEBEE"
                implicitWidth: 36
                implicitHeight: 36
                opacity: 0.7

                onClicked: deleteClicked(taskId)

                scale: pressed ? 0.9 : 1.0
                Behavior on scale {
                    NumberAnimation { duration: 100 }
                }
            }
        }
    }
}
