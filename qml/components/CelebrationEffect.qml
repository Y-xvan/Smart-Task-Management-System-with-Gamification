/**
 * CelebrationEffect.qml
 * @brief Full-screen celebration animation for task completion
 * 
 * Shows confetti/stars particles and XP gained message
 */

import QtQuick

Item {
    id: root
    visible: false

    property int xpGained: 0

    function show(xp) {
        xpGained = xp
        visible = true
        celebrationAnim.start()
    }

    // Semi-transparent overlay
    Rectangle {
        id: overlay
        anchors.fill: parent
        color: "#80000000"
        opacity: 0

        Behavior on opacity {
            NumberAnimation { duration: 200 }
        }
    }

    // Central celebration message
    Rectangle {
        id: messageCard
        anchors.centerIn: parent
        width: 280
        height: 180
        radius: 24
        color: "#FFFFFF"
        scale: 0
        opacity: 0

        // Shadow
        layer.enabled: true
        layer.effect: Item {
            Rectangle {
                anchors.fill: parent
                anchors.topMargin: 6
                color: "#40000000"
                radius: 24
            }
        }

        Column {
            anchors.centerIn: parent
            spacing: 12

            // Success icon
            Text {
                text: "🎉"
                font.pixelSize: 48
                anchors.horizontalCenter: parent.horizontalCenter
            }

            // Success message
            Text {
                text: qsTr("Task Complete!")
                font.pixelSize: 22
                font.bold: true
                color: "#3C3C3C"
                anchors.horizontalCenter: parent.horizontalCenter
            }

            // XP gained
            Rectangle {
                width: xpLabel.implicitWidth + 24
                height: 36
                radius: 18
                color: "#FFF8E1"
                border.color: "#FFD700"
                border.width: 2
                anchors.horizontalCenter: parent.horizontalCenter

                Text {
                    id: xpLabel
                    anchors.centerIn: parent
                    text: "+" + root.xpGained + " XP"
                    font.pixelSize: 18
                    font.bold: true
                    color: "#FF8F00"
                }
            }

            // Encouraging text
            Text {
                text: getEncouragement()
                font.pixelSize: 14
                color: "#777777"
                anchors.horizontalCenter: parent.horizontalCenter

                function getEncouragement() {
                    var phrases = [
                        qsTr("Keep it up! 🔥"),
                        qsTr("You're on fire! ⭐"),
                        qsTr("Amazing work! 💪"),
                        qsTr("Unstoppable! 🚀")
                    ]
                    return phrases[Math.floor(Math.random() * phrases.length)]
                }
            }
        }
    }

    // Particle emitters for confetti effect
    Repeater {
        model: 20

        Rectangle {
            id: particle
            width: Math.random() * 12 + 6
            height: width
            radius: Math.random() > 0.5 ? width / 2 : 0 // Mix circles and squares
            x: Math.random() * root.width
            y: -50
            opacity: 0
            rotation: Math.random() * 360

            // Random colors
            color: {
                var colors = ["#FF6B6B", "#4ECDC4", "#FFE66D", "#58CC02", "#1CB0F6", "#FF9600"]
                return colors[Math.floor(Math.random() * colors.length)]
            }

            // Particle animation
            ParallelAnimation {
                id: particleAnim
                running: false

                NumberAnimation {
                    target: particle
                    property: "y"
                    from: -50
                    to: root.height + 50
                    duration: 2000 + Math.random() * 1000
                    easing.type: Easing.OutQuad
                }

                NumberAnimation {
                    target: particle
                    property: "x"
                    from: particle.x
                    to: particle.x + (Math.random() - 0.5) * 200
                    duration: 2000 + Math.random() * 1000
                }

                NumberAnimation {
                    target: particle
                    property: "rotation"
                    from: particle.rotation
                    to: particle.rotation + (Math.random() - 0.5) * 720
                    duration: 2000
                }

                SequentialAnimation {
                    NumberAnimation {
                        target: particle
                        property: "opacity"
                        from: 0
                        to: 1
                        duration: 200
                    }
                    PauseAnimation { duration: 1500 }
                    NumberAnimation {
                        target: particle
                        property: "opacity"
                        from: 1
                        to: 0
                        duration: 500
                    }
                }
            }

            Component.onCompleted: {
                // Start with random delay
                Qt.callLater(function() {
                    particleAnim.start()
                })
            }
        }
    }

    // Main celebration animation sequence
    SequentialAnimation {
        id: celebrationAnim

        // Show overlay
        PropertyAction {
            target: overlay
            property: "opacity"
            value: 1
        }

        // Show message card with bounce
        ParallelAnimation {
            NumberAnimation {
                target: messageCard
                property: "scale"
                from: 0
                to: 1
                duration: 400
                easing.type: Easing.OutBack
                easing.overshoot: 1.5
            }
            NumberAnimation {
                target: messageCard
                property: "opacity"
                from: 0
                to: 1
                duration: 300
            }
        }

        // Hold
        PauseAnimation { duration: 1500 }

        // Fade out
        ParallelAnimation {
            NumberAnimation {
                target: messageCard
                property: "opacity"
                to: 0
                duration: 300
            }
            NumberAnimation {
                target: messageCard
                property: "scale"
                to: 0.8
                duration: 300
            }
            NumberAnimation {
                target: overlay
                property: "opacity"
                to: 0
                duration: 300
            }
        }

        // Hide
        PropertyAction {
            target: root
            property: "visible"
            value: false
        }
    }

    // Allow clicking to dismiss early
    MouseArea {
        anchors.fill: parent
        onClicked: {
            if (celebrationAnim.running) {
                celebrationAnim.stop()
                overlay.opacity = 0
                messageCard.scale = 0
                messageCard.opacity = 0
                root.visible = false
            }
        }
    }
}
