/**
 * NavigationBar.qml
 * @brief Bottom navigation bar with Duolingo-style icons and animations
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts

Rectangle {
    id: root
    color: "#FFFFFF"

    // Signal for navigation
    signal navigateTo(string pageName)

    // Current selected tab
    property int currentIndex: 0

    // Navigation items configuration
    readonly property var navItems: [
        { icon: "📋", label: qsTr("Tasks"), page: "tasks" },
        { icon: "📁", label: qsTr("Projects"), page: "projects" },
        { icon: "🏆", label: qsTr("Achievements"), page: "achievements" },
        { icon: "📊", label: qsTr("Statistics"), page: "statistics" },
        { icon: "⚙️", label: qsTr("Settings"), page: "settings" }
    ]

    // Top border
    Rectangle {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 1
        color: "#E0E0E0"
    }

    RowLayout {
        anchors.fill: parent
        anchors.margins: 4
        spacing: 0

        Repeater {
            model: navItems

            delegate: Item {
                Layout.fillWidth: true
                Layout.fillHeight: true

                property bool isSelected: root.currentIndex === index

                // Background highlight for selected item
                Rectangle {
                    anchors.centerIn: parent
                    width: parent.width - 8
                    height: parent.height - 8
                    radius: 16
                    color: isSelected ? "#E8F5E9" : "transparent"
                    border.color: isSelected ? "#58CC02" : "transparent"
                    border.width: isSelected ? 2 : 0

                    Behavior on color {
                        ColorAnimation { duration: 200 }
                    }
                }

                Column {
                    anchors.centerIn: parent
                    spacing: 2

                    // Icon with bounce animation
                    Text {
                        id: iconText
                        text: modelData.icon
                        font.pixelSize: isSelected ? 24 : 20
                        anchors.horizontalCenter: parent.horizontalCenter

                        // Bounce animation on selection
                        scale: isSelected ? 1.0 : 0.9
                        
                        Behavior on font.pixelSize {
                            NumberAnimation {
                                duration: 200
                                easing.type: Easing.OutBack
                            }
                        }

                        Behavior on scale {
                            NumberAnimation {
                                duration: 200
                                easing.type: Easing.OutBack
                            }
                        }

                        // Jump animation when clicked
                        SequentialAnimation {
                            id: bounceAnim
                            NumberAnimation {
                                target: iconText
                                property: "y"
                                from: 0
                                to: -8
                                duration: 100
                                easing.type: Easing.OutQuad
                            }
                            NumberAnimation {
                                target: iconText
                                property: "y"
                                from: -8
                                to: 0
                                duration: 150
                                easing.type: Easing.OutBounce
                            }
                        }
                    }

                    // Label
                    Text {
                        text: modelData.label
                        font.pixelSize: 10
                        font.bold: isSelected
                        color: isSelected ? "#58CC02" : "#777777"
                        anchors.horizontalCenter: parent.horizontalCenter

                        Behavior on color {
                            ColorAnimation { duration: 200 }
                        }
                    }
                }

                // Click area
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        if (root.currentIndex !== index) {
                            root.currentIndex = index
                            bounceAnim.start()
                            root.navigateTo(modelData.page)
                        }
                    }
                }
            }
        }
    }
}
