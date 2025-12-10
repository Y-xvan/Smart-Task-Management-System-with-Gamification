import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    id: dialog
    title: "New Project"
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    width: 300
    modal: true
    standardButtons: Dialog.Ok | Dialog.Cancel

    ColumnLayout {
        width: parent.width
        spacing: 8

        TextField { id: pName; placeholderText: "Project Name"; Layout.fillWidth: true }
        TextField { id: pDesc; placeholderText: "Description (optional)"; Layout.fillWidth: true }
        TextField { id: pColor; placeholderText: "Color hex (e.g. #03A9F4)"; Layout.fillWidth: true }
    }

    onAccepted: {
        var color = pColor.text === "" ? "#4CAF50" : pColor.text
        projectModel.addProject(pName.text, pDesc.text, color)

        // reset for next use
        pName.text = ""
        pDesc.text = ""
        pColor.text = ""
    }
}
