import QtQuick 2.2
import mywindow 1.0
import utility 1.0
import "../"

MyWindow{
    id: root
    minHeight: 500
    minWidth: 600
    visible: true
    noBorder: true//无边框的
    removable: true//可移动的
    fixedSize: false//固定大小的
    dockableWindow: false//可停靠的
    topHint: true//窗口保持在最前端
    noNotifyIcon:false//隐藏任务栏图标
    color: "transparent"
    
    property QtObject control: null
    property int mynumber
    property string myuin: ""
    property var type
    property var closeFun//窗口关闭的时候调用此函数
    
    Connections{
        target: control
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
                        message+="此处为图片 "
                    }else if(type == QQ.ShakeWindow){
                        console.log("窗口震动消息")
                        message+="窗口抖动消息"
                    }else if(type == QQ.InputNotify){
                        console.log("正在输入消息")
                        show_text.text = myqq.getValue(uin+"nick", uin)+"正在输入"
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
                console.log(message)
                var data = {"uin":uin, "mode": "right", "message": message}
                mymodel.append(data)
            }
        }
    }
    

    Rectangle{
        anchors.fill: parent
        color: "#eee"
        border.width: 1
        border.color: "#f07000"
        radius: 10
        Item{
            id: menu_bar
            height: 30
            anchors.left: image_quit_icon.right
            anchors.leftMargin: 10
            anchors.right: parent.right
            anchors.top: parent.top
            Timer{
                id: timer_show_text
                interval: 2000
                onTriggered: {
                    show_text.text = ""
                }
            }

            Text{
                id: show_text
                verticalAlignment: Text.AlignVCenter
                anchors.verticalCenter: parent.verticalCenter
                onTextChanged: {
                    if(text!=""){
                        timer_show_text.start()
                    }
                }
            }
        }
        
        SvgView{
            id:image_quit_icon
            width: defaultSize.width*myqq.windowScale
            source: "qrc:/images/button-quit.svg"
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.margins: 10
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    root.close()
                    if(root.closeFun)
                        root.closeFun(root.mynumber)
                    root.destroy()
                }
            }
        }
        Item{
            id: item_qqshow
            anchors.top: menu_bar.bottom
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            width: 150
            Image{
                source: "qqshow.png"
            }
        }
        
        MyTextArea{
            id: input
            //font.pointSize: 12
            wrapMode: TextEdit.Wrap
            anchors.bottom: button_send.top
            anchors.left: parent.left
            anchors.right: item_qqshow.left
            anchors.margins: 10
            height: 100
            onActiveFocusChanged: {
                if(activeFocus){
                    
                }
            }
        }
        MyButton{
            id: button_send
            anchors.right: item_qqshow.left
            anchors.bottom: parent.bottom
            anchors.margins: 10
            text: "发送"
            function sendMessageFinished(error, data){
                console.log(data)
            }

            Keys.onEnterPressed: {
                button_send.clicked()
            }
            Keys.onReturnPressed: {
                button_send.clicked()
            }
            onClicked: {
                myqq.sendMessage(sendMessageFinished, myuin, input.text)//发送消息
                var data = {"uin":myqq.userQQ, "mode": "left", "message": input.text}
                mymodel.append(data)
                input.text = ""
            }
        }
        

        Rectangle{
            anchors.top: menu_bar.bottom
            anchors.right: item_qqshow.left
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
                    width: scroll_list.width
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
    Keys.onEnterPressed: {
        button_send.clicked()
    }
    Keys.onReturnPressed: {
        button_send.clicked()
    }
}
