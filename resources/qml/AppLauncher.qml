import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

// 这是一个“启动器”QML：它尝试加载 MainView.qml（或其它主页面），
// 如果加载失败会显示错误信息。这样即使某些子模块缺失，窗口也能弹起并报告问题。

ApplicationWindow {
    id: root
    width: 450
    height: 850
    visible: true
    title: "SmartTask RPG - GUI"

    // 状态栏
    RowLayout {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 36
        Rectangle { color: "#1e1e1e"; anchors.fill: parent }
        Text { text: "SmartTask RPG"; color: "white"; verticalAlignment: Text.AlignVCenter; anchors.left: parent.left; anchors.leftMargin: 10 }
    }

    Loader {
        id: mainLoader
        anchors.top: parent.top
        anchors.topMargin: 36
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        // 尝试从资源加载主视图（qrc:/qml/MainView.qml），
        // 若你未将 QML 打包到 qrc，会优先在 exe 目录寻找 resources/qml/MainView.qml（由 C++ 控制）
        source: "qrc:/qml/MainView.qml"
        asynchronous: true

        onStatusChanged: {
            if (status === Loader.Ready) {
                // loaded
            } else if (status === Loader.Error) {
                console.error("Failed to load MainView.qml: " + mainLoader.item ? mainLoader.item : mainLoader.errorString());
            }
        }
    }

    // 如果 loader 加载出错或为空，显示友好提示
    Rectangle {
        anchors.fill: parent
        color: "transparent"
        visible: mainLoader.status === Loader.Error || mainLoader.status === Loader.Null
        Column {
            anchors.centerIn: parent
            spacing: 12
            Text { text: "无法加载主界面 (MainView.qml)"; color: "white"; font.bold: true; font.pixelSize: 18; horizontalAlignment: Text.AlignHCenter }
            Text {
                text: "请确保 resources/qml/MainView.qml 可用，或在构建时把 QML 打包到 qrc。"
                color: "#ccc"
                wrapMode: Text.WrapAnywhere
                horizontalAlignment: Text.AlignHCenter
                width: parent.width * 0.8
            }
            Button {
                text: "打开日志目录"
                onClicked: {
                    // 尝试打开 exe 目录，供用户查看日志与 qml 文件（平台支持可能不同）
                    Qt.openUrlExternally("file://" + Qt.resolvedUrl("."));
                }
            }
        }
    }
}