import QtQuick 2.2
import utility 1.0
import mywindow 1.0
import QQItemInfo 1.0
import "../"
import "../Utility"

Item{
    id: root
    anchors.fill: parent
    property string myuin
    property int type
    property var myinfo
    signal sendClicked//点击发送按钮好调用此函数
    property alias menuBar: menu_bar
    property alias rightBar: right_bar
    property alias inputBox: input
    property alias listModel: mymodel
    
    Connections{
        target: myqq
        onNewMessage:{
            if(fromUin==myuin){
                var data = {
                    "uin":fromUin,//从哪发过来的
                    "send_uin":senderUin,//发送者是谁
                    "mytype": myinfo.mytype,
                    "mode": "left",
                    "message": message//消息内容
                }
                listModel.append(data)
            }
        }
    }
    
    MyShortcut{
        target: input
        shortcut: "Ctrl+Return"
        onTrigger: {
            input.insert(input.cursorPosition, "<br>")
        }
    }
    MyShortcut{
        target: input
        shortcut: "Ctrl+Enter"
        onTrigger: {
            input.insert(input.cursorPosition, "<br>")
        }
    }
    MyShortcut{
        target: input
        shortcut: "Return"
        onTrigger: {
            button_send.clicked()
        }
    }
    MyShortcut{
        target: input
        shortcut: "Enter"
        onTrigger: {
            button_send.clicked()
        }
    }

    Rectangle{
        anchors.fill: parent
        color: "#eee"
        radius: 10
        Item{
            id: menu_bar
            height: 30
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
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
            //wrapMode: TextEdit.Wrap
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
                //sendClicked()//发射信号
                inputBox.selectAll()//先选中全部
                var data = {
                    "uin":myqq.userQQ,
                    "send_uin":myuin,
                    "mytype": myinfo.mytype,
                    "mode": "right",
                    "message": inputBox.selectedText//选中的文本
                }
                listModel.append(data)
                inputBox.text = ""
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
