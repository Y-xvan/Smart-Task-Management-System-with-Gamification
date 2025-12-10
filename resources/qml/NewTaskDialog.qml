import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    id: dialog
    title: "New Quest"
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    width: 300
    modal: true
    standardButtons: Dialog.Ok | Dialog.Cancel

    ColumnLayout {
        width: parent.width
        TextField { id: tTitle; placeholderText: "Quest Title"; Layout.fillWidth: true }
        TextField { id: tDesc; placeholderText: "Description"; Layout.fillWidth: true }
        
        Label { text: "Priority" }
        ComboBox { 
            id: tPrio; model: ["Low", "Medium", "High"]
            Layout.fillWidth: true 
        }

        Label { text: "Project (Optional)" }
        ComboBox {
            id: tProj
            model: projectModel.getProjectNames()
            Layout.fillWidth: true
        }

        Label { text: "Estimated Pomodoros" }
        SpinBox { id: tPomo; from: 0; to: 10; value: 1 }
        
        Label { text: "Due Date (YYYY-MM-DD)" }
        TextField { id: tDate; placeholderText: "2025-12-31"; Layout.fillWidth: true }
    }

    onOpened: {
        tProj.model = projectModel.getProjectNames()
    }

    onAccepted: {
        // ��ȡ Project ID (��Ҫӳ��)
        var pid = -1
        if (tProj.currentIndex >= 0) {
             pid = projectModel.getProjectId(tProj.currentIndex)
        }
        taskModel.addTask(tTitle.text, tDesc.text, tPrio.currentIndex, pid, tPomo.value, tDate.text)
        gameController.refresh()

        // Reset
        tTitle.text = ""
        tDesc.text = ""
    }
}
