import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import utility 1.0
import mywindow 1.0
import "../Utility"
import "../Utility/ComboBox"

Item{
    width: effect.actualWidth
    height: effect.actualHeight
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
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: image_qq_for_ubuntu.bottom
                anchors.topMargin: root.height/10
                
                columns: 2
                rowSpacing: root.width/20
                columnSpacing: root.height/20
                horizontalItemAlignment: Qt.AlignHCenter
                verticalItemAlignment: Qt.AlignVCenter
                
                MyTextField{
                    id: proxy_location_input
                    enabled: proxy_combo.currentIndex != 0
                    width: root.width/2.5
                    height: width/8
                    title: "地址:"
                    text: utility.value("proxyLocation", "")
                }
                MyTextField{
                    id: proxy_port_input
                    enabled: proxy_combo.currentIndex != 0
                    width: root.width/2.5
                    height: width/8
                    title: "端口:"
                    text: utility.value("proxyPort", "")
                }
                MyTextField{
                    id: proxy_username_input
                    enabled: proxy_combo.currentIndex != 0
                    width: root.width/2.5
                    height: width/8
                    title: "账户:"
                    text: utility.value("proxyUsername", "")
                }
                MyTextField{
                    id: proxy_password_input
                    enabled: proxy_combo.currentIndex != 0
                    width: root.width/2.5
                    height: width/8
                    title: "密码:"
                    text: utility.value("proxyPassword", "")
                }
                Item{
                    width: root.width/2.5
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
                        currentIndex: utility.value("proxyTypeIndex", 0)
                        model: ListModel {
                            ListElement { text: "不使用代理"; value: Utility.NoProxy}
                            ListElement { text: "HTTP代理"; value: Utility.HttpProxy}
                            ListElement { text: "SOCKS5代理"; value: Utility.Socks5Proxy}
                        }
                    }
                }
                MyButton {
                    text: "测  试"
                    width: root.width/4
                    height: width/4
                    visible: proxy_combo.currentIndex != 0
                    font.pointSize: proxy_type_text.font.pointSize
                    onClicked:{
                        console.log("fdsafds")
                        utility.setApplicationProxy(proxy_combo.currentValue, proxy_location_input.text, proxy_port_input.text, proxy_username_input.text, proxy_password_input.text)
                        utility.httpGet(testNetwork, "http://d.web2.qq.com/channel/poll2")
                        button_affirm.enabled = false
                        enabled = false
                    }
                    
                    function testNetwork(error ,data) {
                        var temp1 = utility.value("proxyLocation", "")
                        var temp2 = utility.value("proxyPort", "")
                        var temp3 = utility.value("proxyUsername", "")
                        var temp4 = utility.value("proxyPassword", "")
                        var temp5 = utility.value("proxyType", Utility.NoProxy)
                        utility.setApplicationProxy(temp5, temp1, temp2, temp3, temp4)//将代理设置复原
                        button_affirm.enabled = true
                        enabled = true
                        if( error )
                            myqq.showWarningInfo("测试失败")
                        else
                            myqq.showWarningInfo("测试通过")
                    }
                    
                }
            }

            MyLoginButton{
                id: button_affirm
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 30/250*root.height-height/2-1*height/defaultSize.height
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
