import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQuick.Window 2.1
import QtWinExtras 1.0
import mywindow 1.0
import utility 1.0
import Qt.labs.settings 1.0
import "../"

MyWindow{
    id:main
    windowIcon: "qrc:/images/avatar.png"
    //width: 410/1366*screen.size.width
    //height: 31/41*width
    width: login_page.width
    height: login_page.height
    visible: true//可视的
    noBorder: true//无边框的
    removable: true//可移动的
    fixedSize: true//固定大小的
    dockableWindow: false//可停靠的
    topHint: false//窗口保持在最前端
    noNotifyIcon: false//隐藏任务栏图标
    
    color: "transparent"
    Component.onCompleted: {
        main.x = screen.size.width/2 - main.width/2//让程序居中显示
        main.y = screen.size.height/2 - main.height/2
    }

    Connections{
        target: myqq
        onError:{
            if( message.indexOf("验证码")<0 ){
                login_page.reLogin()//重新登录
                myqq.inputCodeClose()//关闭输入验证码
            }
        }
        onLoginStatusChanged:{
            if(myqq.loginStatus == QQ.LoginFinished){
                utility.loadQml("qml/MainPanelPage/main.qml")
                main.close()
            }
        }
    }

    Connections{
        target: systemTray
        onActivated:{
            if( arg == MySystemTrayIcon.Trigger ) {
                if(main.visible) {
                    if( main.visibility!= Window.Windowed){
                        main.show()
                    }
                    main.requestActivate()//让窗体显示出来
                }
            }
        }
        onTriggered: {
            if(arg == "打开主面板"){
                if(main.visible) {
                    if( main.visibility!= Window.Windowed){
                        main.show()
                    }
                    main.requestActivate()//让窗体显示出来
                }
            }
        }
    }
    function openSettingPage() {//进行设置
        settings_page.enabled = true
        flipable.flipped = false
    }
    function openLoginPage() {//打开登录面板
        login_page.enabled = true
        flipable.flipped = true
    }
    
    ThumbnailToolBar {
        ThumbnailToolButton { iconSource: "qrc:/images/avatar.png"; tooltip: "登录"; }
        ThumbnailToolButton { interactive: false; flat: true }
        ThumbnailToolButton { iconSource: "qrc:/images/avatar.png"; tooltip: "退出";  }
    }
    
    Flipable {
         id: flipable
         anchors.fill: parent
         property bool flipped: true
         onFlippedChanged:{
             timer.start()
         }
         Timer{
             id: timer
             interval: 200
             onTriggered: {
                 if( flipable.flipped ){
                     settings_page.enabled = false
                 }else{
                     login_page.enabled = false
                 }
             }
         }

         front: LoginPage{
             id: login_page
             anchors.right: parent.right
         }
         back: SettingPage{
             id: settings_page;
             enabled: false
             anchors.right: parent.right
         }
    
         transform: Rotation {
             id: rotation
             origin.x: flipable.width/2
             origin.y: flipable.height/2
             axis.x: 0; axis.y: 1; axis.z: 0     // set axis.y to 1 to rotate around y-axis
             angle: 0    // the default angle
         }
    
         states: State {
             name: "back"
             PropertyChanges { target: rotation; angle: 180 }
             when: !flipable.flipped
         }

         transitions: Transition {
             NumberAnimation { 
                 target: rotation; 
                 property: "angle"; 
                 duration: timer.interval ;
                 easing.type: Easing.InQuart
             }
         }
    }
}
