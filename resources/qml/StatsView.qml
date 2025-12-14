import QtQuick
import QtQuick.Controls

Item {
    Rectangle { anchors.fill: parent; color: "#121212" }
    
    Flickable {
        anchors.fill: parent
        contentHeight: col.height + 50
        
        Column {
            id: col
            width: parent.width
            spacing: 20
            padding: 20

            Text { text: "📈 Statistics"; color: "white"; font.bold: true; font.pixelSize: 20 }
            
            Row {
                spacing: 20
                StatsCard { label: "Total Tasks"; value: gameController.totalTasks }
                StatsCard { label: "Pomodoros"; value: gameController.totalPomodoros }
            }
            
            Text { text: "🏆 Achievements"; color: "white"; font.bold: true; font.pixelSize: 20 }
            
            // Dynamic achievements display - shows locked by default until earned
            Repeater {
                model: ListModel {
                    ListElement { 
                        achievementName: "First Blood"
                        achievementDesc: "Complete your first task"
                        checkProperty: "totalTasks"
                        threshold: 1
                    }
                    ListElement { 
                        achievementName: "Task Master"
                        achievementDesc: "Complete 10 tasks"
                        checkProperty: "totalTasks"
                        threshold: 10
                    }
                    ListElement { 
                        achievementName: "Pomodoro Pro"
                        achievementDesc: "Complete 5 pomodoros"
                        checkProperty: "totalPomodoros"
                        threshold: 5
                    }
                }
                Rectangle {
                    width: parent.width - 40; height: 70
                    color: "#1e1e1e"; radius: 8
                    
                    property bool isUnlocked: {
                        if (model.checkProperty === "totalTasks") {
                            return gameController.totalTasks >= model.threshold
                        } else if (model.checkProperty === "totalPomodoros") {
                            return gameController.totalPomodoros >= model.threshold
                        }
                        return false
                    }
                    
                    Row {
                        anchors.centerIn: parent; spacing: 15
                        Text { text: isUnlocked ? "✅" : "🔒"; font.pixelSize: 24 }
                        Column {
                            Text { text: model.achievementName; color: "white"; font.bold: true }
                            Text { text: model.achievementDesc; color: "#888"; font.pixelSize: 12 }
                        }
                    }
                }
            }
        }
    }
    
    component StatsCard: Rectangle {
        property string label
        property string value
        width: 150; height: 100; color: "#1e1e1e"; radius: 8
        Column {
            anchors.centerIn: parent
            Text { text: value; color: "#03A9F4"; font.pixelSize: 32; font.bold: true; anchors.horizontalCenter: parent.horizontalCenter }
            Text { text: label; color: "#888"; font.pixelSize: 12; anchors.horizontalCenter: parent.horizontalCenter }
        }
    }
}