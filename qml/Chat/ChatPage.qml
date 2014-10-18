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
    property var myinfo//用来储存自己的各种信息（用uin标识）
    signal sendClicked//点击发送按钮好调用此函数
    property alias menuBar: menu_bar
    property alias rightBar: right_bar
    property alias inputBox: input
    property alias listModel: mymodel
    
    Connections{
        target: myqq
        onNewMessage:{
            if(fromUin==myuin){
                var message_info
                if(myinfo){
                    message_info = myinfo.addChatRecords(senderUin, message)//将聊天记录保存到本地
                }

                var data = {
                    "uin":senderUin,//发送者是谁
                    "send_uin":"",//如果mode为right才需要此值(为发送给谁)
                    "mytype": myinfo.mytype,
                    "mode": "left",
                    "message": message,//消息内容
                    "message_info": message_info
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
                var message_info
                if(myinfo){//如果myinfo不为未定义或null
                    message_info = myinfo.addChatRecords(myqq.userQQ, inputBox.text)//将聊天记录保存到本地
                }
                var data = {
                    "uin":myqq.userQQ,//发送者是当前登录的用户qq
                    "send_uin":myuin,//发送给自己（这里的自己代表对当前登录的用户qq来说是他的好友或者群，讨论组）
                    "mytype": myinfo.mytype,
                    "mode": "right",
                    "message": inputBox.selectedText,//选中的文本
                    "message_info": message_info
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
