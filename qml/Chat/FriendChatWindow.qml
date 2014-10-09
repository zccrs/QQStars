import QtQuick 2.2
import utility 1.0
import "../QQItemInfo"

ChatWindow{
    id: root
    windowIcon: myinfo.avatar40//myqq.value(myuin+"avatar-40", "qrc:/images/avatar.png")
    FriendInfo{
        id: myinfo
        userQQ: myqq.userQQ
        uin: myuin
    }

    function sendMessageFinished(error, data){
        console.log(data)
    }
    onSendClicked: {
        myqq.sendMessage(sendMessageFinished, myuin, inputBox.text)//发送消息
        var data = {"uin":myqq.userQQ, "mode": "right", "message": inputBox.text}
        listModel.append(data)
        inputBox.text = ""
    }
    onNewMessage:{
        if(uin==myuin){
            var temp = JSON.parse(messageData)
            var message=""
            for(var i=0; i<temp.content.length; ++i){
                var content = temp.content[i]
                var type = content.type
                if(type == QQ.Text){
                    console.log(content.text)
                    message+=content.text+" "
                }else if(type == QQ.Image){
                    console.log("图片消息")
                    message+="此处为图片"
                }else if(type == QQ.ShakeWindow){
                    console.log("窗口震动消息")
                    root.windowShake()
                    message+="窗口震动消息 "
                }else if(type == QQ.InputNotify){
                    console.log("正在输入消息")
                    show_text.text = myinfo.aliasOrNick+"正在输入"
                    return
                }else if(type == QQ.Face){
                    console.log("表情消息："+content.face_code)
                    message+="表情("+content.face_code+")"
                }else if(type == QQ.FileMessage){
                    if( content.flag==1 ){
                        console.log("请求发送文件："+content.name)
                        message+="请求发送文件："+content.name
                    }else{
                        console.log("取消发送文件")
                        message+="取消发送文件"
                    }
                }else if(type == QQ.AvRequest){
                    console.log("请求开视频")
                    message+="请求视频通话"
                }else if(type == QQ.AvRefuse){
                    console.log("取消视频通话")
                    message+="取消视频通话"
                }
            }
            //console.log(message)
            var data = {"uin":uin, "mode": "left", "message": message}
            listModel.append(data)
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
