import QtQuick 2.2
import mywindow 1.0
import utility 1.0
import "../"
import "../../Utility"
import QQItemInfo 1.0

Item{
    id: root
    clip: true
    
    function getGroupListFinished( error, data ) {
        if(error){
            myqq.getGroupList(getGroupListFinished) //获取群列表
            return
        }
        
        data = JSON.parse(data)
        if(data.retcode ==0 ) {
            var groupmarknames = data.result.gmarklist//群备注信息
            var i=0;
            
            for( i=0; i<groupmarknames.length;++i ) {
                var obj_info = myqq.createGroupInfo(groupmarknames[i].uin)
                if(obj_info){
                    obj_info.alias = groupmarknames[i].markname//储存备注信息
                }
            }

            var list_info = data.result.gnamelist
            for( i=0; i< list_info.length;++i ) {
                var info = myqq.createGroupInfo(list_info[i].gid)
                info.nick = list_info[i].name
                info.code = list_info[i].code
                mymodel.append({"obj_info": info})
            }
        }
    }
    Component.onCompleted: {
        myqq.getGroupList(getGroupListFinished)
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
            property var myinfo: obj_info

            MyImage{
                id: avatar
                x:10
                width:40
                height: 40
                sourceSize.width: width
                maskSource: "qrc:/images/bit.bmp"
                cache: false
                source: myinfo.avatar40
                onLoadError: {
                    item_root.myinfo.avatar40 = "qrc:/images/avatar.png"
                }
            }
            Text{
                id:text_nick
                anchors.top: avatar.top
                anchors.left: avatar.right
                anchors.leftMargin: 10
                font.pointSize: 14
                text: myinfo.aliasOrNick
            }
            MouseArea{
                anchors.fill: parent
                onDoubleClicked: {
                    myqq.addChatPage(item_root.myinfo.uin, QQItemInfo.Group)//打开聊天页
                }
            }
        }
    }    
}
