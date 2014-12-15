import QtQuick 2.2
import QtQuick.Window 2.0
import mywindow 1.0
//import "../QQItemInfo"

Window{
    id: root
    flags: Qt.SplashScreen|Qt.WindowStaysOnTopHint
    height: list.height+user_nick.implicitHeight+item_button.height+30
    width: 200
    color: "transparent"
    signal stopShakeIcon//停止对托盘图标的闪动
    signal setCurrentInfo(var info)
    property var infosQueue: new Array
    
    function appendModel(senderInfo){
        for(var i in infosQueue){
            if(infosQueue[i] == senderInfo){//如果对象已经存在
                return
            }
        }
        mymodel.append({"sender_info": senderInfo})
        infosQueue.push(senderInfo)//加到队列里边
    }
    function removeModel(index){
        if(index>=0){
            mymodel.remove(index)
            infosQueue.splice(index, 1)
            //console.debug(mymodel.count+","+infosQueue.length)
            if(mymodel.count==0)
                stopShakeIcon()//停止闪动
        }
    }
    function clearModel(){
        stopShakeIcon()//发送信号
        mymodel.clear()
        infosQueue.length=0
    }
    function getLatestInfo(){
        if(infosQueue.length>0)
            return infosQueue[infosQueue.length-1]
        else
            return null
    }
    function showWindow(trayX, trayY, trayWidth, trayHeight){
        timer_close.stop()//先停止动画
        root.opacity = 1
        root.show()
        var tempx = trayX-width/2+trayWidth/2
        if(tempx<0)
            x=trayX+trayWidth
        else if(tempx+width>Screen.desktopAvailableWidth)
            x=trayX-width
        else
            x=tempx
        var tempy = trayY-height/2+trayHeight/2
        if(tempy<0)
            y=trayY+trayHeight
        else if(tempy+height>Screen.desktopAvailableHeight)
            y=trayY-height
        else
            y=tempy
    }
    function closeWindow(){
        if(!mouse_area.hovered)//如果当前鼠标没有在自己区域
            timer_close.start()//启动动画定时器
    }
    NumberAnimation{
        id: timer_close
        target: root
        property: "opacity"
        running: false
        duration: 800
        from: 1
        to: 0
        onStopped: {//当动画结束后
            if(root.opacity==0){
                root.close()
            }
        }
    }

    MouseArea{
        id: mouse_area
        anchors.fill: parent
        property bool hovered: false
        hoverEnabled: true
        onEntered: {
            hovered = true
            timer_close.stop()//停止关闭窗口的动画
            root.opacity = 1
            //console.debug("进入了区域")
        }
        onExited: {
            var globalX = utility.mouseDesktopPos().x
            var globalY = utility.mouseDesktopPos().y
            //console.log("x:"+root.x+","+width+","+globalX)
            //console.log("y:"+root.y+","+height+","+globalY)
            if(globalX<root.x||globalX>root.x+root.width||globalY<root.y||globalY>root.y+root.height){
                hovered = false
                root.closeWindow()
                //console.debug("离开了区域")
            }
        }
    }
    Rectangle{
        width: parent.width
        height: list.height+user_nick.implicitHeight+item_button.height+30
        border.width: 1
        border.color: "#f07000"
        color: "white"
        radius: 10
        Text{
            id: user_nick
            text: myqq.nick
            x:10
            y:10
        }

        ListView{
            id:list
            delegate: list_delegate
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: user_nick.bottom
            height: mymodel.count*40
            
            clip: true
            anchors.margins: 10
            signal openAllChatPage//当点击"查看全部"时被发射
            model: ListModel{
                id:mymodel
            }
        }
        
        Item{
            id: item_button
            anchors.top: list.bottom
            anchors.topMargin: 10
            anchors.left: list.left
            anchors.right: list.right
            height: 30
            Rectangle{
                width: parent.width
                height: 1
                color: "#f07000"
            }

            Text{
                text: "忽略全部"
                color: "#f07000"
                font.underline: true
                anchors.verticalCenter: parent.verticalCenter
                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        clearModel()//先清除所有数据
                        root.close()//然后关闭窗口
                    }
                }
            }
            Text{
                text: "查看全部"
                color: "#f07000"
                font.underline: true
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        list.openAllChatPage()//发送信号
                        clearModel()//先清除所有数据
                        root.close()//然后关闭窗口
                    }
                }
            }
        }
    }
    Component{
        id: list_delegate
        Item{
            id: myitem
            width: parent.width
            height: 40
            property var myinfo: sender_info
            function removeMe(){
                root.close()//关闭窗口
                removeModel(index)//清除自己
                var temp_info = getLatestInfo()
                if(temp_info!=null){
                    setCurrentInfo(temp_info)
                }
            }

            Connections{
                target: list
                onOpenAllChatPage: {
                    myqq.addChatPage(myinfo.uin, myinfo.mytype)
                }
            }
            
            Rectangle{
                anchors.fill: parent
                color: "#aaa"
                opacity: 0.8
                visible: item_mouse.hovered
            }

            MyImage{
                id: avatar
                x:10
                width:parent.height-10
                height: width
                sourceSize.width: width
                anchors.verticalCenter: parent.verticalCenter
                maskSource: "qrc:/images/bit.bmp"
                cache: false
                source: myinfo.avatar40
            }
            Text{
                id: nick
                anchors.left: avatar.right
                anchors.leftMargin: 10
                anchors.top: avatar.top
                text: myinfo.aliasOrNick
            }
            Rectangle{
                width: text_message_count.implicitWidth+10
                height: text_message_count.implicitHeight+10
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                anchors.rightMargin: 5
                color: "red"
                opacity: 0.75
                radius: height
                Text{
                    id: text_message_count
                    anchors.centerIn: parent
                    text: myinfo.unreadMessagesCount//未读消息的个数
                    color: "white"
                    onTextChanged: {
                        if(text == "100"){
                            text = "99+"
                        }else if(text == "0"){//如果未读消息个数变为0
                            myitem.removeMe()//移除自己
                        }
                    }
                }
            }

            MouseArea{
                id: item_mouse
                anchors.fill: parent
                property bool hovered: false
                hoverEnabled: true
                onEntered: {
                    hovered = true
                }
                onExited: {
                    hovered = false
                }
                onClicked: {
                    myqq.addChatPage(myinfo.uin, myinfo.mytype)//点击了之后开始聊天
                    myitem.removeMe()//移除自己
                }
            }
        }
    }
}
