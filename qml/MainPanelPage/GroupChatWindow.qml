import QtQuick 2.2
import utility 1.0

ChatWindow{
    id: root
    function sendMessageFinished(error, data){
        console.log(data)
    }
    onSendClicked: {
        var data = {"uin":myqq.userQQ, "mode": "right", "message": inputBox.text}
        listModel.append(data)
        inputBox.text = ""
        myqq.sendGroupMessage(sendMessageFinished, myuin, inputBox.text)//发送消息
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
            var data = {"uin":uin, "mode": "left", "message": message}
            listModel.append(data)
        }
    }

    Text{
        id: show_text
        parent: menuBar
        verticalAlignment: Text.AlignVCenter
        anchors.verticalCenter: parent.verticalCenter
        text: myqq.setValue(myuin+"nick", myuin)
    }
}
