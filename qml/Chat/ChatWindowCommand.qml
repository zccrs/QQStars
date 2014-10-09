import QtQuick 2.2
import utility 1.0

Item{
    visible: false
    QtObject{
        id: chat_window
        signal newMessage(var uin, var messageData)
        property var chatWindow: new Array
        
        function removeObject(number){
            console.log("销毁"+number)
            chatWindow.slice(number,1)
        }
    }
    function addChat( uin , senderType){
        console.log("新建聊天")
        for(var i=0;i<chat_window.chatWindow.length;++i){
            if(chat_window.chatWindow[i]){
                if( chat_window.chatWindow[i].myuin==uin ){
                    console.log("窗口已经存在")
                    return//如果聊天窗口已经存在就return
                }
            }
        }
        var str = senderType==QQ.Friend?"FriendChatWindow.qml":(senderType==QQ.Group?"GroupChatWindow.qml":"")
        
        console.log(str)
        var component = Qt.createComponent(str);
        if (component.status == Component.Ready){
            console.log("将要创建窗口")
            var data = {"myuin": uin, "control":chat_window, "type":senderType, "closeFun": chat_window.removeObject};
            var sprite = component.createObject(null, data);
            sprite.mynumber = chat_window.chatWindow.push(sprite)
        }
    }


    function addMessage( uin, messageData ){
        chat_window.newMessage(uin, messageData)
    }
}
