import QtQuick 2.2
import QtQuick.Particles 2.0
import mywindow 1.0
import utility 1.0
//import Qt.labs.settings 1.0
import QQItemInfo 1.0
import MyTextEditPlugin 1.0
import "../Utility"
import "../MainPanel"
import "LoginPanel"
import "../Chat"

MyWindow{
    id:main
    windowIcon: "qrc:/images/avatar.png"
    actualWidth: 350*myqq.windowScale+200
    actualHeight: 250*myqq.windowScale+150
    visible: true//可视的
    contentItemAreaTop: login_page.contentItem.y///////////////////////////设置
    contentItemAreaBottom: contentItemAreaTop+login_page.contentItem.height//响应
    contentItemAreaLeft: login_page.contentItem.x/////////////////////////鼠标的
    contentItemAreaRight: contentItemAreaLeft+login_page.contentItem.width//区域
    noBorder: true//无边框的
    removable: true//可移动的
    fixedSize: true//固定大小的
    dockableWindow: false//可停靠的
    topHint: false//窗口保持在最前端
    noNotifyIcon: true//隐藏任务栏图标
    windowGlow: false//是否开启阴影
    color: "transparent"

    function openSettingPage() {//进行设置
        settings_page.enabled = true
        flipable.flipped = false
    }
    function openLoginPage() {//打开登录面板
        login_page.enabled = true
        flipable.flipped = true
    }
    Connections{
        target: myqq
        onError:{
            if( message.indexOf("验证码")<0 ){
                login_page.reLogin()//重新登录
                myqq.closeCodeWindow()//关闭输入验证码的窗口
            }else{
                myqq.updataCode()//刷新验证码
            }
        }
    }

    Connections{
        target: systemTray
        onActivated:{
            if( arg == MySystemTrayIcon.Trigger ) {
                main.showFront()
            }
        }
        onTriggered: {
            if(arg == "打开主面板"){
                main.showFront()
            }
        }
    }
    
    ParticleSystem {//粒子系统
        id: particles
        anchors.centerIn: parent
        width: main.actualWidth
        height: main.actualHeight
        Age{
            system: particles
            anchors.left: parent.left
            height: parent.height
            width: 80
            once: true
            lifeLeft: 3000
            advancePosition: false
            
        }
        Age{
            system: particles
            anchors.right: parent.right
            height: parent.height
            width: 80
            once: true
            lifeLeft: 3000
            advancePosition: false
            
        }
        
        Age{
            system: particles
            anchors.bottom: parent.bottom
            height: 80
            width: parent.width
            once: true
            lifeLeft: 3000
            advancePosition: false
            
        }
        
        ImageParticle {
            source: "qrc:/images/star.png"
            groups: ['star']
            alpha: 0.2
            alphaVariation: 0.1
            colorVariation: 2
            autoRotation: true
            rotationVariation: 360
            rotationVelocity: 40
        }
        Emitter {
            id: pulseEmitter
            anchors.top: parent.top
            width: parent.width
            group: 'star'
            emitRate: 8
            lifeSpan: parent.height*1000/40
            velocityFromMovement: 20
            velocity: AngleDirection{
                angle: 90
                angleVariation: 50
                magnitude: 90
                magnitudeVariation: 10
            }
            acceleration: AngleDirection{
                angle: -90
                magnitude: 5
                magnitudeVariation: 2
            }
            size: 20
            sizeVariation: 5
            endSize: 0
        }
        
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
