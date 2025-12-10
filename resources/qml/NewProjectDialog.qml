import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    id: dialog
    title: "New Project"
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    width: 320
    modal: true
    standardButtons: Dialog.Ok | Dialog.Cancel

    ColumnLayout {
        width: parent.width
        spacing: 8
        padding: 8

        TextField { id: pName; placeholderText: "Project Name"; Layout.fillWidth: true }
        TextField { id: pDesc; placeholderText: "Description"; Layout.fillWidth: true }

        Label { text: "Color (hex, optional)" }
        TextField { id: pColor; placeholderText: "#03A9F4"; Layout.fillWidth: true }
    }

    onOpened: {
        pName.forceActiveFocus()
    }

    onAccepted: {
        projectModel.addProject(pName.text, pDesc.text, pColor.text)
        projectModel.reload()
        taskModel.reload()
        pName.text = ""
        pDesc.text = ""
        pColor.text = ""
    }
}
