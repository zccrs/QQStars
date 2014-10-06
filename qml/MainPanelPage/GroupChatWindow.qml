import QtQuick 2.2
import utility 1.0

ChatWindow{
    id: root
    windowIcon: myqq.getValue(myuin+"avatar-40", "qrc:/images/avatar.png")
    
    function sendMessageFinished(error, data){
        console.log(data)
    }
    onSendClicked: {
        myqq.sendGroupMessage(sendMessageFinished, myuin, inputBox.text)//发送消息
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
                }else if(type == QQ.Face){
                    console.log("表情消息："+content.face_code)
                    message+="表情("+content.face_code+")"
                }
            }
            var data = {"uin":temp.send_uin, "mode": "left", "message": message}
            listModel.append(data)
        }
    }

    Text{
        id: show_text
        parent: menuBar
        verticalAlignment: Text.AlignVCenter
        anchors.verticalCenter: parent.verticalCenter
        text: myqq.getValue(myuin+"nick", myuin)
    }
}
