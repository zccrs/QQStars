import QtQuick 2.2
import mywindow 1.0
import "../"
import "../../QQItemInfo"
import "../../Utility"

Item{
    clip:true
    width: parent.width
    height: parent.height
    Component.onCompleted: {
        myqq.getRecentList(getRecentListFinished) //获取最近联系人
    }
    function getRecentListFinished(error, data){//当获取最近联系人列表完成后
        if(error){
            myqq.getRecentList(getRecentListFinished) //获取最近联系人
            return
        }

        data = JSON.parse(data)
        if( data.retcode==0 ) {
            data = data.result
            for(var i=0;i<data.length;++i){
                addModel(data[i])//添加最近联系人
            }
        }
    }
    function addModel( info ){
        mymodel.append({"obj_info": info})
    }
    

    Component{
        id: component
        Item{
            width: parent.width
            height: avatar.height
            property var myinfo: {
                switch(obj_info.type) {
                case 0://0为好友
                    return myqq.createFriendInfo(obj_info.uin)
                case 1://1为群
                    return myqq.createGroupInfo(obj_info.uin)
                case 2://2为讨论组
                    return myqq.createDiscuInfo(obj_info.uin)
                }
            }
            
            MyImage{
                id: avatar
                x:10
                width:40
                maskSource: "qrc:/images/bit.bmp"
                source: myinfo.avatar40//myqq.value(info.uin+"avatar-40", "qrc:/images/avatar.png")
                onLoadError: {
                    myinfo.avatar40 = "qrc:/images/avatar.png"
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
        }
    }
    MyScrollView{
        anchors.fill: parent
        Item{
            implicitHeight: list.contentHeight+10
            implicitWidth: parent.width
            
            ListView{
               id: list
               anchors.fill: parent
               interactive: false
               model: ListModel{
                   id:mymodel
               }
               spacing :10
               delegate: component
            }
        }
    }
}
