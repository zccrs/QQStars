import QtQuick 2.2
import mywindow 1.0
import QtQuick.Window 2.1
import utility 1.0
import "../"

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
    
    minWidth: 220
    minHeight: 400
    maxWidth: 500
    maxHeight: 700
    
    width: Math.max(myqq.panelSize.width, minWidth)
    height: Math.max(myqq.panelSize.height, minHeight)
    
    x: Screen.desktopAvailableWidth - width - 20

    Component.onCompleted: {
        y = 20
    }
    function windowToActive(){//将窗口转为活动状态
        if(main.visible) {
            if( main.visibility!= Window.Windowed){
                main.show()
            }
            main.requestActivate()//让窗体显示出来
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
