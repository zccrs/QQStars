import mywindow 1.0
import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import utility 1.0

MyWindow{
    id: root_window
    visible: true//可视的
    color: "transparent"
    noBorder: true//无边框的
    removable: true//可移动的
    fixedSize: true//固定大小的
    dockableWindow: false//可停靠的
    topHint: false//窗口保持在最前端
    noNotifyIcon: false//隐藏任务栏图标
    modality : Qt.ApplicationModal
    width: 400
    height: 7/9*width
    property string str: ""
    property var backFun//验证码获取成功后调用此函数
    onStrChanged: {
        if(str!="")
            code_image.source = "https://ssl.captcha.qq.com/getimage?aid=1003903&r=0.9101365606766194&uin="+myqq.userQQ+"&cap_cd="+str
    }
    function updateCode(){
        code_image.source = ""
        code_image.source = "https://ssl.captcha.qq.com/getimage?aid=1003903&r=0.9101365606766194&uin="+utility.userQQ+"&cap_cd="+str
        code_input.text = ""
    }
    BorderImage{
        source: "qrc:/images/login-panel-shadow.png"
        anchors.fill: parent
        border{left: 20;right: 20;top:20;bottom: 20}
    }
    Connections{
        target: myqq
        onUpdateCode:{
            updateCode()//刷新验证码
        }
        onInputCodeClose:{
            root_window.close()
        }
    }

    SvgView {
        id:root_page
        width: 4/5*parent.width
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.topMargin: parent.height/12
        anchors.rightMargin: parent.width/10
        source: "qrc:/images/login-panel.svg"
        
        SvgView{
            id:image_quit_icon
            //sourceSize.width: width
            width: 2/35*parent.width
            source: "qrc:/images/button-quit.svg"
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.margins: 10
            MouseArea{
                anchors.fill: parent
                onClicked: root_window.close()
            }
        }
        Text{
            id: text_code
            text: "请输入验证码"
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.leftMargin: root_page.width/15
            anchors.topMargin: root_page.height/8
            color: "#f47421"
            font.pointSize: root_page.height/25
        }
        TextField{
            id: code_input
            anchors.left: code_image.left
            anchors.top: code_image.bottom
            anchors.topMargin: 5
            width: code_image.width
            height: 20
            style: TextFieldStyle {
                    textColor: "black"
                    background: BorderImage {
                        source: "qrc:/images/background_input.png"
                        border.left: 5; border.top: 5
                        border.right: 5; border.bottom: 5
                    }
                }
            Component.onCompleted: code_input.forceActiveFocus()
        }
        Image{
            id: code_image
            width: 130
            height: 53
            cache: false
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top :text_code.top
            anchors.topMargin: root_page.height/12
            source: "https://ssl.captcha.qq.com/getimage?aid=1003903&r=0.9101365606766194&uin="+utility.userQQ+"&cap_cd="+str
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    parent.source = ""
                    parent.source = "https://ssl.captcha.qq.com/getimage?aid=1003903&r=0.9101365606766194&uin="+utility.userQQ+"&cap_cd="+str
                }
            }
        }
        MyButton{
            id: button_affirm
            anchors.bottom: parent.bottom
            anchors.bottomMargin: root_page.height/20
            width: 19/42*parent.width
            anchors.horizontalCenter: parent.horizontalCenter
            text:"确    认"
            font.pointSize: width/15
            onClicked: {
                if( myqq.loginStatus == QQ.Logining&&code_input.text!="" ) {
                    backFun(code_input.text)
                }
            }
        }
        Component.onCompleted: {
            forceActiveFocus()
        }
        Keys.onEnterPressed: {
            button_affirm.clicked()
        }
        Keys.onReturnPressed: {
            button_affirm.clicked()
        }
    }
}
