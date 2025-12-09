import QtQuick
import QtQuick.Controls

Item {
    Rectangle { anchors.fill: parent; color: "#121212" }

    GridView {
        id: grid
        anchors.fill: parent; anchors.margins: 10
        cellWidth: width / 2
        cellHeight: 160
        clip: true
        
        model: projectModel
        delegate: ProjectCard {
            width: grid.cellWidth - 10
            height: grid.cellHeight - 10
            
            name: model.name
            desc: model.description
            colorCode: model.colorCode
            progress: model.progress
            taskCount: model.taskCountText
            
            onDelete: projectModel.deleteProject(index)
        }
    }

    Button {
        anchors.right: parent.right; anchors.bottom: parent.bottom; anchors.margins: 20
        width: 60; height: 60
        background: Rectangle { radius: 30; color: "#03A9F4" }
        contentItem: Text { text: "+" ; font.pixelSize: 32; color: "#fff"; anchors.centerIn: parent }
        onClicked: newProjDialog.open()
    }

    NewProjectDialog { id: newProjDialog }
}