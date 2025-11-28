/**
 * AchievementsPage.qml
 * @brief Achievements and challenges page with gamification display
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
                text: "🏆"
                font.pixelSize: 24
            }

            Text {
                text: qsTr("Achievements")
                font.pixelSize: 20
                font.bold: true
                color: "#3C3C3C"
                Layout.fillWidth: true
            }

            Rectangle {
                width: achievementCountText.implicitWidth + 16
                height: 28
                radius: 14
                color: "#FFF8E1"
                border.color: "#FFD700"
                border.width: 1

                Text {
                    id: achievementCountText
                    anchors.centerIn: parent
                    text: gamificationController.achievementsUnlocked + qsTr(" unlocked")
                    font.pixelSize: 12
                    color: "#FF8F00"
                }
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

            // Level card
            Rectangle {
                Layout.fillWidth: true
                Layout.margins: 12
                Layout.preferredHeight: levelContent.implicitHeight + 32
                radius: 20
                
                gradient: Gradient {
                    GradientStop { position: 0.0; color: "#1CB0F6" }
                    GradientStop { position: 1.0; color: "#0D8BE0" }
                }

                ColumnLayout {
                    id: levelContent
                    anchors.fill: parent
                    anchors.margins: 16
                    spacing: 12

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 16

                        // Level badge
                        Rectangle {
                            width: 70
                            height: 70
                            radius: 35
                            color: "#FFFFFF"

                            Text {
                                anchors.centerIn: parent
                                text: gamificationController.levelBadge
                                font.pixelSize: 36
                            }
                        }

                        Column {
                            spacing: 4

                            Text {
                                text: qsTr("Level ") + gamificationController.currentLevel
                                font.pixelSize: 24
                                font.bold: true
                                color: "white"
                            }

                            Text {
                                text: gamificationController.levelTitle
                                font.pixelSize: 14
                                color: "white"
                                opacity: 0.9
                            }

                            Text {
                                text: qsTr("Total XP: ") + gamificationController.totalXP
                                font.pixelSize: 12
                                color: "white"
                                opacity: 0.8
                            }
                        }
                    }

                    // XP to next level
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 4

                        Text {
                            text: qsTr("Progress to next level")
                            font.pixelSize: 12
                            color: "white"
                            opacity: 0.9
                        }

                        Rectangle {
                            Layout.fillWidth: true
                            height: 12
                            radius: 6
                            color: "#FFFFFF40"

                            Rectangle {
                                width: parent.width * gamificationController.levelProgress
                                height: parent.height
                                radius: 6
                                color: "#FFD700"

                                Behavior on width {
                                    NumberAnimation { duration: 500 }
                                }
                            }
                        }

                        Text {
                            text: gamificationController.currentXP + " / " + gamificationController.xpForNextLevel + " XP"
                            font.pixelSize: 12
                            color: "white"
                            opacity: 0.9
                        }
                    }
                }
            }

            // Daily Challenges Section
            Text {
                text: qsTr("📅 Daily Challenges")
                font.pixelSize: 18
                font.bold: true
                color: "#3C3C3C"
                Layout.leftMargin: 12
            }

            // Challenge cards
            Repeater {
                model: challengeListModel

                delegate: Rectangle {
                    Layout.fillWidth: true
                    Layout.margins: 12
                    Layout.preferredHeight: challengeContent.implicitHeight + 24
                    radius: 16
                    color: model.completed ? "#E8F5E9" : "#FFFFFF"
                    border.color: model.completed ? "#4CAF50" : "#E0E0E0"
                    border.width: model.completed ? 2 : 1

                    ColumnLayout {
                        id: challengeContent
                        anchors.fill: parent
                        anchors.margins: 12
                        spacing: 8

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 8

                            Text {
                                text: model.typeIcon
                                font.pixelSize: 20
                            }

                            Text {
                                text: model.title
                                font.pixelSize: 16
                                font.bold: true
                                color: "#3C3C3C"
                                Layout.fillWidth: true
                            }

                            // XP reward
                            Rectangle {
                                width: rewardLabel.implicitWidth + 12
                                height: 24
                                radius: 12
                                color: "#FFF8E1"
                                border.color: "#FFD700"
                                border.width: 1

                                Text {
                                    id: rewardLabel
                                    anchors.centerIn: parent
                                    text: "+" + model.rewardXP + " XP"
                                    font.pixelSize: 11
                                    font.bold: true
                                    color: "#FF8F00"
                                }
                            }
                        }

                        Text {
                            text: model.description
                            font.pixelSize: 13
                            color: "#777777"
                            Layout.fillWidth: true
                        }

                        // Progress bar
                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 8

                            Text {
                                text: model.categoryIcon
                                font.pixelSize: 14
                            }

                            Rectangle {
                                Layout.fillWidth: true
                                height: 8
                                radius: 4
                                color: "#E8E8E8"

                                Rectangle {
                                    width: parent.width * model.progress
                                    height: parent.height
                                    radius: 4
                                    color: model.completed ? "#4CAF50" : "#1CB0F6"
                                }
                            }

                            Text {
                                text: model.progressText
                                font.pixelSize: 12
                                font.bold: true
                                color: model.completed ? "#4CAF50" : "#1CB0F6"
                            }

                            // Claim button (if completed but not claimed)
                            Button {
                                visible: model.completed && !model.claimed
                                text: qsTr("Claim!")
                                font.pixelSize: 12
                                Material.background: "#FFD700"
                                Material.foreground: "#3C3C3C"
                                implicitHeight: 28
                            }

                            // Completed indicator
                            Text {
                                visible: model.claimed
                                text: "✓"
                                font.pixelSize: 18
                                font.bold: true
                                color: "#4CAF50"
                            }
                        }
                    }
                }
            }

            // Achievement badges section
            Text {
                text: qsTr("🏆 Achievement Badges")
                font.pixelSize: 18
                font.bold: true
                color: "#3C3C3C"
                Layout.leftMargin: 12
                Layout.topMargin: 16
            }

            // Sample achievement badges grid
            GridLayout {
                Layout.fillWidth: true
                Layout.margins: 12
                columns: 4
                rowSpacing: 16
                columnSpacing: 8

                Repeater {
                    model: [
                        { icon: "🌟", name: qsTr("First Steps"), unlocked: true },
                        { icon: "🔥", name: qsTr("On Fire"), unlocked: true },
                        { icon: "⚡", name: qsTr("Speed Demon"), unlocked: false },
                        { icon: "🎯", name: qsTr("Perfect Aim"), unlocked: false },
                        { icon: "💪", name: qsTr("Strong Start"), unlocked: true },
                        { icon: "🍅", name: qsTr("Pomodoro Pro"), unlocked: false },
                        { icon: "📚", name: qsTr("Scholar"), unlocked: false },
                        { icon: "🏃", name: qsTr("Marathon"), unlocked: false }
                    ]

                    delegate: Column {
                        spacing: 4

                        Rectangle {
                            width: 60
                            height: 60
                            radius: 30
                            color: modelData.unlocked ? "#FFF8E1" : "#F5F5F5"
                            border.color: modelData.unlocked ? "#FFD700" : "#E0E0E0"
                            border.width: 2
                            anchors.horizontalCenter: parent.horizontalCenter

                            Text {
                                anchors.centerIn: parent
                                text: modelData.icon
                                font.pixelSize: 28
                                opacity: modelData.unlocked ? 1.0 : 0.3
                            }

                            // Lock overlay
                            Rectangle {
                                visible: !modelData.unlocked
                                anchors.fill: parent
                                radius: parent.radius
                                color: "#80FFFFFF"

                                Text {
                                    anchors.centerIn: parent
                                    text: "🔒"
                                    font.pixelSize: 20
                                }
                            }
                        }

                        Text {
                            text: modelData.name
                            font.pixelSize: 10
                            color: modelData.unlocked ? "#3C3C3C" : "#9E9E9E"
                            anchors.horizontalCenter: parent.horizontalCenter
                            horizontalAlignment: Text.AlignHCenter
                            width: 70
                            wrapMode: Text.WordWrap
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
