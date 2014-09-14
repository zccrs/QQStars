import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import utility 1.0
import mywindow 1.0
import "../"

BorderImage{
    source: "qrc:/images/login-panel-shadow.png"
    anchors.fill: parent
    border{left: 20;right: 20;top:20;bottom: 20}
    SvgView {
        id:root
        //sourceSize.width: width
        width: 4/5*parent.width
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.topMargin: main.height/12
        anchors.rightMargin: main.width/10
        source: "qrc:/images/login-panel.svg"
        
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
                onClicked: {
                    main.openLoginPage()//返回登录
                }
            }
        }

        SvgView {
            id: image_qq_for_ubuntu
            //sourceSize.width: width
            width: 1/3*main.width
            source: "qrc:/images/QQ-for-ubuntu.svg"
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.margins: 20
        }
        
        Text {
            id: proxy_location_text
            text: "代理地址:"
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.top: image_quit_icon.bottom
            anchors.topMargin: 40
        }
        TextField{
            id: proxy_location_input
            anchors.left: proxy_location_text.right
            anchors.verticalCenter: proxy_location_text.verticalCenter
            height: 20
            width:proxy_combo.width
            text: utility.getValue("proxyLocation", "")
            onTextChanged: {
                utility.setValue("proxyLocation", text)
            }

            style: TextFieldStyle {
                    textColor: "black"
                    background: BorderImage {
                        source: "qrc:/images/background_input.png"
                        border.left: 5; border.top: 5
                        border.right: 5; border.bottom: 5
                    }
                }
            Component.onCompleted: proxy_location_input.forceActiveFocus()
        }
        Text{
            id: proxy_port_text
            text: "端口:"
            anchors.verticalCenter: proxy_location_input.verticalCenter
            anchors.left: proxy_location_input.right
            anchors.leftMargin: 10
        }
        TextField{
            id: proxy_port_input
            anchors.verticalCenter: proxy_port_text.verticalCenter
            anchors.left: proxy_port_text.right
            height: 20
            width: 60
            text: utility.getValue("proxyPort", "")
            onTextChanged: {
                utility.setValue("proxyPort", text)
            }
            style: TextFieldStyle {
                    textColor: "black"
                    background: BorderImage {
                        source: "qrc:/images/background_input.png"
                        border.left: 5; border.top: 5
                        border.right: 5; border.bottom: 5
                    }
                }
        }
        Text {
            id: proxy_username_text
            text: "用 户 名:"
            anchors.left: proxy_location_text.left
            anchors.top: proxy_port_input.bottom
            anchors.topMargin: 20
        }
        TextField{
            id: proxy_username_input
            anchors.left: proxy_username_text.right
            anchors.verticalCenter: proxy_username_text.verticalCenter
            height: 20
            width:proxy_combo.width
            text: utility.getValue("proxyUsername", "")
            onTextChanged: {
                utility.setValue("proxyUsername", text)
            }

            style: TextFieldStyle {
                    textColor: "black"
                    background: BorderImage {
                        source: "qrc:/images/background_input.png"
                        border.left: 5; border.top: 5
                        border.right: 5; border.bottom: 5
                    }
                }
        }
        Text{
            id: proxy_password_text
            text: "密码:"
            anchors.verticalCenter: proxy_username_input.verticalCenter
            anchors.left: proxy_username_input.right
            anchors.leftMargin: 10
        }
        TextField{
            id: proxy_password_input
            anchors.verticalCenter: proxy_password_text.verticalCenter
            anchors.left: proxy_password_text.right
            height: 20
            width: 60
            text: utility.getValue("proxyPassword", "")
            onTextChanged: {
                utility.setValue("proxyPassword", text)
            }
            style: TextFieldStyle {
                    textColor: "black"
                    background: BorderImage {
                        source: "qrc:/images/background_input.png"
                        border.left: 5; border.top: 5
                        border.right: 5; border.bottom: 5
                    }
                }
        }
        Text {
            id: proxy_type_text
            text: "类    型:"
            anchors.left: proxy_location_text.left
            anchors.top: proxy_password_input.bottom
            anchors.topMargin: 20
        }
        
        MyComboBox{
            id: proxy_combo
            width: 120
            height: 20
            anchors.left: proxy_type_text.right
            anchors.verticalCenter: proxy_type_text.verticalCenter
            currentIndex: utility.getValue("proxyTypeIndex", 0)
            onCurrentIndexChanged: {
                utility.setValue("proxyType", currentValue)
                utility.setValue("proxyTypeIndex", currentIndex)
            }
            model: ListModel {
                ListElement { text: "不使用代理"; value: Utility.NoProxy}
                ListElement { text: "HTTP代理"; value: Utility.HttpProxy}
                ListElement { text: "SOCKS5代理"; value: Utility.Socks5Proxy}
            }
        }

        MyButton{
            id: button_affirm
            anchors.bottom: parent.bottom
            anchors.bottomMargin: root.height/20
            width: 19/45*main.width
            anchors.horizontalCenter: parent.horizontalCenter
            text:"确    认"
            font.pointSize: width/15
            onClicked: {
                utility.setApplicationProxy(proxy_combo.currentValue, proxy_location_input.text, proxy_port_input.text, proxy_username_input.text, proxy_password_input.text)
                main.openLoginPage()//返回登录
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
