/**
 * GamificationHUD.qml
 * @brief Top bar showing level, XP progress, streak - Duolingo style
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts

Rectangle {
    id: root
    color: "#FFFFFF"

    // Shadow effect
    layer.enabled: true
    layer.effect: Item {
        Rectangle {
            anchors.fill: parent
            anchors.topMargin: 2
            color: "#1A000000"
            radius: 0
        }
    }

    // Animation for XP gain
    function animateXPGain(xpGained) {
        xpGainLabel.text = "+" + xpGained + " XP"
        xpGainAnimation.start()
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 8

        // Top row: Level badge and streak
        RowLayout {
            Layout.fillWidth: true
            spacing: 12

            // Level badge with icon
            Rectangle {
                width: 50
                height: 50
                radius: 25
                color: "#1CB0F6"

                Text {
                    anchors.centerIn: parent
                    text: gamificationController.levelBadge
                    font.pixelSize: 24
                }

                // Pulse animation on level up
                SequentialAnimation on scale {
                    id: levelPulseAnim
                    running: false
                    NumberAnimation { to: 1.2; duration: 150; easing.type: Easing.OutQuad }
                    NumberAnimation { to: 1.0; duration: 150; easing.type: Easing.InQuad }
                }
            }

            // Level info
            Column {
                spacing: 2

                Text {
                    text: "Lv. " + gamificationController.currentLevel
                    font.pixelSize: 18
                    font.bold: true
                    color: "#3C3C3C"
                }

                Text {
                    text: gamificationController.levelTitle
                    font.pixelSize: 12
                    color: "#777777"
                }
            }

            Item { Layout.fillWidth: true }

            // Streak display (fire icon)
            Rectangle {
                width: 80
                height: 44
                radius: 22
                color: gamificationController.currentStreak > 0 ? "#FFF3E0" : "#F5F5F5"
                border.color: gamificationController.currentStreak > 0 ? "#FF9600" : "#E0E0E0"
                border.width: 2

                Row {
                    anchors.centerIn: parent
                    spacing: 4

                    Text {
                        text: "🔥"
                        font.pixelSize: 20

                        // Fire animation when streak > 0
                        SequentialAnimation on rotation {
                            running: gamificationController.currentStreak > 0
                            loops: Animation.Infinite
                            NumberAnimation { from: -3; to: 3; duration: 300; easing.type: Easing.InOutSine }
                            NumberAnimation { from: 3; to: -3; duration: 300; easing.type: Easing.InOutSine }
                        }
                    }

                    Text {
                        text: gamificationController.currentStreak
                        font.pixelSize: 16
                        font.bold: true
                        color: gamificationController.currentStreak > 0 ? "#FF5722" : "#9E9E9E"
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }
            }

            // Achievements count
            Rectangle {
                width: 60
                height: 44
                radius: 22
                color: "#E8F5E9"
                border.color: "#4CAF50"
                border.width: 2

                Row {
                    anchors.centerIn: parent
                    spacing: 4

                    Text {
                        text: "🏆"
                        font.pixelSize: 18
                    }

                    Text {
                        text: gamificationController.achievementsUnlocked
                        font.pixelSize: 14
                        font.bold: true
                        color: "#2E7D32"
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }
            }
        }

        // XP Progress bar
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 4

            // XP label
            RowLayout {
                Layout.fillWidth: true

                Text {
                    text: "XP"
                    font.pixelSize: 12
                    font.bold: true
                    color: "#777777"
                }

                Item { Layout.fillWidth: true }

                Text {
                    text: gamificationController.currentXP + " / " + gamificationController.xpForNextLevel
                    font.pixelSize: 12
                    color: "#777777"
                }

                // Animated XP gain indicator
                Text {
                    id: xpGainLabel
                    text: ""
                    font.pixelSize: 14
                    font.bold: true
                    color: "#FFD700"
                    opacity: 0

                    SequentialAnimation {
                        id: xpGainAnimation
                        PropertyAnimation {
                            target: xpGainLabel
                            property: "opacity"
                            to: 1
                            duration: 200
                        }
                        PropertyAnimation {
                            target: xpGainLabel
                            property: "y"
                            from: 0
                            to: -20
                            duration: 800
                            easing.type: Easing.OutQuad
                        }
                        PropertyAnimation {
                            target: xpGainLabel
                            property: "opacity"
                            to: 0
                            duration: 300
                        }
                    }
                }
            }

            // Progress bar
            ProgressBar {
                id: xpProgressBar
                Layout.fillWidth: true
                Layout.preferredHeight: 12
                from: 0
                to: 1
                value: gamificationController.levelProgress

                background: Rectangle {
                    implicitWidth: 200
                    implicitHeight: 12
                    radius: 6
                    color: "#E8E8E8"
                }

                contentItem: Item {
                    implicitWidth: 200
                    implicitHeight: 12

                    Rectangle {
                        width: xpProgressBar.visualPosition * parent.width
                        height: parent.height
                        radius: 6
                        
                        // Gradient for XP bar
                        gradient: Gradient {
                            orientation: Gradient.Horizontal
                            GradientStop { position: 0.0; color: "#58CC02" }
                            GradientStop { position: 1.0; color: "#7FD62F" }
                        }

                        // Shine effect animation
                        Rectangle {
                            id: shineEffect
                            width: 30
                            height: parent.height
                            radius: 6
                            opacity: 0.3

                            gradient: Gradient {
                                orientation: Gradient.Horizontal
                                GradientStop { position: 0.0; color: "transparent" }
                                GradientStop { position: 0.5; color: "white" }
                                GradientStop { position: 1.0; color: "transparent" }
                            }

                            NumberAnimation on x {
                                from: -30
                                to: parent.width
                                duration: 2000
                                loops: Animation.Infinite
                                running: true
                            }
                        }
                    }
                }

                // Animate value changes
                Behavior on value {
                    NumberAnimation {
                        duration: 500
                        easing.type: Easing.OutQuad
                    }
                }
            }
        }

        // Daily challenge preview
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 36
            radius: 18
            color: "#FFF8E1"
            border.color: "#FFB300"
            border.width: 1

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 12
                anchors.rightMargin: 12
                spacing: 8

                Text {
                    text: "🎯"
                    font.pixelSize: 16
                }

                Text {
                    text: qsTr("Daily Challenge: Complete 3 tasks")
                    font.pixelSize: 12
                    color: "#5D4037"
                    Layout.fillWidth: true
                    elide: Text.ElideRight
                }

                // Mini progress
                Rectangle {
                    width: 50
                    height: 6
                    radius: 3
                    color: "#FFE0B2"

                    Rectangle {
                        width: parent.width * 0.33 // 1/3 progress
                        height: parent.height
                        radius: 3
                        color: "#FF9800"
                    }
                }

                Text {
                    text: "1/3"
                    font.pixelSize: 10
                    font.bold: true
                    color: "#FF6F00"
                }
            }
        }
    }

    // Level up connection
    Connections {
        target: gamificationController
        function onLevelUp(newLevel, newTitle) {
            levelPulseAnim.start()
        }
    }
}
