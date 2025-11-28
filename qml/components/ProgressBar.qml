/**
 * ProgressBar.qml
 * @brief Custom progress bar with animation and Duolingo style
 */

import QtQuick
import QtQuick.Controls

Item {
    id: root

    property real value: 0      // 0.0 to 1.0
    property color backgroundColor: "#E8E8E8"
    property color fillColor: "#58CC02"
    property color fillGradientEnd: "#7FD62F"
    property int animationDuration: 500
    property bool showShine: true

    implicitHeight: 12
    implicitWidth: 200

    Rectangle {
        id: background
        anchors.fill: parent
        radius: height / 2
        color: root.backgroundColor
    }

    Rectangle {
        id: fill
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        height: parent.height
        width: Math.max(height, parent.width * root.value)
        radius: height / 2

        gradient: Gradient {
            orientation: Gradient.Horizontal
            GradientStop { position: 0.0; color: root.fillColor }
            GradientStop { position: 1.0; color: root.fillGradientEnd }
        }

        Behavior on width {
            NumberAnimation {
                duration: root.animationDuration
                easing.type: Easing.OutQuad
            }
        }

        // Shine animation
        Rectangle {
            id: shine
            visible: root.showShine && root.value > 0
            width: 20
            height: parent.height
            radius: parent.radius
            opacity: 0.4

            gradient: Gradient {
                orientation: Gradient.Horizontal
                GradientStop { position: 0.0; color: "transparent" }
                GradientStop { position: 0.5; color: "white" }
                GradientStop { position: 1.0; color: "transparent" }
            }

            NumberAnimation on x {
                from: -20
                to: fill.width
                duration: 1500
                loops: Animation.Infinite
                running: root.showShine && root.value > 0.1
            }

            clip: true
        }
    }
}
