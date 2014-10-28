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
    signal sendClicked//点击发送按钮好调用此函数
    property alias menuBar: menu_bar
    property alias rightBar: right_bar
    property alias inputBox: input
    property alias listModel: mymodel
    property QQItemInfo myinfo//用来储存自己的各种信息（用uin标识）
    onMyinfoChanged: {
        if(myinfo){//从缓冲区中读取数据
            var message_list = myinfo.getChatRecords()//获取内存中的所有聊天记录
            for(var i=0;i<message_list.length;++i){
                var message = message_list[i]//读取第i条消息
                var data = {
                    "uin":message.senderUin,//发送者是谁
                    "send_uin":"",//如果mode为right才需要此值(为发送给谁)
                    "mode": message.senderUin==myqq.userQQ?"right":"left",//要判断发送者是不是自己（这条消息是发送的还是接收的）
                    "message": message.contentData,//消息内容
                    "parent_info": myinfo
                }
                listModel.append(data)
            }
            scroll_list.contentAtEnd()//将内容拉到最后
        }
    }

    Connections{
        target: myqq//记录自己各种信息的类对象
        onNewMessage:{
            if(fromUin==myuin&&type == myinfo.mytype){
                var data = {
                    "uin":info.senderUin,//发送者是谁
                    "send_uin":"",//如果mode为right才需要此值(为发送给谁)
                    "mode": "left",//要判断发送者是不是自己（这条消息是发送的还是接收的）
                    "message": info.contentData,//消息内容
                    "parent_info": myinfo
                }
                var temp = scroll_list.isContentEnd()//记录是否应该讲聊天页面拉到最后
                listModel.append(data)
                if(temp){
                    scroll_list.contentAtEnd()//将内容拉到最后
                }
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
                inputBox.selectAll()//先选中全部
                var data = {
                    "uin":myqq.userQQ,//发送者是当前登录的用户qq
                    "send_uin":myuin,//发送给自己（这里的自己代表对当前登录的用户qq来说是他的好友或者群，讨论组）
                    "mode": "right",
                    "message": inputBox.selectedText,//选中的文本
                    "parent_info": myinfo
                }
                listModel.append(data)
                inputBox.text = ""
                scroll_list.contentAtEnd()//将内容放到最后
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
                function contentAtEnd(){
                    var flickableItem = scroll_list.flickableItem
                    flickableItem.contentY = flickableItem.contentHeight-scroll_list.height+60
                    //console.debug(flickableItem.contentY+","+(flickableItem.contentHeight-scroll_list.height+60))
                }
                function isContentEnd(){
                    var flickableItem = scroll_list.flickableItem
                    //console.debug(flickableItem.contentY+","+(flickableItem.contentHeight-scroll_list.height-5))
                    return flickableItem.contentY > flickableItem.contentHeight-scroll_list.height-5
                }

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
                        property int old_contentHeight: 0
                        
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
