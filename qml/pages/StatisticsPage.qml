/**
 * StatisticsPage.qml
 * @brief Statistics and analytics page with data visualization
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts

Page {
    id: root

    background: Rectangle {
        color: "#F7F7F7"
    }

    header: Rectangle {
        height: 60
        color: "#FFFFFF"

        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 16
            anchors.rightMargin: 16
            spacing: 12

            Text {
                text: "📊"
                font.pixelSize: 24
            }

            Text {
                text: qsTr("Statistics")
                font.pixelSize: 20
                font.bold: true
                color: "#3C3C3C"
                Layout.fillWidth: true
            }
        }

        Rectangle {
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            height: 1
            color: "#E0E0E0"
        }
    }

    ScrollView {
        anchors.fill: parent
        contentWidth: availableWidth

        ColumnLayout {
            width: parent.width
            spacing: 16

            // Summary cards row
            RowLayout {
                Layout.fillWidth: true
                Layout.margins: 12
                spacing: 12

                // Tasks completed today
                Rectangle {
                    Layout.fillWidth: true
                    height: 100
                    radius: 16
                    color: "#E8F5E9"

                    Column {
                        anchors.centerIn: parent
                        spacing: 4

                        Text {
                            text: gamificationController.tasksCompletedToday
                            font.pixelSize: 32
                            font.bold: true
                            color: "#2E7D32"
                            anchors.horizontalCenter: parent.horizontalCenter
                        }

                        Text {
                            text: qsTr("Today")
                            font.pixelSize: 12
                            color: "#4CAF50"
                            anchors.horizontalCenter: parent.horizontalCenter
                        }

                        Text {
                            text: "✅"
                            font.pixelSize: 16
                            anchors.horizontalCenter: parent.horizontalCenter
                        }
                    }
                }

                // Tasks completed this week
                Rectangle {
                    Layout.fillWidth: true
                    height: 100
                    radius: 16
                    color: "#E3F2FD"

                    Column {
                        anchors.centerIn: parent
                        spacing: 4

                        Text {
                            text: gamificationController.tasksCompletedThisWeek
                            font.pixelSize: 32
                            font.bold: true
                            color: "#1565C0"
                            anchors.horizontalCenter: parent.horizontalCenter
                        }

                        Text {
                            text: qsTr("This Week")
                            font.pixelSize: 12
                            color: "#1CB0F6"
                            anchors.horizontalCenter: parent.horizontalCenter
                        }

                        Text {
                            text: "📅"
                            font.pixelSize: 16
                            anchors.horizontalCenter: parent.horizontalCenter
                        }
                    }
                }

                // Completion rate
                Rectangle {
                    Layout.fillWidth: true
                    height: 100
                    radius: 16
                    color: "#FFF8E1"

                    Column {
                        anchors.centerIn: parent
                        spacing: 4

                        Text {
                            text: Math.round(gamificationController.completionRate * 100) + "%"
                            font.pixelSize: 32
                            font.bold: true
                            color: "#FF8F00"
                            anchors.horizontalCenter: parent.horizontalCenter
                        }

                        Text {
                            text: qsTr("Rate")
                            font.pixelSize: 12
                            color: "#FF9800"
                            anchors.horizontalCenter: parent.horizontalCenter
                        }

                        Text {
                            text: "📈"
                            font.pixelSize: 16
                            anchors.horizontalCenter: parent.horizontalCenter
                        }
                    }
                }
            }

            // Streak section
            Rectangle {
                Layout.fillWidth: true
                Layout.margins: 12
                height: streakContent.implicitHeight + 32
                radius: 16
                color: "#FFFFFF"

                ColumnLayout {
                    id: streakContent
                    anchors.fill: parent
                    anchors.margins: 16
                    spacing: 12

                    Text {
                        text: qsTr("🔥 Streak Record")
                        font.pixelSize: 16
                        font.bold: true
                        color: "#3C3C3C"
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 20

                        // Current streak
                        Column {
                            spacing: 4

                            Row {
                                spacing: 4
                                anchors.horizontalCenter: parent.horizontalCenter

                                Text {
                                    text: "🔥"
                                    font.pixelSize: 20

                                    SequentialAnimation on rotation {
                                        running: gamificationController.currentStreak > 0
                                        loops: Animation.Infinite
                                        NumberAnimation { from: -3; to: 3; duration: 200 }
                                        NumberAnimation { from: 3; to: -3; duration: 200 }
                                    }
                                }

                                Text {
                                    text: gamificationController.currentStreak
                                    font.pixelSize: 36
                                    font.bold: true
                                    color: "#FF5722"
                                }
                            }

                            Text {
                                text: qsTr("Current Streak")
                                font.pixelSize: 12
                                color: "#777777"
                                anchors.horizontalCenter: parent.horizontalCenter
                            }
                        }

                        // Divider
                        Rectangle {
                            width: 1
                            Layout.fillHeight: true
                            color: "#E0E0E0"
                        }

                        // Longest streak
                        Column {
                            spacing: 4

                            Row {
                                spacing: 4
                                anchors.horizontalCenter: parent.horizontalCenter

                                Text {
                                    text: "🏆"
                                    font.pixelSize: 20
                                }

                                Text {
                                    text: gamificationController.longestStreak
                                    font.pixelSize: 36
                                    font.bold: true
                                    color: "#FFD700"
                                }
                            }

                            Text {
                                text: qsTr("Longest Streak")
                                font.pixelSize: 12
                                color: "#777777"
                                anchors.horizontalCenter: parent.horizontalCenter
                            }
                        }
                    }

                    // Streak message
                    Rectangle {
                        Layout.fillWidth: true
                        height: 36
                        radius: 18
                        color: gamificationController.currentStreak > 0 ? "#FFF3E0" : "#F5F5F5"

                        Text {
                            anchors.centerIn: parent
                            text: gamificationController.currentStreak > 0 
                                ? qsTr("Keep it up! Don't break your streak! 🔥")
                                : qsTr("Complete a task today to start a streak!")
                            font.pixelSize: 12
                            color: gamificationController.currentStreak > 0 ? "#E65100" : "#777777"
                        }
                    }
                }
            }

            // Heatmap section
            Rectangle {
                Layout.fillWidth: true
                Layout.margins: 12
                height: heatmapContent.implicitHeight + 32
                radius: 16
                color: "#FFFFFF"

                ColumnLayout {
                    id: heatmapContent
                    anchors.fill: parent
                    anchors.margins: 16
                    spacing: 12

                    Text {
                        text: qsTr("📆 Activity Heatmap")
                        font.pixelSize: 16
                        font.bold: true
                        color: "#3C3C3C"
                    }

                    // Simplified heatmap visualization
                    // (In production, this would be generated from actual data)
                    Column {
                        Layout.fillWidth: true
                        spacing: 2

                        Text {
                            text: qsTr("Last 4 weeks")
                            font.pixelSize: 12
                            color: "#777777"
                        }

                        // Week rows
                        Repeater {
                            model: 4

                            Row {
                                spacing: 2

                                Repeater {
                                    model: 7

                                    Rectangle {
                                        width: 30
                                        height: 30
                                        radius: 4
                                        
                                        // Random intensity for demo
                                        property int intensity: Math.floor(Math.random() * 5)
                                        
                                        color: {
                                            switch (intensity) {
                                                case 0: return "#EBEDF0"
                                                case 1: return "#C6E48B"
                                                case 2: return "#7BC96F"
                                                case 3: return "#239A3B"
                                                case 4: return "#196127"
                                                default: return "#EBEDF0"
                                            }
                                        }

                                        // Tooltip on hover
                                        MouseArea {
                                            anchors.fill: parent
                                            hoverEnabled: true

                                            ToolTip.visible: containsMouse
                                            ToolTip.text: parent.intensity + qsTr(" tasks")
                                        }
                                    }
                                }
                            }
                        }

                        // Legend
                        Row {
                            spacing: 4
                            Layout.topMargin: 8

                            Text {
                                text: qsTr("Less")
                                font.pixelSize: 10
                                color: "#777777"
                            }

                            Repeater {
                                model: ["#EBEDF0", "#C6E48B", "#7BC96F", "#239A3B", "#196127"]

                                Rectangle {
                                    width: 12
                                    height: 12
                                    radius: 2
                                    color: modelData
                                }
                            }

                            Text {
                                text: qsTr("More")
                                font.pixelSize: 10
                                color: "#777777"
                            }
                        }
                    }
                }
            }

            // Bottom spacer
            Item {
                Layout.preferredHeight: 20
            }
        }
    }
}
