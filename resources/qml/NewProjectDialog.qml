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
    closePolicy: Popup.NoAutoClose

    ColumnLayout {
        width: parent.width
        spacing: 8

        TextField { id: pName; placeholderText: "Project Name"; Layout.fillWidth: true }
        TextField { id: pDesc; placeholderText: "Description"; Layout.fillWidth: true }

        Label { text: "Color (hex, optional)" }
        TextField { id: pColor; placeholderText: "#03A9F4"; Layout.fillWidth: true }
        Label {
            id: projectError
            visible: false
            color: "#e53935"
            text: "Project name is required"
            font.pixelSize: 12
        }
    }

    onOpened: {
        pName.forceActiveFocus()
        projectError.visible = false
    }

    onAccepted: {
        if (pName.text.trim() === "") {
            projectError.visible = true
        } else {
            projectError.visible = false
            projectModel.addProject(pName.text, pDesc.text, pColor.text)
            pName.text = ""
            pDesc.text = ""
            pColor.text = ""
            dialog.close()
        }
    }

    onRejected: {
        projectError.visible = false
        dialog.close()
    }
}
