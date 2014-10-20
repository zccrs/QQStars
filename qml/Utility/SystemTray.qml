import QtQuick 2.2
import mywindow 1.0
import utility 1.0
import QQItemInfo 1.0

MySystemTrayIcon{
    id: root
    visible: true
    windowIcon: "qrc:/images/avatar.png"
    property var currentInfo
    
    onWindowIconChanged: {
        if(windowIcon!="qrc:/images/avatar.png"){
            timer_shake.start()
        }else{
            timer_shake.stop()
        }
    }

    Timer{
        id: timer_shake
        interval: 300
        repeat: true//开启重复闪动
        property string old_icon: "qrc:/images/avatar.png"
        onTriggered: {
            if(root.windowIcon!=""){
                old_icon=root.windowIcon
                root.windowIcon=""
            }else{
                root.windowIcon=old_icon
            }
        }
    }

    onMessageClicked:{
        windowIcon="qrc:/images/avatar.png"
        myqq.addChatPage(currentInfo.uin, currentInfo.mytype)//增加聊天页面
    }
    onActivated:{
        if( timer_shake.running&&arg == MySystemTrayIcon.Trigger ) {
            windowIcon="qrc:/images/avatar.png"
            myqq.addChatPage(currentInfo.uin, currentInfo.mytype)//增加聊天页面
        }
    }
    toolTip: {
        if( myqq.userData ){
            return "QQ:"+myqq.nick+"("+myqq.userQQ+")"
        }else
            return "星辰QQ"
    }
    signal triggered(var arg)
    menu: myqq.loginStatus == QQ.LoginFinished?menu2:menu1
    
    Connections{
        target: myqq
        onNewMessage:{
            if(!myqq.isChatPageExist(fromUin, type)){//判断聊天页面是否存在
                if(type==QQItemInfo.Friend){//如果是qq消息
                    currentInfo = myqq.createFriendInfo(fromUin)
                    root.showMessage("来自："+currentInfo.aliasOrNick+"的消息", info.contentData)
                    root.windowIcon = currentInfo.avatar40
                }
            }
        }
    }

    MyMenu{
        id:menu1
        styleSource: "../../style/menuStyle.css"
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
        styleSource: "../../style/menuStyle.css"
        MyMenuItem{
            text: "我在线上"
            icon: "qrc:/images/imonline.png"
            onTriggered: {
                myqq.userStatus = QQ.Online
            }
        }
        MyMenuItem{
            text: "Q我吧"
            icon: "qrc:/images/Qme.png"
            onTriggered: {
                myqq.userStatus = QQ.Callme
            }
        }
        MyMenuItem{
            text: "离开"
            icon: "qrc:/images/away.png"
            onTriggered: {
                myqq.userStatus = QQ.Away
            }
        }
        MyMenuItem{
            text: "忙碌"
            icon: "qrc:/images/busy.png"
            onTriggered: {
                myqq.userStatus = QQ.Busy
            }
        }
        MyMenuItem{
            text: "请勿打扰"
            icon: "qrc:/images/mute.png"
            onTriggered: {
                myqq.userStatus = QQ.Silent
            }
        }
        MyMenuItem{
            text: "隐身"
            icon: "qrc:/images/invisible.png"
            onTriggered: {
                myqq.userStatus = QQ.Hidden
            }
        }
        MyMenuItem{
            text: "离线"
            icon: "qrc:/images/imoffline.png"
            onTriggered: {
                myqq.userStatus = QQ.Offlineing
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
