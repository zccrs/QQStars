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
    function addModel( name, index ,obj_friendListData ){
        mymodel.append({"obj_groupingName": name, "obj_groupingIndex": index, "obj_friendListData":obj_friendListData})
    }
    function insertModel( sort, name, index ,obj_friendListData ){
        mymodel.insert(sort, {"obj_groupingName": name, "obj_groupingIndex": index, "obj_friendListData":obj_friendListData})
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
                var obj_info = myqq.createFriendInfo(marknames[i].uin)
                if(obj_info){
                    obj_info.alias = marknames[i].markname//储存备注信息
                }
            }
            for( i=0; i< data.result.friends.length;++i ) {
                myqq.addFriendUin(data.result.friends[i].uin)
                //将所有好友的uin都添加进去，为判断一个uin是否为陌生人做基础
            }
            
            var categories = data.result.categories//分组信息
            if(categories.length>0&&categories[0].index>0)//如果分组数目大于0，但是第一个分组的index不为0
                addModel("我的好友", 0, data.result)//则将默认的"我的好友"分组加进去
            var arr = new Array
            for(i=0; i<categories.length;++i){
                arr.push(categories[i])//现将每个对象都放到数组当中
            }
            arr.sort(function(a,b){
                //console.log("调用了排序,"+(a.sort>b.sort))
                return a.sort>b.sort?1:-1//将数组按照里边的sort属性排序
            })

            for(i=0; i<arr.length; ++i){//遍历数组
                addModel(arr[i].name, arr[i].index, data.result)//增加分组
            }
        }else{
            console.debug("好友列表获取失败："+data.retcode)
        }
    }
    function getOnlineFriendsFinished(error, data){//获取在线好友完成
        if(error){
            myqq.getOnlineFriends(getOnlineFriendsFinished)//再去获取
        }
        data = JSON.parse(data)
        if(data.retcode == 0){
            for(var i in data.result){
                var info = myqq.createFriendInfo(data.result[i].uin)
                info.stateToString = data.result[i].status//设置状态
            }
        }
    }

    Component.onCompleted: {
        myqq.getOnlineFriends(getOnlineFriendsFinished)//获取在线好友
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
                        var info = myqq.createFriendInfo(friends_info[i].uin)
                        info.nick = friends_info[i].nick//设置昵称
                        var data = {"obj_info": info}
                        mymodel2.append(data)//增加好友 
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
                    
                    property int onlineIndex: 0//在线好友的序列，初始为0
                    
                    function stateToOnline(index){
                        //当序列为index的model项调用此函数后，会将他前移到onlineIndex尾部
                        if(index>=onlineIndex)
                            move(index, onlineIndex++, 1)//移动此项
                    }
                    function stateToOffline(index){
                        //当序列为index的model项调用此函数后，会将他前移到尾部
                        if(index<=count-1){
                            --onlineIndex
                            move(index, count-1, 1)
                        }
                    }
                }
                spacing: 10
                delegate: component2
                anchors.top: titleBar.bottom
                anchors.topMargin: 10
                width: parent.width
                height: parent.height
            }
            Component{
                id: component2
                Item{
                    id: item_root
                    width: parent.width
                    height: avatar.height
                    property FriendInfo myinfo: obj_info
                    
                    Component.onCompleted: {
                        if(myinfo.state != FriendInfo.Offline){//如果不是离线状态
                            mymodel2.stateToOnline(index)//将自己移动到前面
                        }
                    }
                    Connections{
                        target: myinfo
                        onStateChanged:{//如果状态改变
                            if(myinfo.state == FriendInfo.Offline){//如果是离线状态
                                mymodel2.stateToOffline(index)//将自己移动到最后
                            }else{
                                mymodel2.stateToOnline(index)//否则就往上移动
                            }
                        }
                    }
        
                    MyImage{
                        id: avatar
                        x:10
                        width:40
                        height: 40
                        sourceSize.width: width
                        grayscale: myinfo.state==FriendInfo.Offline
                        maskSource: "qrc:/images/bit.bmp"
                        cache: false
                        source: item_root.myinfo.avatar40
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
                            myqq.addChatPage(myinfo.uin, QQItemInfo.Friend)
                        }
                    }
                }
            }
        }
    }
}
