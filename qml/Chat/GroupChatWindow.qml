import QtQuick 2.2
import utility 1.0

ChatWindow{
    id: root
    myinfo: myqq.createGroupInfo(myuin)

    Text{
        id: show_text
        parent: menuBar
        verticalAlignment: Text.AlignVCenter
        anchors.verticalCenter: parent.verticalCenter
        text: {
            if(myinfo)
                return myinfo.aliasOrNick
            else
                return ""
        }
    }
}
