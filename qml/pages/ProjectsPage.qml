/**
 * ProjectsPage.qml
 * @brief Projects list page
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
                text: "📁"
                font.pixelSize: 24
            }

            Text {
                text: qsTr("Projects")
                font.pixelSize: 20
                font.bold: true
                color: "#3C3C3C"
                Layout.fillWidth: true
            }

            Rectangle {
                width: projectCountText.implicitWidth + 16
                height: 28
                radius: 14
                color: "#E8F5E9"
                border.color: "#4CAF50"
                border.width: 1

                Text {
                    id: projectCountText
                    anchors.centerIn: parent
                    text: projectListModel.count + qsTr(" projects")
                    font.pixelSize: 12
                    color: "#2E7D32"
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

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // Empty state
        Item {
            visible: projectListModel.count === 0
            Layout.fillWidth: true
            Layout.fillHeight: true

            Column {
                anchors.centerIn: parent
                spacing: 16

                Rectangle {
                    width: 150
                    height: 150
                    radius: 75
                    color: "#E3F2FD"
                    anchors.horizontalCenter: parent.horizontalCenter

                    Text {
                        anchors.centerIn: parent
                        text: "📁"
                        font.pixelSize: 64
                        opacity: 0.5
                    }
                }

                Text {
                    text: qsTr("No projects yet!")
                    font.pixelSize: 20
                    font.bold: true
                    color: "#3C3C3C"
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                Text {
                    text: qsTr("Create a project to organize your quests")
                    font.pixelSize: 14
                    color: "#777777"
                    anchors.horizontalCenter: parent.horizontalCenter
                }
            }
        }

        // Project list
        ListView {
            id: projectListView
            visible: projectListModel.count > 0
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.margins: 12
            spacing: 12
            clip: true

            model: projectListModel

            delegate: Rectangle {
                width: projectListView.width
                height: projectContent.implicitHeight + 24
                radius: 16
                color: "#FFFFFF"
                border.color: model.colorLabel || "#1CB0F6"
                border.width: 2

                // Press animation
                scale: projectMouse.pressed ? 0.98 : 1.0
                Behavior on scale {
                    NumberAnimation { duration: 100 }
                }

                MouseArea {
                    id: projectMouse
                    anchors.fill: parent
                    onClicked: {
                        // TODO: Navigate to project detail
                    }
                }

                ColumnLayout {
                    id: projectContent
                    anchors.fill: parent
                    anchors.margins: 12
                    spacing: 8

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 8

                        // Color indicator
                        Rectangle {
                            width: 12
                            height: 12
                            radius: 6
                            color: model.colorLabel || "#1CB0F6"
                        }

                        Text {
                            text: model.name
                            font.pixelSize: 16
                            font.bold: true
                            color: "#3C3C3C"
                            Layout.fillWidth: true
                            elide: Text.ElideRight
                        }

                        // Archived badge
                        Rectangle {
                            visible: model.archived
                            width: archivedLabel.implicitWidth + 12
                            height: 20
                            radius: 10
                            color: "#F5F5F5"

                            Text {
                                id: archivedLabel
                                anchors.centerIn: parent
                                text: qsTr("Archived")
                                font.pixelSize: 10
                                color: "#9E9E9E"
                            }
                        }
                    }

                    Text {
                        visible: model.description !== ""
                        text: model.description
                        font.pixelSize: 13
                        color: "#777777"
                        Layout.fillWidth: true
                        wrapMode: Text.WordWrap
                        maximumLineCount: 2
                        elide: Text.ElideRight
                    }

                    // Progress bar
                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 8

                        Rectangle {
                            Layout.fillWidth: true
                            height: 8
                            radius: 4
                            color: "#E8E8E8"

                            Rectangle {
                                width: parent.width * (model.progress || 0)
                                height: parent.height
                                radius: 4
                                color: model.colorLabel || "#1CB0F6"

                                Behavior on width {
                                    NumberAnimation { duration: 300 }
                                }
                            }
                        }

                        Text {
                            text: Math.round((model.progress || 0) * 100) + "%"
                            font.pixelSize: 12
                            font.bold: true
                            color: model.colorLabel || "#1CB0F6"
                        }
                    }

                    // Task count
                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 8

                        Text {
                            text: "📋"
                            font.pixelSize: 14
                        }

                        Text {
                            text: model.progressText || "0/0"
                            font.pixelSize: 12
                            color: "#777777"
                        }

                        Item { Layout.fillWidth: true }

                        Text {
                            visible: model.targetDate !== ""
                            text: "📅 " + (model.targetDate || "")
                            font.pixelSize: 12
                            color: "#777777"
                        }
                    }
                }
            }

            add: Transition {
                NumberAnimation { property: "opacity"; from: 0; to: 1; duration: 300 }
            }
        }
    }

    // FAB
    RoundButton {
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 20
        width: 60
        height: 60
        text: "➕"
        font.pixelSize: 24
        Material.background: "#1CB0F6"
        Material.foreground: "white"

        scale: pressed ? 0.9 : 1.0
        Behavior on scale {
            NumberAnimation { duration: 100 }
        }

        onClicked: {
            // TODO: Create project dialog
        }
    }
}
