import QtQuick
import QtQuick.Controls

Item {
    // 背景
    Rectangle {
        anchors.fill: parent
        color: "#121212"
    }

    // 列表
    ListView {
        id: taskList
        anchors.fill: parent
        anchors.margins: 12
        spacing: 12
        clip: true
        
        model: taskModel

        delegate: QuestCard {
            width: taskList.width
            
            // 绑定数据
            title: model.title
            desc: model.description
            priority: model.priority
            xp: model.xpReward
            projectName: model.projectName
            dueDate: model.dueDate

            // 信号处理
            onComplete: taskModel.completeTask(index)
            onDelete: taskModel.deleteTask(index)
        }

        // 底部留白
        footer: Item { height: 80 }
    }

    // 新建按钮 (FAB)
    Button {
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 20
        width: 60; height: 60
        
        background: Rectangle {
            radius: 30
            color: "#FFD700"
            layer.enabled: true
        }
        
        contentItem: Text {
            text: "+"
            font.pixelSize: 32
            color: "#000"
            anchors.centerIn: parent
        }

        onClicked: newTaskDialog.open()
    }

    NewTaskDialog { id: newTaskDialog }
}