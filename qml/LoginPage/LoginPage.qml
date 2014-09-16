import QtQuick 2.2
import utility 1.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import mywindow 1.0
import "../"

BorderImage{
    source: "qrc:/images/login-panel-shadow.png"
    anchors.fill: parent
    border{left: 20;right: 20;top:20;bottom: 20}
    function reLogin(){//重新登录
        avatar_image.x = -30/77*avatar_image.width
        myqq.loginStatus = QQ.Offline
    }
    SvgView {
        id: root
        //sourceSize.width: width
        width: 4/5*parent.width
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.topMargin: main.height/12
        anchors.rightMargin: main.width/12
        source: "qrc:/images/login-panel.svg"
        
        Connections{
            target: myqq
            onLoginStatusChanged:{
                if( myqq.loginStatus == QQ.Logining )
                    animation_avatar.start()
            }
        }
        
        MyImage{
            id:avatar_image
            maskSource: "qrc:/images/bit.bmp"
            width: 8/45*main.width
            source: utility.getValue(myqq.userQQ+"avatar-100", "qrc:/images/avatar.png")
            x:-30/77*width
            anchors.verticalCenter: inputarea.verticalCenter
            
            onLoadError:{
                source = "qrc:/images/avatar.png"
            }

            SvgView{
                id: rect_border
                //sourceSize.width: width
                source: "qrc:/images/avatar-border.svg"
                anchors.centerIn: parent
                width: avatar_image.width*1.23
            }
    
            Timer{
                id: timer_login
                interval: 10
                onTriggered: myqq.login()
            }
    
            NumberAnimation{
                id: animation_avatar
                target: avatar_image
                property: "x"
                duration: 300
                from: -30/77*avatar_image.width
                to: root.width/2 - avatar_image.width/2
                onStopped: timer_login.start()//必须在结束后启动定时器然后在定时器结束后在调用login()，不然会导致ui卡一下
            }
    
           SvgView{
                id: user_status_icon
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                //sourceSize.width: width
                width: 5/16*avatar_image.width
                visible: myqq.loginStatus == QQ.Offline
                source: "qrc:/images/status-"+myqq.userStatusToString+"-1.svg"
                MyMenu{
                    id: menu_userStatus
                    styleSheet: "QMenu::item:selected {
             background: #F07000;
             color:#E6FFFF;
             height:25px;
         }
         QMenu{
            background-image: url(':/images/menu_background.png');
         }
         QMenu::icon{
            padding-left:8px;
         }
         QMenu::item{
            padding-left:32px;
            padding-right:20px;
            height:22px;
         }
         QMenu::separator {
              height: 1px;
              margin:5px 0px 5px 22px;
              background: #B2C0CD;
         }
    "
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
                }
                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        menu_userStatus.popup()
                    }
                }
            }
            Text{
                text: myqq.userQQ
                font.pointSize: avatar_image.width/8
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.bottom
                anchors.topMargin: 10
                visible: myqq.loginStatus == QQ.Logining
            }
        }
    
        SvgView{
            id:image_quit_icon
            //sourceSize.width: width
            width: 2/45*main.width
            source: "qrc:/images/button-quit.svg"
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.margins: 10
            MouseArea{
                anchors.fill: parent
                onClicked: Qt.quit()
            }
        }
        /*SvgView{
            anchors.left: image_quit_icon.right
            anchors.top: image_quit_icon.top
            width: image_quit_icon.width
            source: "qrc:/images/button-minimize.svg"
            MouseArea{
                anchors.fill: parent
                onClicked: main.showMinimized()
            }
        }*/

        SvgView {
            id: image_qq_for_ubuntu
            width: 1/3*main.width
            //sourceSize.width: width
            source: "qrc:/images/QQ-for-ubuntu.svg"
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.margins: 20
        }
        
        LoginInputArea{
            id: inputarea
            visible: myqq.loginStatus == QQ.Offline
            anchors.left: avatar_image.right
            anchors.leftMargin: 30
            anchors.top: image_qq_for_ubuntu.bottom
            anchors.topMargin: root.height/10
            LoginCheckBox{
                id: checkbox_rememberpassword
                checked: utility.getValue(myqq.userQQ+"rememberpassword", 0)==1
                anchors.left: inputarea.left
                anchors.top: inputarea.bottom
                anchors.topMargin: root.height/12
                text: "记住密码"
                onCheckedChanged: {
                    if( !checked ){
                        checkbox_autologin.checked = false
                        utility.setValue(myqq.userQQ+"password", "")//将密码置空
                    }
                }
            }
            
            LoginCheckBox{
                id:checkbox_autologin
                checked: utility.getValue(myqq.userQQ+"autologin", 0)==1
                anchors.right: inputarea.right
                anchors.rightMargin: 10
                anchors.top: inputarea.bottom
                anchors.topMargin: root.height/12
                text: "自动登录"
                onCheckedChanged: {
                    if(checked)
                        checkbox_rememberpassword.checked = true
                }
            }
        }
        Connections{
            target: myqq
            onUserQQChanged:{
                myqq.userStatus = Number(utility.getValue(myqq.userQQ+"status", QQ.Online))//设置用户的登录状态
            }
        }
        Item{
            id: progress_item
            clip: true
            anchors.bottom: button_login.top
            anchors.bottomMargin: root.height/22-height
            visible: myqq.loginStatus == QQ.Logining
            onVisibleChanged: {
                if(visible)
                    progress_animation1.start()
                else{
                    progress_animation1.stop()
                }
            }

            width: root.width
            height: image_progress1.height
            Image{
                id: image_progress1
                source: "qrc:/images/progress-bar.png"
                width: parent.width
                
                NumberAnimation{
                    id:progress_animation1
                    running: false
                    target: image_progress1
                    property: "x"
                    from: 0
                    to: root.width
                    duration: 5000
                    onStopped: {
                        if( progress_item.visible )
                            start()
                    }
                }
            }
            Image{
                id: image_progress2
                source: "qrc:/images/progress-bar.png"
                width: parent.width
                anchors.right: image_progress1.left
            }
        }
        
        MyButton{
            id:button_login
            anchors.bottom: parent.bottom
            anchors.bottomMargin: root.height/20
            width: 19/45*main.width
            anchors.horizontalCenter: parent.horizontalCenter
            text: myqq.loginStatus != QQ.Offline?"取    消":"登    录"
            font.pointSize: width/15
            
            onClicked: {
                if( myqq.loginStatus == QQ.Offline ){
                    if( myqq.userQQ!=""&&myqq.userPassword!="" ){
                        myqq.loginStatus = QQ.Logining
                        utility.setValue(myqq.userQQ+"autologin", Number(checkbox_autologin.checked))
                        utility.setValue(myqq.userQQ+"rememberpassword", Number(checkbox_rememberpassword.checked))
                        
                        utility.setValue(myqq.userQQ+"status", myqq.userStatus)//设置下次登录的状态
                        utility.setValue("mainqq", myqq.userQQ)//设置当前活动qq为myqq.userQQ
                    }
                }else if( myqq.loginStatus == QQ.Logining ){
                    reLogin()//调用重新登录
                    myqq.loginStatus = QQ.Offline
                }
            }
        }
        SvgView{
            id: button_setting
            source: "qrc:/images/button-settings.svg"
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            
            visible: myqq.loginStatus == QQ.Offline
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    main.openSettingPage()//进行设置
                }
            }
        }

        Component.onCompleted: {
            if( utility.getValue(myqq.userQQ+"autologin", 0)==1 )//此账号如果设置了自动登录
                myqq.loginStatus = QQ.Logining
            forceActiveFocus()
        }
        Keys.onEnterPressed: {
            button_login.clicked()
        }
        Keys.onReturnPressed: {
            button_login.clicked()
        }
    }    
}
