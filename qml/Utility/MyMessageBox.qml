import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

MyWindow {
    id:root
    visible: true//可视的
    noBorder: true//无边框的
    removable: true//可移动的
    fixedSize: true//固定大小的
    dockableWindow: false//可停靠的
    topHint: true//窗口保持在最前端
    noNotifyIcon: true//隐藏任务栏图标
    modality : Qt.ApplicationModal
    color: "transparent"
    width: 360
    height: 200
    property string text
    Component.onCompleted: {
        button_ok.forceActiveFocus()
    }
    Rectangle{
        anchors.fill: parent
        gradient: Gradient {
                        GradientStop { position: 0 ; color:  "#EEEDEC" }
                        GradientStop { position: 1 ; color:  "#f9f9f8" }
                    }
        radius: 10
        Text{
            id: text
            text: root.text
            anchors.centerIn: parent
            wrapMode :Text.WordWrap//自动换行
            color: "#f47421"
            width: parent.width-20
            anchors.bottom: button_ok.top
            anchors.top: parent.top
            anchors.margins: 10
        }
    
        MyButton{
            id: button_ok
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: 10
            width: 60
            height: 25
            text: "确认"
            Keys.onEnterPressed: {
                button_ok.clicked()
            }
            Keys.onReturnPressed: {
                button_ok.clicked()
            }
            onClicked: {
                root.close()
            }
        }
    }
}
