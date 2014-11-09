import QtQuick 2.2
import mywindow 1.0
import QtQuick.Window 2.1
import utility 1.0
import "../Utility"
import "../Chat"
import qqstars 1.0
//import Qt.labs.settings 1.0

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
                windowToActive()//将窗口转为活动状态
            }
        }
        onTriggered: {
            if(arg == "打开主面板"){
                windowToActive()//将窗口转为活动状态
            }
        }
    }
    
    Rectangle{
        anchors.fill: parent
        radius: 10
        gradient: Gradient {
                        GradientStop { position: 0 ; color:  "#EEEDEC" }
                        GradientStop { position: 120/main.height ; color:  "#E7E5E4" }
                        GradientStop { position: 120/main.height+0.01 ; color:  "#f9f9f8" }
                        GradientStop { position: 1 ; color:  "#f9f9f8" }
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
                    myqq.state = QQ.Offline//将状态改为离线
                    Qt.quit()
                }
            }
        }
        SvgView{
            id:image_minimize_icon
            width: defaultSize.width*myqq.windowScale
            source: "qrc:/images/button-minimize.svg"
            anchors.top: image_quit_icon.top
            anchors.left: image_quit_icon.right
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    main.showMinimized()
                }
            }
        }
        MainPanelPage{
            id: panel_page
            anchors.top: image_minimize_icon.bottom
            anchors.topMargin: 10
            anchors.bottom: parent.bottom
            width: parent.width
        }
    }
}
