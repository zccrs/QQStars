import mywindow 1.0
import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import utility 1.0
import qqstars 1.0

MyWindow{
    id: root_window
    visible: true//可视的
    color: "transparent"
    noBorder: true//无边框的
    removable: true//可移动的
    fixedSize: true//固定大小的
    dockableWindow: false//可停靠的
    topHint: false//窗口保持在最前端
    noNotifyIcon: true//隐藏任务栏图标
    modality : Qt.ApplicationModal
    width: root_page.width
    height: root_page.height
    
    property string code: code_input.text
    property var backFun//验证码获取成功后调用此函数
    property alias source: code_image.source

    function updateCode(){
        var temp = source
        source = ""
        source = temp
        code_input.text = ""
    }
    SvgView {
        id:root_page
        width: 300*myqq.windowScale
        source: "qrc:/images/login-panel.svg"
        SvgView{
            id:image_quit_icon
            width: defaultSize.width*myqq.windowScale
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
            anchors.topMargin: root_page.height/6
            color: "#f47421"
            font.pointSize: root_page.height/25
        }
        TextField{
            id: code_input
            anchors.left: code_image.left
            anchors.top: code_image.bottom
            anchors.topMargin: 10
            width: code_image.width
            height: root_page.height/12
            font.pointSize: text_code.font.pointSize
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

            MouseArea{
                anchors.fill: parent
                onClicked: {
                    updateCode()
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
        Keys.onEnterPressed: {
            button_affirm.clicked()
        }
        Keys.onReturnPressed: {
            button_affirm.clicked()
        }
    }
}
