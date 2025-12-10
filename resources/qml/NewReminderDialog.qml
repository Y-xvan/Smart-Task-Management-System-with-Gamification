import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    id: dialog
    title: "New Reminder"
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    width: 320
    modal: true
    standardButtons: Dialog.Ok | Dialog.Cancel

    ColumnLayout {
        width: parent.width
        spacing: 8
        padding: 8

        TextField { id: rTitle; placeholderText: "Title"; Layout.fillWidth: true }
        TextField { id: rMessage; placeholderText: "Message (optional)"; Layout.fillWidth: true }
        TextField { id: rTime; placeholderText: "YYYY-MM-DD HH:MM"; Layout.fillWidth: true }

        Label { text: "Recurrence" }
        ComboBox {
            id: rRule
            model: ["once", "daily", "weekly", "monthly"]
            Layout.fillWidth: true
        }
    }

    onOpened: rTitle.forceActiveFocus()

    onAccepted: {
        reminderModel.addReminder(rTitle.text, rMessage.text, rTime.text, rRule.currentText)
        reminderModel.reload()
        rTitle.text = ""
        rMessage.text = ""
        rTime.text = ""
        rRule.currentIndex = 0
    }
}
