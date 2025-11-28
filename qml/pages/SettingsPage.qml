/**
 * SettingsPage.qml
 * @brief Settings and preferences page
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
                text: "⚙️"
                font.pixelSize: 24
            }

            Text {
                text: qsTr("Settings")
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
            spacing: 0

            // Pomodoro Settings
            Rectangle {
                Layout.fillWidth: true
                Layout.margins: 12
                height: pomodoroContent.implicitHeight + 32
                radius: 16
                color: "#FFFFFF"

                ColumnLayout {
                    id: pomodoroContent
                    anchors.fill: parent
                    anchors.margins: 16
                    spacing: 16

                    Text {
                        text: qsTr("🍅 Pomodoro Settings")
                        font.pixelSize: 16
                        font.bold: true
                        color: "#3C3C3C"
                    }

                    // Work duration
                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 12

                        Text {
                            text: qsTr("Work Duration")
                            font.pixelSize: 14
                            color: "#3C3C3C"
                            Layout.fillWidth: true
                        }

                        SpinBox {
                            id: workDurationSpinner
                            from: 15
                            to: 60
                            value: 25
                            stepSize: 5

                            textFromValue: function(value) {
                                return value + qsTr(" min")
                            }
                        }
                    }

                    // Short break
                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 12

                        Text {
                            text: qsTr("Short Break")
                            font.pixelSize: 14
                            color: "#3C3C3C"
                            Layout.fillWidth: true
                        }

                        SpinBox {
                            from: 3
                            to: 15
                            value: 5
                            stepSize: 1

                            textFromValue: function(value) {
                                return value + qsTr(" min")
                            }
                        }
                    }

                    // Long break
                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 12

                        Text {
                            text: qsTr("Long Break")
                            font.pixelSize: 14
                            color: "#3C3C3C"
                            Layout.fillWidth: true
                        }

                        SpinBox {
                            from: 10
                            to: 30
                            value: 15
                            stepSize: 5

                            textFromValue: function(value) {
                                return value + qsTr(" min")
                            }
                        }
                    }
                }
            }

            // Notification Settings
            Rectangle {
                Layout.fillWidth: true
                Layout.margins: 12
                height: notificationContent.implicitHeight + 32
                radius: 16
                color: "#FFFFFF"

                ColumnLayout {
                    id: notificationContent
                    anchors.fill: parent
                    anchors.margins: 16
                    spacing: 16

                    Text {
                        text: qsTr("🔔 Notifications")
                        font.pixelSize: 16
                        font.bold: true
                        color: "#3C3C3C"
                    }

                    // Sound
                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 12

                        Text {
                            text: qsTr("Sound")
                            font.pixelSize: 14
                            color: "#3C3C3C"
                            Layout.fillWidth: true
                        }

                        Switch {
                            checked: true
                        }
                    }

                    // Desktop notifications
                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 12

                        Text {
                            text: qsTr("Desktop Notifications")
                            font.pixelSize: 14
                            color: "#3C3C3C"
                            Layout.fillWidth: true
                        }

                        Switch {
                            checked: true
                        }
                    }

                    // Reminders
                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 12

                        Text {
                            text: qsTr("Task Reminders")
                            font.pixelSize: 14
                            color: "#3C3C3C"
                            Layout.fillWidth: true
                        }

                        Switch {
                            checked: true
                        }
                    }
                }
            }

            // Appearance Settings
            Rectangle {
                Layout.fillWidth: true
                Layout.margins: 12
                height: appearanceContent.implicitHeight + 32
                radius: 16
                color: "#FFFFFF"

                ColumnLayout {
                    id: appearanceContent
                    anchors.fill: parent
                    anchors.margins: 16
                    spacing: 16

                    Text {
                        text: qsTr("🎨 Appearance")
                        font.pixelSize: 16
                        font.bold: true
                        color: "#3C3C3C"
                    }

                    // Theme selection
                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 12

                        Text {
                            text: qsTr("Theme")
                            font.pixelSize: 14
                            color: "#3C3C3C"
                            Layout.fillWidth: true
                        }

                        ComboBox {
                            model: [qsTr("Light"), qsTr("Dark"), qsTr("System")]
                            currentIndex: 0
                        }
                    }

                    // Language
                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 12

                        Text {
                            text: qsTr("Language")
                            font.pixelSize: 14
                            color: "#3C3C3C"
                            Layout.fillWidth: true
                        }

                        ComboBox {
                            model: ["English", "中文", "日本語"]
                            currentIndex: 0
                        }
                    }
                }
            }

            // About section
            Rectangle {
                Layout.fillWidth: true
                Layout.margins: 12
                height: aboutContent.implicitHeight + 32
                radius: 16
                color: "#FFFFFF"

                ColumnLayout {
                    id: aboutContent
                    anchors.fill: parent
                    anchors.margins: 16
                    spacing: 12

                    Text {
                        text: qsTr("ℹ️ About")
                        font.pixelSize: 16
                        font.bold: true
                        color: "#3C3C3C"
                    }

                    Text {
                        text: qsTr("Smart Task Manager v1.0.0")
                        font.pixelSize: 14
                        color: "#3C3C3C"
                    }

                    Text {
                        text: qsTr("A gamified task management system with Duolingo-style UI")
                        font.pixelSize: 12
                        color: "#777777"
                        wrapMode: Text.WordWrap
                        Layout.fillWidth: true
                    }

                    Text {
                        text: qsTr("Built with Qt6/QML and C++")
                        font.pixelSize: 12
                        color: "#777777"
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
