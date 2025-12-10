import QtQuick
import QtQuick.Controls

Item {
    // ����
    Rectangle {
        anchors.fill: parent
        color: "#121212"
    }

    // �б�
    ListView {
        id: taskList
        anchors.fill: parent
        anchors.margins: 12
        spacing: 12
        clip: true
        
        model: taskModel

        delegate: QuestCard {
            width: taskList.width
            
            // ������
            title: model.title
            desc: model.description
            priority: model.priority
            xp: model.xpReward
            projectName: model.projectName
            dueDate: model.dueDate

            // �źŴ���
            onComplete: {
                taskModel.completeTask(index)
                gameController.refresh()
            }
            onDelete: {
                taskModel.deleteTask(index)
                gameController.refresh()
            }
        }

        // �ײ�����
        footer: Item { height: 80 }
    }

    // �½���ť (FAB)
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
