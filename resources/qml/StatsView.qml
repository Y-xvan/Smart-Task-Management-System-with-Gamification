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
            
            // 这里可以做成 ListView 绑定 AchievementModel，暂时静态模拟
            Repeater {
                model: 3 
                Rectangle {
                    width: parent.width - 40; height: 70
                    color: "#1e1e1e"; radius: 8
                    Row {
                        anchors.centerIn: parent; spacing: 15
                        Text { text: index === 0 ? "✅" : "🔒"; font.pixelSize: 24 }
                        Column {
                            Text { text: index === 0 ? "First Blood" : "Locked Achievement"; color: "white"; font.bold: true }
                            Text { text: index === 0 ? "Complete your first task" : "Keep playing..."; color: "#888"; font.pixelSize: 12 }
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