import QtQuick 2.2
import utility 1.0
import "../QQItemInfo"

ChatWindow{
    id: root
    myinfo: myqq.createFriendInfo(myuin)
   
    Connections{
        target: myqq
        onShakeWindow:{
            if(fromUin==myuin){
                console.log("窗口震动消息")
                myqq.shakeChatMainWindow(root)//抖动聊天窗口
            }
        }
        onFriendInputNotify:{
            if(fromUin==myuin){
                console.log("正在输入消息")
                show_text.text = myinfo.aliasOrNick+"正在输入"
            }
        }
    }
    
    Timer{
        id: timer_show_text
        interval: 2000
        onTriggered: {
            show_text.text = ""
        }
    }

    Text{
        id: show_text
        parent: menuBar
        anchors.right: parent.right
        anchors.rightMargin: 10
        verticalAlignment: Text.AlignVCenter
        anchors.verticalCenter: parent.verticalCenter
        onTextChanged: {
            if(text!=""){
                timer_show_text.start()
            }
        }
    }
    
    Image{
        parent: rightBar
        source: "qqshow.png"
    }
}
