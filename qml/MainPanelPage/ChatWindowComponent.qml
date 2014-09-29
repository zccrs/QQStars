import QtQuick 2.2
import mywindow 1.0
import "../"

MyWindow{
    id: root
    minHeight: 500
    minWidth: 500
    visible: true
    noBorder: true//无边框的
    removable: true//可移动的
    fixedSize: false//固定大小的
    dockableWindow: false//可停靠的
    topHint: true//窗口保持在最前端
    noNotifyIcon:false//隐藏任务栏图标
    color: "transparent"
    
    property QtObject control: null
    property int mynumber
    property string myuin: ""
    property var type
    property var closeFun//窗口关闭的时候调用此函数
    Component.onCompleted: {
        console.log(myuin+","+control+","+type)
    }
    Connections{
        target: control
        onNewMessage:{
            if(uin==myuin){
                console.log(messageData)
            }
        }
    }
    Rectangle{
        anchors.fill: parent
        color: "#eee"
        border.width: 1
        border.color: "#f07000"
        radius: 10
        SvgView{
            id:image_quit_icon
            width: defaultSize.width*myqq.windowScale
            source: "qrc:/images/button-quit.svg"
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.margins: 10
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    root.close()
                    if(root.closeFun)
                        root.closeFun(root.mynumber)
                    root.destroy()
                }
            }
        }
        MyTextArea{
            font.pointSize: 15
            wrapMode: TextEdit.Wrap
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: 5
            height: 100
        }
    }
}
