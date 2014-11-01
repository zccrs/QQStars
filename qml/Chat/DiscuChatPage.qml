import QtQuick 2.2
import utility 1.0

ChatPage{
    id: root
    myinfo: myuin!=""?myqq.createDiscuInfo(myuin):null
}
