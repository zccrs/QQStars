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
        width = Math.max(200, minimumWidth)
        height = Math.max(500, minimumHeight)
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
