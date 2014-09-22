import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import utility 1.0
import mywindow 1.0
import "../"

Item{
    width: effect.x+effect.actualWidth()
    height: effect.y+effect.actualHeight()
    MyRectangularGlow{
        id: effect
        glowRadius: 50
        spread: 0.1
        color: "black"
        glowOpacity: 0.75
        width: root.width
        height: root.height
        biasY: 10

        item:SvgView {
            id:root
            source: "qrc:/images/login-panel.svg"
            width: defaultSize.width*myqq.windowScale
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
                        main.openLoginPage()//返回登录
                    }
                }
            }
    
            SvgView {
                id: image_qq_for_ubuntu
                width: defaultSize.width*myqq.windowScale
                source: "qrc:/images/QQ-for-ubuntu.svg"
                anchors.top: parent.top
                anchors.right: parent.right
                anchors.margins: 20
            }
            Grid{
                anchors.centerIn: parent
                width: 2*parent.width/3
                height: parent.height/2
                
                columns: 2
                rowSpacing: root.width/20
                columnSpacing: root.height/20
                horizontalItemAlignment: Qt.AlignHCenter
                verticalItemAlignment: Qt.AlignVCenter
                
                MyTextField{
                    id: proxy_location_input
                    enabled: proxy_combo.currentIndex != 0
                    width: root.width/3
                    height: width/8
                    title: "地址:"
                    text: utility.getValue("proxyLocation", "")
                }
                MyTextField{
                    id: proxy_port_input
                    enabled: proxy_combo.currentIndex != 0
                    width: root.width/3
                    height: width/8
                    title: "端口:"
                    text: utility.getValue("proxyPort", "")
                }
                MyTextField{
                    id: proxy_username_input
                    enabled: proxy_combo.currentIndex != 0
                    width: root.width/3
                    height: width/8
                    title: "账户:"
                    text: utility.getValue("proxyUsername", "")
                }
                MyTextField{
                    id: proxy_password_input
                    enabled: proxy_combo.currentIndex != 0
                    width: root.width/3
                    height: width/8
                    title: "密码:"
                    text: utility.getValue("proxyPassword", "")
                }
                Item{
                    width: root.width/3
                    height: width/8
                    
                    Text {
                        id: proxy_type_text
                        text: "类型:"
                        font.pointSize: parent.height/2
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    
                    MyComboBox{
                        id: proxy_combo
                        height: parent.height
                        anchors.left: proxy_type_text.right
                        anchors.right: parent.right
                        currentIndex: utility.getValue("proxyTypeIndex", 0)
                        model: ListModel {
                            ListElement { text: "不使用代理"; value: Utility.NoProxy}
                            ListElement { text: "HTTP代理"; value: Utility.HttpProxy}
                            ListElement { text: "SOCKS5代理"; value: Utility.Socks5Proxy}
                        }
                    }
                }
                Button {
                    text: "测  试"
                    width: root.width/5
                    height: width/4
                    visible: proxy_combo.currentIndex != 0
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
                            border.width: 1
                            border.color: "#aaa"
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
            }
            
            
            MyButton{
                id: button_affirm
                anchors.bottom: parent.bottom
                anchors.bottomMargin: root.height/20
                width: defaultSize.width*myqq.windowScale
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
            Keys.onEnterPressed: {
                button_affirm.clicked()
            }
            Keys.onReturnPressed: {
                button_affirm.clicked()
            }
        }    
    }
}
