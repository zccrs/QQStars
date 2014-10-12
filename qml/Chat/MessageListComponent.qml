import QtQuick 2.2
import mywindow 1.0
import QQItemInfo 1.0

Component{
    Item{
        id: root
        width: parent.width
        height: nick.implicitHeight+backgound.height+backgound.anchors.topMargin
        property var myinfo: myqq.createFriendInfo(uin)
        property string sendUin: send_uin
        property var sendMessage: {
            if(sendUin=="")
                return
            switch(mytype){
                case QQItemInfo.Friend:
                    return myqq.sendFriendMessage
                case QQItemInfo.Group:
                    return myqq.sendGroupMessage
                case QQItemInfo.Discu:
                    return myqq.sendDiscuMessage
                default:{
                    return
                }
            }
        }

        Component.onCompleted: {
            if(send_uin!=""&&sendMessage){//如果为right代表是要发送消息
                sendMessage(sendMessageFinished, send_uin, message)//发送消息
            }
        }
        function sendMessageFinished(error, data){//如果这个Item发送信息，此函数用来接收发送结果
            //console.log(data)
            if(!error){//如果没有出错
                data = JSON.parse(data)
                if(data.retcode==0&&data.result=="ok"){
                    console.log("发送成功")
                }else{
                    console.log("发送失败")
                    mytext.text+=":发送失败"
                }
            }else{
                console.log("发送失败")
                mytext.text+=":发送失败"
            }
        }
        MyImage{
            id: avatar
            x:mode=="left"?0:root.width-width
            width:40
            maskSource: "qrc:/images/bit.bmp"
            source: myinfo.avatar40
            onLoadError: {
                avatar.source = "qrc:/images/avatar.png"
            }
        }
        Text{
            id: nick
            x: mode=="left"?avatar.x+avatar.width+5:avatar.x-implicitWidth-5
            anchors.top: avatar.top
            text: myqq.aliasOrNick
        }

        BorderImage {
            id: backgound
            x: mode=="left"?avatar.x+avatar.width+5:avatar.x-width-5
            anchors.top: nick.bottom
            anchors.topMargin: 5
            source: "qrc:/images/bubble_放飞心情_"+mode+".png"
            height: mytext.height+20
            width: mytext.width+30
            border.left: 20; border.top: 20
            border.right: 20; border.bottom: 20

            TextEdit{
                id: mytext
                anchors.centerIn: parent
                readOnly: true
                textFormat :TextEdit.RichText//支持富文本
                selectByMouse :true
                selectByKeyboard :true
                onWidthChanged: {
                    var temp = root.width-avatar.width-35
                    if(width>temp){
                        width = temp
                    }
                }

                wrapMode: TextEdit.Wrap
                text: message
                onTextChanged: {
                    if(text[text.length-1]=="\n"){
                        text = text.substr(0, text.length-1)
                    }
                }
            }
        }
    }
}
