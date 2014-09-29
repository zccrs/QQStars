import QtQuick 2.2
import mywindow 1.0

Component{
    Item{
        id: root
        width: parent.width
        height: nick.implicitHeight+backgound.height+backgound.anchors.topMargin
        MyImage{
            id: avatar
            x:10
            width:40
            maskSource: "qrc:/images/bit.bmp"
            source: myqq.getValue(uin+"avatar-40", "qrc:/images/avatar.png")
            onLoadError: {
                avatar.source = "qrc:/images/avatar.png"
            }
        }
        Text{
            id: nick
            anchors.left: avatar.right
            anchors.leftMargin: 5
            anchors.top: avatar.top
            text: myqq.getValue(uin+"alias", myqq.getValue(uin+"nick", uin))
        }

        BorderImage {
            id: backgound
            anchors.left: nick.left
            anchors.top: nick.bottom
            anchors.topMargin: 5
            source: "qrc:/images/bubble_放飞心情_"+mode+".png"
            height: mytext.height+20
            width: mytext.width+40
            border.left: 20; border.top: 20
            border.right: 20; border.bottom: 20
            TextEdit{
                id: mytext
                anchors.centerIn: parent
                readOnly: true
                selectByMouse :true
                selectByKeyboard :true
                onWidthChanged: {
                    var temp = root.width-avatar.width-nick.anchors.leftMargin-70
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
