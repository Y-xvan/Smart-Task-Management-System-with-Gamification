import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    Rectangle { anchors.fill: parent; color: "#121212" }

    ColumnLayout {
        anchors.centerIn: parent
        spacing: 40

        // 圆形进度条
        Item {
            width: 260; height: 260
            Layout.alignment: Qt.AlignHCenter
            
            Rectangle {
                anchors.fill: parent; radius: 130
                color: "transparent"
                border.color: "#333"; border.width: 10
            }

            // 模拟进度环 (简化版，用矩形遮罩模拟)
            // 注意：在 QML 中绘制完美圆环最好用 Canvas 或 Shape，这里为了代码简洁性和兼容性，
            // 我们用一个填充的半透明圆来表示进度。
            Rectangle {
                anchors.bottom: parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width
                height: parent.height * gameController.timerProgress
                color: gameController.timerMode === "Work" ? "#F44336" : "#03A9F4"
                opacity: 0.3
                radius: 130
                clip: true
            }

            Column {
                anchors.centerIn: parent
                Text { 
                    text: gameController.timerMode
                    color: "#888"; font.pixelSize: 18; anchors.horizontalCenter: parent.horizontalCenter 
                }
                Text { 
                    text: gameController.timerText
                    color: "#fff"; font.pixelSize: 48; font.bold: true 
                }
            }
        }

        // 控制按钮
        RowLayout {
            spacing: 20
            Layout.alignment: Qt.AlignHCenter
            
            Button {
                text: "Focus (25m)"
                highlighted: true
                enabled: !gameController.isTimerRunning
                onClicked: gameController.startTimer("Work")
            }
            
            Button {
                text: "Short Break"
                enabled: !gameController.isTimerRunning
                onClicked: gameController.startTimer("Short")
            }
        }
        
        Button {
            text: "STOP"
            Layout.alignment: Qt.AlignHCenter
            visible: gameController.isTimerRunning
            background: Rectangle { color: "#D32F2F"; radius: 4 }
            contentItem: Text { text: "STOP"; color: "white"; anchors.centerIn: parent }
            onClicked: gameController.stopTimer()
        }
        
        // 统计信息
        Text {
            text: "Total Pomodoros: " + gameController.totalPomodoros
            color: "#666"
            Layout.alignment: Qt.AlignHCenter
        }
    }
}