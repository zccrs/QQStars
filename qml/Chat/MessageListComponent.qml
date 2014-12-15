import QtQuick 2.2
import mywindow 1.0
import QQItemInfo 1.0
import MyTextEditPlugin 1.0
import "../Utility"

Component{
    Item{
        id: root
        width: parent.width
        height: nick.implicitHeight+backgound.height+backgound.anchors.topMargin
        property FriendInfo myinfo: sender_info
        //这条消息的发送者的信息，好友发送的话就是你的好友本身，群里边就是此条群消息的发送者
        property QQItemInfo parentInfo: parent_info
        //消息属主的info，好友里面就是你的好友本身，群里边就是群本身
        property ChatMessageInfo messageInfo: message_info
        //这条消息自身的信息，千万不要和parentInfo和myinfo搞混了
        property string toUin: to_uin
        
        property var sendMessage: {
            if(!parentInfo)
                return null
            switch(parentInfo.mytype){
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
            //console.log(message)//输出消息内容
            if(sendMessage&&mode=="right"&&toUin!=""){//如果为模式right代表是要发送消息
                sendMessage(sendMessageFinished, toUin, messageInfo.contentData)//发送消息
            }
        }
        function sendMessageFinished(error, data){//如果这个Item发送信息，此函数用来接收发送结果
            //console.log(data)
            if(!error){//如果没有出错
                data = JSON.parse(data)
                if(data.retcode==0&&data.result=="ok"){
                    var date_time = new Date
                    messageInfo.date = date_time.getDate()
                    messageInfo.time = date_time.getTime()
                    parentInfo.addChatRecord(messageInfo)//将聊天记录保存到内存当中
                    console.debug("消息发送成功")
                }else{
                    console.log("发送失败")
                    mytext.append('<font color="red">[发送失败]</font>')
                    messageInfo.destroy()//销毁此对象
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
            height: 40
            sourceSize.width: width
            maskSource: "qrc:/images/bit.bmp"
            source: root.myinfo.avatar40
            onLoadError: {
                avatar.source = "qrc:/images/avatar.png"
            }
        }
        Text{
            id: nick
            x: mode=="left"?avatar.x+avatar.width+5:avatar.x-implicitWidth-5
            anchors.top: avatar.top
            text: root.myinfo.aliasOrNick
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
                wrapMode: TextEdit.Wrap
                text: root.messageInfo.contentData
                
                onWidthChanged: {
                    var temp = root.width-avatar.width-35
                    if(width>temp){
                        width = temp
                    }
                }
                onTextChanged: {
                    //console.debug(text)
                    if(text[text.length-1]=="\n"){
                        text = text.substr(0, text.length-1)
                    }
                }
                
                TextEditPlayGif{
                    target: mytext
                }
            }
        }
    }
}
