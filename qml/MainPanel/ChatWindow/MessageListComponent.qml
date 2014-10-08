import QtQuick 2.2
import mywindow 1.0

Component{
    Item{
        id: root
        width: parent.width
        height: nick.implicitHeight+backgound.height+backgound.anchors.topMargin

        MyImage{
            id: avatar
            x:mode=="left"?0:root.width-width
            width:40
            maskSource: "qrc:/images/bit.bmp"
            source: myqq.value(uin+"avatar-40", "qrc:/images/avatar.png")
            onLoadError: {
                avatar.source = "qrc:/images/avatar.png"
            }
        }
        Text{
            id: nick
            x: mode=="left"?avatar.x+avatar.width+5:avatar.x-implicitWidth-5
            anchors.top: avatar.top
            text: myqq.value(uin+"alias", myqq.value(uin+"alias", myqq.value(uin+"nick", uin)))
        }

        BorderImage {
            id: backgound
            x: mode=="left"?avatar.x+avatar.width+5:avatar.x-width-5
            anchors.top: nick.bottom
            anchors.topMargin: 5
            source: "qrc:/images/bubble_放飞心情_"+mode+".png"
            height: mytext.height+20
            width: mytext.width+30
            border.left: 20; border.top: 20
            border.right: 20; border.bottom: 20

            TextEdit{
                id: mytext
                anchors.centerIn: parent
                readOnly: true
                selectByMouse :true
                selectByKeyboard :true
                onWidthChanged: {
                    var temp = root.width-avatar.width-35
                    if(width>temp){
                        width = temp
                    }
                }

                wrapMode: TextEdit.Wrap
                text: message
            }
        }
    }
}
