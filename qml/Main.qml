/**
 * Main.qml
 * @brief Main application window - Quest Tracker style (Duolingo-inspired)
 * 
 * Design Philosophy:
 * - Fun, vivid, interactive
 * - Rounded corners and soft shadows
 * - Bouncy animations
 * - Game-like feedback
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import "components"
import "pages"

ApplicationWindow {
    id: window
    visible: true
    width: 420
    height: 800
    minimumWidth: 380
    minimumHeight: 600
    title: qsTr("Smart Task Manager")

    // Material Design theme configuration (Duolingo-style colors)
    Material.theme: Material.Light
    Material.accent: "#58CC02"      // Duolingo green
    Material.primary: "#1CB0F6"      // Duolingo blue
    Material.background: "#F7F7F7"

    // Custom color palette
    readonly property color colorSuccess: "#58CC02"      // Green
    readonly property color colorPrimary: "#1CB0F6"      // Blue
    readonly property color colorWarning: "#FF9600"      // Orange
    readonly property color colorDanger: "#FF4B4B"       // Red
    readonly property color colorXP: "#FFD700"           // Gold
    readonly property color colorStreak: "#FF5722"       // Fire orange
    readonly property color colorBackground: "#F7F7F7"
    readonly property color colorCard: "#FFFFFF"
    readonly property color colorText: "#3C3C3C"
    readonly property color colorTextSecondary: "#777777"

    // Background
    Rectangle {
        anchors.fill: parent
        color: colorBackground
    }

    // Main layout
    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // Gamification HUD (Top bar)
        GamificationHUD {
            id: gamificationHUD
            Layout.fillWidth: true
            Layout.preferredHeight: 140
        }

        // Page Stack
        StackView {
            id: stackView
            Layout.fillWidth: true
            Layout.fillHeight: true

            initialItem: taskListPage

            // Custom transitions with bounce effect
            pushEnter: Transition {
                ParallelAnimation {
                    PropertyAnimation {
                        property: "opacity"
                        from: 0
                        to: 1
                        duration: 300
                        easing.type: Easing.OutQuad
                    }
                    PropertyAnimation {
                        property: "x"
                        from: stackView.width * 0.3
                        to: 0
                        duration: 400
                        easing.type: Easing.OutBack
                        easing.overshoot: 1.2
                    }
                }
            }

            pushExit: Transition {
                PropertyAnimation {
                    property: "opacity"
                    from: 1
                    to: 0
                    duration: 200
                }
            }

            popEnter: Transition {
                PropertyAnimation {
                    property: "opacity"
                    from: 0
                    to: 1
                    duration: 300
                }
            }

            popExit: Transition {
                ParallelAnimation {
                    PropertyAnimation {
                        property: "opacity"
                        from: 1
                        to: 0
                        duration: 200
                    }
                    PropertyAnimation {
                        property: "x"
                        from: 0
                        to: stackView.width * 0.3
                        duration: 300
                        easing.type: Easing.InQuad
                    }
                }
            }
        }

        // Navigation Bar
        NavigationBar {
            id: navigationBar
            Layout.fillWidth: true
            Layout.preferredHeight: 70

            onNavigateTo: function(pageName) {
                switch(pageName) {
                    case "tasks":
                        stackView.replace(taskListPage)
                        break
                    case "projects":
                        stackView.replace(projectsPage)
                        break
                    case "achievements":
                        stackView.replace(achievementsPage)
                        break
                    case "statistics":
                        stackView.replace(statisticsPage)
                        break
                    case "settings":
                        stackView.replace(settingsPage)
                        break
                }
            }
        }
    }

    // Page components
    Component {
        id: taskListPage
        TaskListPage {}
    }

    Component {
        id: projectsPage
        ProjectsPage {}
    }

    Component {
        id: achievementsPage
        AchievementsPage {}
    }

    Component {
        id: statisticsPage
        StatisticsPage {}
    }

    Component {
        id: settingsPage
        SettingsPage {}
    }

    // Celebration overlay for completed tasks
    CelebrationEffect {
        id: celebrationEffect
        anchors.fill: parent
        z: 1000
    }

    // Global functions
    function showCelebration(xpGained) {
        celebrationEffect.show(xpGained)
        gamificationHUD.animateXPGain(xpGained)
    }

    // Connect to task completion signal
    Connections {
        target: taskListModel
        function onTaskCompleted(taskId, xpGained) {
            showCelebration(xpGained)
        }
    }
}
