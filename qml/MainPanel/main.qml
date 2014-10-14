import QtQuick 2.2
import mywindow 1.0
import QtQuick.Window 2.1
import utility 1.0
import "../Utility"
import "../Chat"
import Qt.labs.settings 1.0

MyWindow{
    id:main
    visible: true//可视的
    noBorder: true//无边框的
    removable: true//可移动的
    fixedSize: false//固定大小的
    dockableWindow: true//可停靠的
    topHint: true//窗口保持在最前端
    noNotifyIcon: true//隐藏任务栏图标
    color: "transparent"
    centered: false
    minimumWidth: 220
    minimumHeight: 400
    maximumWidth: 500
    maximumHeight: 700
    
    width: 240
    height: 500

    Component.onCompleted: {
        width = Math.max(myqq.panelSize.width, minimumWidth)
        height = Math.max(myqq.panelSize.height, minimumHeight)
        main.x = Screen.desktopAvailableWidth - main.actualWidth
        main.y = 20
    }
    function windowToActive(){//将窗口转为活动状态
        main.showFront()
        if(main.visible) {
            main.showWindow()//从停靠状态转为可停靠状态
        }
    }

    Connections{
        target: systemTray
        onActivated:{
            if( arg == MySystemTrayIcon.Trigger ) {
                utility.consoleLog("点击了托盘")
                windowToActive()//将窗口转为活动状态
            }
        }
        onTriggered: {
            if(arg == "打开主面板"){
                windowToActive()//将窗口转为活动状态
            }
        }
    }
    
    Connections{
        target: myqq
        onMessageArrive:{
            var temp = JSON.parse(jsonData)
            if(!temp)
                console.log(jsonData)
            //console.log("来自"+myqq.value(uin+"alias", myqq.value(uin+"nick", uin))+"的消息")
            var content
            var i
            var type
            var showMessage=""
            if(senderType == QQ.Friend){//如果是好友消息
                chat_command.addMessage(uin, jsonData)//显示消息
            }else if(senderType == QQ.Group){//如果是群消息
                chat_command.addMessage(uin, jsonData)//显示消息
            }else if(senderType == QQ.Discu){//如果是讨论组消息
                chat_command.addMessage(uin, jsonData)//显示消息
            }else if(senderType == QQ.SystemMessage){//如果是系统消息
                content = temp
                type = content.type
                if(type == QQ.FriendStatusChanged){
                    //console.log("状态改变为："+content.status)
                    //showMessage+="状态改变为："+content.status
                }else if(type == QQ.FriendVerify){
                    console.log("qq号为"+content.account+"要加你为好友")
                    showMessage+="qq号为"+content.account+"要加你为好友"
                }else if(type == QQ.GroupAdmin){
                    if(content.flag==1){
                        console.log(content.uin+"被设置为管理员")
                        showMessage+=content.uin+"被设置为管理员"
                    }else{
                        console.log(content.uin+"被取消管理员资格")
                        showMessage+=content.uin+"被取消管理员资格"
                    }
                }else if(type == QQ.GroupLeave){
                    console.log(content.old_member+"离开了群")
                    showMessage+=content.old_member+"离开了群"
                }
            }
            //if(showMessage!="")
                //systemTray.showMessage("来自"+myqq.value(uin+"alias", myqq.value(uin+"nick", uin))+"的消息", showMessage)
        }
    }
    
    Text{
        anchors.centerIn: parent
        text: "已离线"
        visible: myqq.userStatus == QQ.Offlineing
    }

    MainPanelPage{
        id: panel_page
        visible: myqq.userStatus != QQ.Offlineing
    }
}
