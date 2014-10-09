import QtQuick 2.2
import mywindow 1.0
import QtQuick.Window 2.1
import "../Utility/Window"
import "../"
import "../Utility"

MyWindow{
    id: root
    minHeight: 500
    minWidth: 600
    visible: true
    noBorder: true//无边框的
    removable: true//可移动的
    fixedSize: false//固定大小的
    dockableWindow: false//可停靠的
    topHint: false//窗口保持在最前端
    noNotifyIcon:false//隐藏任务栏图标
    color: "transparent"
    windowGlowItem.color: "black"//"#f07000"
    
    property QtObject control: null
    property int mynumber
    property string myuin
    property var type
    property var closeFun//窗口关闭的时候调用此函数
    signal sendClicked//点击发送按钮好调用此函数
    property alias menuBar: menu_bar
    property alias rightBar: right_bar
    property alias inputBox: input
    property alias listModel: mymodel
    signal newMessage(var uin, var messageData)
    Connections{
        target: control
        onNewMessage:{
            root.newMessage(uin, messageData)
        }
    }
    Component.onCompleted: {
        root.x = Screen.desktopAvailableWidth/2 - root.width/2//让程序居中显示
        root.y = Screen.desktopAvailableHeight/2 - root.height/2
    }
    
    shortcuts: MyShortcutList{
        MyShortcut{
            shortcut: "Ctrl+Return"
            onTrigger: {
                button_send.clicked()
            }
        }
        MyShortcut{
            shortcut: "Ctrl+Enter"
            onTrigger: {
                button_send.clicked()
            }
        }
    }

    Rectangle{
        anchors.fill: parent
        color: "#eee"
        radius: 10
        Item{
            id: menu_bar
            height: 30
            anchors.left: image_minimize_icon.right
            anchors.leftMargin: 10
            anchors.right: parent.right
            anchors.top: parent.top
        }
        
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
        SvgView{
            id:image_minimize_icon
            width: defaultSize.width*myqq.windowScale
            source: "qrc:/images/button-minimize.svg"
            anchors.top: image_quit_icon.top
            anchors.left: image_quit_icon.right
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    root.showMinimized()
                }
            }
        }
        
        Item{
            id: right_bar
            anchors.top: menu_bar.bottom
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            width: 150
        }
        
        MyTextArea{
            id: input
            //font.pointSize: 12
            wrapMode: TextEdit.Wrap
            anchors.bottom: button_send.top
            anchors.left: parent.left
            anchors.right: right_bar.left
            anchors.margins: 10
            height: 100
        }
        MyButton{
            id: button_send
            anchors.right: right_bar.left
            anchors.bottom: parent.bottom
            anchors.margins: 10
            text: "发送"

            onClicked: {
                sendClicked()//发射信号
            }
        }
        

        Rectangle{
            anchors.top: menu_bar.bottom
            anchors.right: right_bar.left
            anchors.bottom: input.top
            anchors.left: parent.left
            anchors.margins: 10
            radius: 5
            color: "#f5f5f5"
            border.width: 2
            border.color: "#ddd"
            
            MyScrollView{
                id: scroll_list
                anchors.fill: parent
                anchors.margins: 5
                Item{
                    height: list.contentHeight+10
                    width: scroll_list.width-25
                    x:5
                    implicitHeight: height
                    implicitWidth: width
                   
                    ListView{
                        id: list
                        anchors.fill: parent
                        spacing: 10
                        model: ListModel{
                            id: mymodel
                        }
                        delegate: MessageListComponent{}
                    }
                }
            }
        }
    }
}
