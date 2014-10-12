import QtQuick 2.2
import mywindow 1.0
import utility 1.0
import "../"
import "../../Utility"
import QQItemInfo 1.0

Item{
    id: root
    clip: true
    
    function getDiscusListFinished(error, data) {//获取讨论组列表完成
        if(error){
            myqq.getDiscusList(getDiscusListFinished) //讨论组列表
            return
        }
        data = JSON.parse(data)
        if(data.retcode ==0 ) {
            var list_info = data.result.dnamelist
            for( var i=0; i< list_info.length;++i ) {
                mymodel.append({"obj_info": list_info[i]})
            }
        }
    }
    Component.onCompleted: {
        myqq.getDiscusList(getDiscusListFinished) //讨论组列表
    }
    MyScrollView{
        anchors.fill: parent
        Item{
            height: list.contentHeight+10
            width: root.width
            implicitHeight: height
            implicitWidth: width
            ListView{
               id: list
               interactive: false
               anchors.fill: parent
               model: ListModel{
                   id:mymodel
               }
               spacing :10
               delegate: component
            }
        }
    }
    
    Component{
        id: component
        Item{
            id: item_root
            width: parent.width
            height: avatar.height
            property var myinfo: myqq.createDiscuInfo(obj_info.did) 
            Component.onCompleted: {
                myinfo.nick = obj_info.name
            }

            MyImage{
                id: avatar
                x:10
                width:40
                source: "qrc:/images/avatar.png"
                maskSource: "qrc:/images/bit.bmp"
            }
            Text{
                id:text_nick
                anchors.top: avatar.top
                anchors.left: avatar.right
                anchors.leftMargin: 10
                font.pointSize: 14
                text: item_root.myinfo.aliasOrNick
            }
            MouseArea{
                anchors.fill: parent
                onDoubleClicked: {
                    myqq.addChatWindow(item_root.myinfo.uin, QQItemInfo.Discu)
                }
            }
        }
    }    
}
