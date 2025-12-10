import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    id: dialog
    title: "New Reminder"
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    width: 300
    modal: true
    standardButtons: Dialog.Ok | Dialog.Cancel

    ColumnLayout {
        width: parent.width
        spacing: 8

        TextField { id: rTitle; placeholderText: "Reminder Title"; Layout.fillWidth: true }
        TextField { id: rTime; placeholderText: "YYYY-MM-DD HH:MM:SS"; Layout.fillWidth: true }
    }

    onAccepted: {
        reminderModel.addReminder(rTitle.text, rTime.text)
        rTitle.text = ""
        rTime.text = ""
    }
}
