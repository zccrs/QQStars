import QtQuick 2.2
import utility 1.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQuick.Particles 2.0
import mywindow 1.0
import qqstars 1.0
import "../../Utility"
import "../"

Item{
    property alias contentItem: effect
    
    width: effect.actualWidth
    height: effect.actualHeight
    function reLogin(){
        myqq.loginStatus = QQ.WaitLogin
        animation_avatar.stop()//停止动画
        avatar_image.x = -30/77*avatar_image.width//将头像位置复原
    }
    
    MyRectangularGlow{
        id: effect
        glowRadius: 50
        spread: 0.1
        color: "black"
        glowOpacity: 0.75
        width: root.width
        height: root.height
        biasY: 20
       
        item:SvgView {
            id: root
            source: "qrc:/images/login-panel2.svg"
            width: defaultSize.width*myqq.windowScale
            
            Connections{
                target: myqq
                onLoginStatusChanged:{
                    if( myqq.loginStatus == QQ.Logining )
                        animation_avatar.start()
                }
            }
            ParticleSystem {
                id: particles
                anchors.fill: parent
                //running: true
                ImageParticle {
                    source: "qrc:/images/未标题-1.png"
                }
                Emitter {
                    id: pulseEmitter
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.margins: 120
                    emitRate: 10
                    lifeSpan: 2000
                    velocity: AngleDirection{
                        magnitude: 35; 
                        magnitudeVariation: 20
                        angle: -90
                        angleVariation: 45
                    }
                    size: 5
                    sizeVariation: 20
                }
            }
            MyImage{
                id:avatar_image
                maskSource: "qrc:/images/bit.bmp"
                width: 80*myqq.windowScale
                height: width
                sourceSize.width: width
                source: myqq.avatar240
                x:-30/80*width
                anchors.verticalCenter: inputarea.verticalCenter


                Connections{
                    target: myqq
                    onUserQQChanged:{
                        avatar_image.source = myqq.avatar240
                    }
                }

                onLoadError:{
                    console.log("头像加载出错:"+myqq.avatar240)
                    source = "qrc:/images/avatar.png"
                }
                
                SvgView{
                    id: rect_border
                    source: "qrc:/images/avatar-border.svg"
                    anchors.centerIn: parent
                    width: avatar_image.width*1.23
                }
        
                Timer{
                    id: timer_login
                    interval: 10
                    onTriggered: myqq.login()//开始登录
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
                    width: defaultSize.width*myqq.windowScale
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    anchors.margins: -0.02*avatar_image.width
                    //sourceSize.width: width
                    //width: 5/16*avatar_image.width
                    enabled: myqq.loginStatus == QQ.WaitLogin
                    source: "qrc:/images/status-"+myqq.stateToString+"-1.svg"
                    MyMenu{
                        id: menu_state
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
                    }
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            menu_state.popup()
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
                width: defaultSize.width*myqq.windowScale
                source: "qrc:/images/button-quit.svg"
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.margins: 10
                MouseArea{
                    anchors.fill: parent
                    onClicked: Qt.quit()
                }
            }
        
            SvgView {
                id: image_qq_for_ubuntu
                width: defaultSize.width*myqq.windowScale
                //sourceSize.width: width
                source: "qrc:/images/QQ-for-ubuntu.svg"
                anchors.top: parent.top
                anchors.right: parent.right
                anchors.margins: 20
            }
            
            LoginInputArea{
                id: inputarea
                width: 220*myqq.windowScale
                height: 64*myqq.windowScale
                visible: myqq.loginStatus == QQ.WaitLogin
                anchors.left: avatar_image.right
                anchors.leftMargin: 30
                anchors.top: image_qq_for_ubuntu.bottom
                anchors.topMargin: root.height/10
                Component.onCompleted: {
                    //console.log(width)
                    //console.log(height)
                }

                LoginCheckBox{
                    id: checkbox_rememberpassword
                    //height:2/40*root.width
                    checked: myqq.rememberPassword//myqq.value("rememberpassword", 0)==1
                    anchors.left: inputarea.left
                    anchors.top: inputarea.bottom
                    anchors.topMargin: root.height/12
                    text: "记住密码"
                    onCheckedChanged: {
                        if( !checked ){
                            checkbox_autologin.checked = false
                            //myqq.removeValue("password")//将密码置空
                        }
                    }
                }
                
                LoginCheckBox{
                    id:checkbox_autologin
                    //height:2/40*root.width
                    checked: myqq.autoLogin//myqq.value("autologin", 0)==1
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
                    checkbox_rememberpassword.checked = myqq.rememberPassword//myqq.value("rememberpassword", 0)==1
                    checkbox_autologin.checked = myqq.autoLogin//myqq.value("autologin", 0)==1
                }
            }
            Item{
                id: progress_item
                clip: true
                anchors.bottom: parent.bottom
                anchors.bottomMargin: root.height/4.2-height
                visible: myqq.loginStatus == QQ.Logining
                onVisibleChanged: {
                    if(visible)
                        progress_animation1.start()
                    else{
                        progress_animation1.stop()
                    }
                }
        
                width: root.implicitWidth
                height: image_progress1.implicitHeight
                Image{
                    id: image_progress1
                    source: "qrc:/images/progress-bar.png"
                    width: root.width
                    
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
                    width: root.width
                    anchors.right: image_progress1.left
                }
            }
            
            MyLoginButton{
                id:button_login
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 30/250*root.height-height/2-1*height/defaultSize.height
                width: defaultSize.width*myqq.windowScale

                anchors.horizontalCenter: parent.horizontalCenter
                text: myqq.loginStatus != QQ.WaitLogin?"取    消":"登    录"
                font.pointSize: width/15
                
                onClicked: {
                    if( myqq.loginStatus == QQ.WaitLogin ){
                        if( myqq.userQQ!=""&&myqq.userPassword!="" ){
                            myqq.loginStatus = QQ.Logining
                            myqq.autoLogin = checkbox_autologin.checked
                            myqq.rememberPassword = checkbox_rememberpassword.checked
                            myqq.saveUserPassword()//保存密码
                            utility.setValue("mainqq", myqq.userQQ)//设置当前活动qq为myqq.userQQ
                        }
                    }else if( myqq.loginStatus == QQ.Logining ){
                        reLogin()//调用重新登录
                        myqq.loginStatus = QQ.WaitLogin
                    }
                }
            }
            SvgView{
                id: button_setting
                width: defaultSize.width*myqq.windowScale
                source: "qrc:/images/button-settings.svg"
                anchors.right: parent.right
                anchors.verticalCenter: button_login.verticalCenter
                visible: myqq.loginStatus == QQ.WaitLogin
                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        main.openSettingPage()//进行设置
                    }
                }
            }
        
            Component.onCompleted: {
                //if( myqq.value("autologin", 0)==1 )//此账号如果设置了自动登录
                if(myqq.autoLogin)//如果是自动登录
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
}
