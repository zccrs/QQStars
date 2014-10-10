import QtQuick 2.2
import mywindow 1.0
import utility 1.0
import "../"
import "../../Utility"
import QQItemInfo 1.0

Item{
    id: friendlist_main
    width: parent.width
    height: parent.height
    clip:true
    function addModel( name, index ,obj_friendListData){
        mymodel.append({"obj_groupingName": name, "obj_groupingIndex": index, "obj_friendListData":obj_friendListData})
    }

    function getFriendListFinished(error, data) {//获取好友列表完成
        if(error){
            myqq.getFriendList(getFriendListFinished) //获取好友列表
            return
        }

        data = JSON.parse(data)
        if( data.retcode == 0) {
            var marknames = data.result.marknames//备注信息
            
            for( var i=0; i<marknames.length;++i ) {
                var newObject = myqq.createFriendInfo(marknames[i].uin)
                if(newObject!=null)
                    newObject.alias = marknames[i].markname//储存备注信息
            }
            var categories = data.result.categories//分组信息
            for(i=0; i<categories.length;++i){
                addModel(categories[i].name, categories[i].index, data.result)//增加分组
                //console.log("增加了分组："+categories[i].name)
            }
        }
    }
    Component.onCompleted: {
        myqq.getFriendList(getFriendListFinished) //获取好友列表
    }

    MyScrollView{
        anchors.fill: parent
        Item{
            height: list.contentHeight+10
            width: friendlist_main.width
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
               delegate: component1
            }
        }
    }
    Component{
        id: component1
        Item{
            id: root
            clip: true
            property string name: obj_groupingName
            property int groupingIndex:obj_groupingIndex
            property var friendListData: obj_friendListData
            onFriendListDataChanged: {
                var friends = friendListData.friends//好友列表
                var friends_info = friendListData.info//好友信息
                for( var i=0; i< friends.length;++i ) {
                    if( friends[i].categories==groupingIndex ){//判断是否是本分组的内容
                        mymodel2.append({"obj_friends": friends[i], "obj_info": friends_info[i]})//增加好友 
                    }
                }
            }

            property alias model: mymodel2
            height: titleBar.height
            width: parent.width
            
            state: "close"
            function stateSwitch(){
                if( state=="close" )
                    state = "unfold"
                else
                    state="close"
            }
        
            states: [
                State {
                    name: "close"
                    PropertyChanges {
                        target: root
                        height: titleBar.height
                    }
                },
                State {
                    name: "unfold"
                    PropertyChanges {
                        target: root
                        height: titleBar.height+list2.contentHeight+10
                    }
                }
            ]
            
            Item{
                id: titleBar
                width: parent.width
                height: Math.max(image_icon.implicitHeight, text_name.implicitHeight)
                Text{
                    id: image_icon
                    x:10
                    anchors.verticalCenter: text_name.verticalCenter
                    text: root.state == "close"?"+":"-"
                    font.pointSize: 16
                }
            
                Text{
                    id: text_name
                    text: name
                    anchors.left: image_icon.right
                    anchors.leftMargin: 10
                    font.pointSize: 10
                    font.bold: true
                }
                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        root.stateSwitch()
                    }
                }
            }
        
            ListView{
                id: list2
                interactive: false
                model: ListModel{
                    id:mymodel2
                }
                spacing: 10
                delegate: component2
                anchors.top: titleBar.bottom
                anchors.topMargin: 10
                width: parent.width
                height: parent.height
            }
        }
    }
    Component{
        id: component2
        Item{
            width: parent.width
            height: avatar.height
            property var friends: obj_friends
            property var myinfo: myqq.createFriendInfo(obj_info.uin)
            Component.onCompleted: {
                myinfo.nick = obj_info.nick
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
                text: myinfo.aliasOrNick//myqq.value(info.uin+"alias", info.nick)
            }
            Text{
                id:text_signature//个性签名
                anchors.left: text_nick.left
                anchors.bottom: avatar.bottom
                font.pointSize: 8
                text: myinfo.QQSignature//myqq.value(info.uin+"signature", "获取中...")
            }
            MouseArea{
                anchors.fill: parent
                onDoubleClicked: {
                    chat_command.addChat(myinfo.uin, FriendInfo.Friend)
                }
            }
        }
    }
}
