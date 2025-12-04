import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

/**
 * QuestCard.qml - Individual task delegate styled as a "Wanted Poster" / Quest Card
 * 
 * Design: Parchment background with wax seal for completion
 * Interaction: Long-press to complete a task
 */
Item {
    id: questCard
    height: cardContent.height + 20
    
    // Properties
    property string questTitle: ""
    property string questDescription: ""
    property int questPriority: 1  // 0=low, 1=medium, 2=high
    property string questDueDate: ""
    property int questXPReward: 0
    property bool questCompleted: false
    
    // Signals
    signal questAccepted()
    
    // RPG Theme Colors
    readonly property color parchmentBeige: "#F5E6D3"
    readonly property color inkBlack: "#2C3E50"
    readonly property color urgentRed: "#E74C3C"
    readonly property color dailyBlue: "#3498DB"
    readonly property color goldColor: "#F1C40F"
    readonly property color darkParchment: "#D4C4A8"
    readonly property color completedGreen: "#27AE60"
    readonly property color waxRed: "#8B0000"
    
    // Priority color mapping
    function getPriorityColor() {
        switch (questPriority) {
            case 0: return dailyBlue     // Low priority
            case 1: return goldColor     // Medium priority
            case 2: return urgentRed     // High priority
            default: return dailyBlue
        }
    }
    
    function getPriorityLabel() {
        switch (questPriority) {
            case 0: return "Common"
            case 1: return "Rare"
            case 2: return "Epic"
            default: return "Common"
        }
    }
    
    // Card background (parchment style)
    Rectangle {
        id: cardBackground
        anchors.fill: parent
        color: questCompleted ? Qt.darker(parchmentBeige, 1.1) : parchmentBeige
        radius: 8
        border.color: questCompleted ? completedGreen : getPriorityColor()
        border.width: 3
        
        // Parchment texture effect (subtle gradient)
        gradient: Gradient {
            GradientStop { position: 0.0; color: questCompleted ? Qt.darker(parchmentBeige, 1.05) : parchmentBeige }
            GradientStop { position: 0.5; color: questCompleted ? Qt.darker(darkParchment, 1.05) : darkParchment }
            GradientStop { position: 1.0; color: questCompleted ? Qt.darker(parchmentBeige, 1.05) : parchmentBeige }
        }
        
        // Shadow effect
        layer.enabled: true
        layer.effect: Item {
            // Simple shadow simulation
        }
    }
    
    // Completed overlay
    Rectangle {
        anchors.fill: parent
        color: "black"
        opacity: questCompleted ? 0.2 : 0
        radius: 8
        
        Behavior on opacity {
            NumberAnimation { duration: 300 }
        }
    }
    
    // Card content
    ColumnLayout {
        id: cardContent
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 15
        spacing: 8
        
        // Top row: Priority badge and XP reward
        RowLayout {
            Layout.fillWidth: true
            
            // Priority badge
            Rectangle {
                Layout.preferredWidth: 70
                Layout.preferredHeight: 24
                color: getPriorityColor()
                radius: 12
                
                Text {
                    anchors.centerIn: parent
                    text: getPriorityLabel()
                    font.pixelSize: 10
                    font.bold: true
                    color: "white"
                }
            }
            
            Item { Layout.fillWidth: true }
            
            // XP reward badge
            Rectangle {
                Layout.preferredWidth: 60
                Layout.preferredHeight: 24
                color: goldColor
                radius: 12
                
                Text {
                    anchors.centerIn: parent
                    text: "+" + questXPReward + " XP"
                    font.pixelSize: 10
                    font.bold: true
                    color: inkBlack
                }
            }
        }
        
        // Quest title
        Text {
            Layout.fillWidth: true
            text: questTitle
            font.pixelSize: 18
            font.bold: true
            color: inkBlack
            wrapMode: Text.WordWrap
            
            // Strikethrough for completed
            font.strikeout: questCompleted
            opacity: questCompleted ? 0.6 : 1.0
        }
        
        // Quest description
        Text {
            Layout.fillWidth: true
            text: questDescription
            font.pixelSize: 13
            color: inkBlack
            opacity: questCompleted ? 0.5 : 0.8
            wrapMode: Text.WordWrap
            maximumLineCount: 2
            elide: Text.ElideRight
            visible: questDescription.length > 0
        }
        
        // Bottom row: Due date and status
        RowLayout {
            Layout.fillWidth: true
            spacing: 10
            
            // Due date
            Row {
                spacing: 5
                visible: questDueDate.length > 0
                
                Text {
                    text: "📅"
                    font.pixelSize: 12
                }
                Text {
                    text: questDueDate
                    font.pixelSize: 12
                    color: inkBlack
                    opacity: 0.7
                }
            }
            
            Item { Layout.fillWidth: true }
            
            // Completion hint or stamp
            Text {
                text: questCompleted ? "" : "Hold to Complete"
                font.pixelSize: 11
                font.italic: true
                color: inkBlack
                opacity: 0.5
                visible: !questCompleted
            }
        }
    }
    
    // Wax seal / Completion stamp
    Rectangle {
        id: completionStamp
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.rightMargin: 15
        anchors.topMargin: 40
        width: 60
        height: 60
        radius: 30
        color: waxRed
        visible: questCompleted
        
        // Inner circle
        Rectangle {
            anchors.centerIn: parent
            width: 50
            height: 50
            radius: 25
            color: Qt.darker(waxRed, 1.2)
            
            Text {
                anchors.centerIn: parent
                text: "✓"
                font.pixelSize: 28
                font.bold: true
                color: goldColor
            }
        }
        
        // Entrance animation
        scale: questCompleted ? 1.0 : 0.0
        
        Behavior on scale {
            NumberAnimation {
                duration: 300
                easing.type: Easing.OutBack
            }
        }
    }
    
    // Interaction area
    MouseArea {
        anchors.fill: parent
        enabled: !questCompleted
        
        // Long press detection
        onPressAndHold: {
            questAccepted()
        }
        
        // Visual feedback
        onPressed: {
            cardBackground.scale = 0.98
        }
        
        onReleased: {
            cardBackground.scale = 1.0
        }
        
        onCanceled: {
            cardBackground.scale = 1.0
        }
    }
    
    // Scale animation
    Behavior on scale {
        NumberAnimation { duration: 100 }
    }
}
