import QtQuick
import QtQuick.Controls

Item {
    Rectangle { anchors.fill: parent; color: "#121212" }
    
    ListView {
        id: list
        anchors.fill: parent; anchors.margins: 10
        spacing: 10
        model: reminderModel
        
        delegate: Rectangle {
            width: list.width; height: 80
            color: "#1e1e1e"; radius: 8
            
            Row {
                anchors.fill: parent; anchors.margins: 10
                spacing: 10
                Text { text: "⏰"; font.pixelSize: 24; anchors.verticalCenter: parent.verticalCenter }
                Column {
                    anchors.verticalCenter: parent.verticalCenter
                    Text { text: model.title; color: "white"; font.bold: true }
                    Text { text: model.time; color: "#03A9F4" }
                }
            }
            
            Button {
                text: "✖"
                anchors.right: parent.right; anchors.verticalCenter: parent.verticalCenter
                flat: true
                onClicked: reminderModel.deleteReminder(index)
            }
        }
    }
    
    Button {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom; anchors.margins: 20
        text: "Add Reminder"
        onClicked: newRemDialog.open()
    }
    
    NewReminderDialog { id: newRemDialog }
}
