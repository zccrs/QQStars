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
        anchors.rightMargin: main.width/12
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
            text: "地址:"
            anchors.left: parent.left
            anchors.leftMargin: root.width/2-width-proxy_location_input.width-5
            anchors.top: image_quit_icon.bottom
            anchors.topMargin: 40
            font.pointSize: root.height/25
        }
        TextField{
            id: proxy_location_input
            anchors.left: proxy_location_text.right
            anchors.verticalCenter: proxy_location_text.verticalCenter
            height: 20
            width:proxy_combo.width
            font.pointSize:proxy_location_text.font.pointSize
            text: utility.getValue("proxyLocation", "")

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
            font.pointSize: root.height/25
        }
        TextField{
            id: proxy_port_input
            anchors.verticalCenter: proxy_port_text.verticalCenter
            anchors.left: proxy_port_text.right
            height: 20
            width: proxy_combo.width
            font.pointSize: proxy_port_text.font.pointSize
            text: utility.getValue("proxyPort", "")
            
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
            text: "账户:"
            anchors.left: proxy_location_text.left
            anchors.top: proxy_port_input.bottom
            anchors.topMargin: 20
            font.pointSize: root.height/25
        }
        TextField{
            id: proxy_username_input
            anchors.left: proxy_username_text.right
            anchors.verticalCenter: proxy_username_text.verticalCenter
            height: 20
            width:proxy_combo.width
            font.pointSize: proxy_username_text.font.pointSize
            text: utility.getValue("proxyUsername", "")

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
            font.pointSize: root.height/25
        }
        TextField{
            id: proxy_password_input
            anchors.verticalCenter: proxy_password_text.verticalCenter
            anchors.left: proxy_password_text.right
            height: 20
            width: proxy_combo.width
            font.pointSize: proxy_password_text.font.pointSize
            text: utility.getValue("proxyPassword", "")
            
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
            text: "类型:"
            anchors.left: proxy_location_text.left
            anchors.top: proxy_password_input.bottom
            anchors.topMargin: 20
            font.pointSize: root.height/25
        }
        
        MyComboBox{
            id: proxy_combo
            width: 120
            height: 20
            anchors.left: proxy_type_text.right
            anchors.verticalCenter: proxy_type_text.verticalCenter
            currentIndex: utility.getValue("proxyTypeIndex", 0)
            
            model: ListModel {
                ListElement { text: "不使用代理"; value: Utility.NoProxy}
                ListElement { text: "HTTP代理"; value: Utility.HttpProxy}
                ListElement { text: "SOCKS5代理"; value: Utility.Socks5Proxy}
            }
        }

        Button {
            anchors.verticalCenter: proxy_combo.verticalCenter
            anchors.horizontalCenter: proxy_password_input.horizontalCenter
            text: "测  试"
            
            onClicked:{
                utility.setApplicationProxy(proxy_combo.currentValue, proxy_location_input.text, proxy_port_input.text, proxy_username_input.text, proxy_password_input.text)
                utility.socketSend(testNetwork, "http://d.web2.qq.com/channel/poll2")
                button_affirm.enabled = false
                enabled = false
            }
            
            function testNetwork(error ,data) {
                var temp1 = utility.getValue("proxyLocation", "")
                var temp2 = utility.getValue("proxyPort", "")
                var temp3 = utility.getValue("proxyUsername", "")
                var temp4 = utility.getValue("proxyPassword", "")
                var temp5 = utility.getValue("proxyType", Utility.NoProxy)
                utility.setApplicationProxy(temp5, temp1, temp2, temp3, temp4)//将代理设置复原
                button_affirm.enabled = true
                enabled = true
                if( error )
                    myqq.showWarningInfo("测试失败")
                else
                    myqq.showWarningInfo("测试通过")
            }
            style: ButtonStyle {
                background: Rectangle {
                    implicitWidth: 80
                    implicitHeight: 20
                    radius: 6
                    gradient: Gradient {
                        GradientStop { position: 0 ; color: {
                                if(!control.enabled)
                                    return "#888"
                                if( control.pressed )
                                    return "#ff9000"
                                if( control.hovered )
                                    return "#eee"
                                return "#fff"
                            }
                        }
                        GradientStop { position: 1 ; color: {
                                if(!control.enabled)
                                    return "#777"
                                if( control.pressed )
                                    return "#f07000"
                                if( control.hovered )
                                    return "#ddd"
                                return "#eee"
                            }
                        }
                    }
                }
                label: Text{}
            }
            Text{
                text: parent.text
                anchors.centerIn: parent
                color: parent.pressed?"white":"black"
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
                utility.setValue("proxyLocation", proxy_location_input.text)
                utility.setValue("proxyPort", proxy_port_input.text)
                utility.setValue("proxyUsername", proxy_username_input.text)
                utility.setValue("proxyPassword", proxy_password_input.text)
                utility.setValue("proxyType", proxy_combo.currentValue)
                utility.setValue("proxyTypeIndex", proxy_combo.currentIndex)
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
