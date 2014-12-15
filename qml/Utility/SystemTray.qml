import QtQuick 2.2
import mywindow 1.0
import utility 1.0
import QQItemInfo 1.0
import qqstars 1.0

MySystemTrayIcon{
    id: root
    property QQItemInfo currentInfo//当前最新消息的发送人的信息
    property bool hovered: false//鼠标是否悬浮在托盘上空
    property TrayMessageWindow trayMessageWindow
    signal triggered(var arg)
    

    visible: true
    windowIcon: "qrc:/images/avatar.png"
    menu: myqq.loginStatus == QQ.LoginFinished?menu2:menu1
    
    toolTip: {
        if( myqq.loginStatus == QQ.LoginFinished ){
            return "QQ:"+myqq.nick+"("+myqq.userQQ+")"
        }else
            return "星辰QQ"
    }
    
    function iconShakeStart(){//开启图标闪动
        windowIcon = currentInfo.avatar40//设置图标
        timer_shake.start()
        console.log("开启了头像闪动")
    }
    function iconShakeStop(){//停止鼠标闪动
        timer_shake.stop()
        windowIcon = "qrc:/images/avatar.png"
    }
    
    onCurrentInfoChanged: {
        if(timer_shake.running)
            windowIcon = currentInfo.avatar40//设置图标
    }
    Connections{
        target: timer_shake.running?currentInfo:null
        onAvatar40Changed:{
            windowIcon = currentInfo.avatar40//设置图标
        }
    }

    Component.onCompleted: {
        var component = Qt.createComponent("TrayMessageWindow.qml");
        if (component.status == Component.Ready){
            trayMessageWindow = component.createObject(null, {});
        }
    }
    
    onMessageClicked:{
        iconShakeStop()//停止闪动头像
        myqq.addChatPage(currentInfo.uin, currentInfo.mytype)//增加聊天页面
    }
    onActivated:{
        if( timer_shake.running&&arg == MySystemTrayIcon.Trigger ) {//如果头像正在闪动
            iconShakeStop()//停止闪动头像
            myqq.addChatPage(currentInfo.uin, currentInfo.mytype)//增加聊天页面
        }
    }
    Connections{
        target: trayMessageWindow
        onStopShakeIcon: {//如果清除全部消息提示
            iconShakeStop()//停止闪动头像
        }
        onSetCurrentInfo:{
            currentInfo = info
        }
    }

    Connections{
        target: timer_shake.running?utility:null
        onMouseDesktopPosChanged:{
            if(arg.x>=root.x&&arg.y>=root.y&&arg.x<=root.x+root.width&&arg.y<=root.y+root.height){
                if(!hovered){
                    hovered = true
                    //console.debug("进入了托盘区域")
                    root.toolTip = ""
                    trayMessageWindow.showWindow(root.x, root.y, root.width, root.height)//显示消息通知框
                }
            }else if(hovered){
                hovered = false
                trayMessageWindow.closeWindow()//隐藏消息通知栏
                root.toolTip = "QQ:"+myqq.nick+"("+myqq.userQQ+")"
            }
        }
    }

    Connections{
        target: myqq
        onNewMessage:{
            if(!myqq.isChatPageExist(fromUin, type)){//判断聊天页面是否存在，如果存在的话 不用提示新消息
                console.debug("收到了新的未读消息："+currentInfo)
                if(type==QQItemInfo.Friend){//如果是qq消息
                    currentInfo = myqq.createFriendInfo(fromUin)
                }else if(type==QQItemInfo.Group){//如果是群消息
                    currentInfo = myqq.createGroupInfo(fromUin)
                }else if(type==QQItemInfo.Discu){//如果是讨论组消息
                    currentInfo = myqq.createDiscuInfo(fromUin)
                }
                trayMessageWindow.appendModel(currentInfo)
                iconShakeStart()//开始闪动
            }
        }
    }
    
    Timer{
        id: timer_shake
        interval: 300
        repeat: true//开启重复闪动
        property string old_icon
        onTriggered: {
            if(root.windowIcon!=""){
                old_icon=root.windowIcon
                root.windowIcon=""
            }else{
                root.windowIcon=old_icon
            }
        }
    }

    MyMenu{
        id:menu1
        styleSource: "qrc:/style/menuStyle.css"
        MyMenuItem{
            text: "打开主面板"
            onTriggered: {
                root.triggered(text)
            }
        }
        MyMenuItem{
            text: "退出"
            onTriggered: Qt.quit()
        }
    }
    
    MyMenu{
        id: menu2

        styleSource: "qrc:/style/menuStyle.css"
        MyMenuItem{
            text: "我在线上"
            icon: "qrc:/images/imonline.png"
            onTriggered: {
                myqq.state = QQ.Online
            }
        }
        MyMenuItem{
            text: "Q我吧"
            icon: "qrc:/images/imcallme.png"
            onTriggered: {
                myqq.state = QQ.Callme
            }
        }
        MyMenuItem{
            text: "离开"
            icon: "qrc:/images/imaway.png"
            onTriggered: {
                myqq.state = QQ.Away
            }
        }
        MyMenuItem{
            text: "忙碌"
            icon: "qrc:/images/imbusy.png"
            onTriggered: {
                myqq.state = QQ.Busy
            }
        }
        MyMenuItem{
            text: "请勿打扰"
            icon: "qrc:/images/imsilent.png"
            onTriggered: {
                myqq.state = QQ.Silent
            }
        }
        MyMenuItem{
            text: "隐身"
            icon: "qrc:/images/imhidden.png"
            onTriggered: {
                myqq.state = QQ.Hidden
            }
        }
        MyMenuItem{
            text: "离线"
            icon: "qrc:/images/imoffline.png"
            onTriggered: {
                myqq.state = QQ.Offline
            }
        }
        MenuSeparator{}
        MyMenuItem{
            text: "关闭所有声音"
        }
        MyMenuItem{
            text: "关闭头像闪动"
        }
        MenuSeparator{}
        MyMenuItem{
            text: lock_qq?"解锁QQ":"锁定QQ"
            shortcut: "Ctrl+Alt+L"
            property bool lock_qq: false
            icon: lock_qq?"qrc:/images/unlock20.png":"qrc:/images/lock20.png"
            onTriggered: {
                lock_qq = !lock_qq
            }
        }
        MenuSeparator{}
        MyMenuItem{
            text: "打开主面板"
            onTriggered: {
                root.triggered(text)
            }
        }
        MenuSeparator{}
        MyMenuItem{
            text: "&退出"
            onTriggered: {
                Qt.quit()
            }
        }
    }
}
