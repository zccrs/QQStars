import QtQuick 2.2
import QtQuick.Window 2.0
import mywindow 1.0
//import "../QQItemInfo"

Window{
    id: root
    visible: true
    flags: Qt.SplashScreen
    height: list.contentHeight+user_nick.implicitHeight+item_button.height+30
    width: 200
    color: "transparent"
    property int trayX
    property int trayY
    property int trayWidth
    property alias mymodel: list.model
    
    x: trayX-width/2+trayWidth/2
    y: trayY-height-5
    MouseArea{
        anchors.fill: parent
        property bool hovered: false
        hoverEnabled: true
        onEntered: {
            hovered = true
        }
        onExited: {
            hovered = false
            //root.destroy()
        }
    }
    Rectangle{
        anchors.fill: parent
        border.width: 1
        border.color: "#f07000"
        color: "white"
        radius: 10
        Text{
            id: user_nick
            text: "测试"
            x:10
            y:10
        }

        ListView{
            id:list
            delegate: list_delegate
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: user_nick.bottom
            anchors.bottom: item_button.top
            anchors.margins: 10
        }
        
        Item{
            id: item_button
            anchors.bottom: parent.bottom
            anchors.left: list.left
            anchors.right: list.right
            height: 30
            Rectangle{
                width: parent.width
                height: 1
                color: "#f07000"
            }

            Text{
                text: "忽略全部"
                color: "#f07000"
                font.underline: true
                anchors.verticalCenter: parent.verticalCenter
                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        root.destroy()
                    }
                }
            }
            Text{
                text: "查看全部"
                color: "#f07000"
                font.underline: true
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
            }
        }
    }
    Component{
        id: list_delegate
        Item{
            width: parent.width
            height: 30
            property var myinfo: sender_info
            MyImage{
                id: avatar
                x:10
                width:parent.height
                maskSource: "qrc:/images/bit.bmp"
                cache: false
                source: myinfo.avatar40
            }
            Text{
                id: nick
                anchors.left: avatar.right
                anchors.leftMargin: 10
                text: myinfo.aliasOrNick
            }
        }
    }
}
