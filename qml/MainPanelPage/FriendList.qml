import QtQuick 2.2
import mywindow 1.0

Item{
    id: friendlist_main
    width: parent.width
    height: parent.height
    clip:true
    function addModel( name, index ,obj_friendListData){
        mymodel.append({"obj_groupingName": name, "obj_groupingIndex": index, "obj_friendListData":obj_friendListData})
    }

    function getFriendListFinished(data) {//获取好友列表完成
        data = JSON.parse(data)
        if( data.retcode == 0) {
            var marknames = data.result.marknames//备注信息
            for( var i=0; i<marknames.length;++i ) {
                utility.setValue(marknames[i].uin+"alias", marknames[i].markname)//储存备注信息
            }
            var categories = data.result.categories//分组信息
            for(i=0; i<categories.length;++i){
                addModel(categories[i].name, categories[i].index, data.result)//增加分组
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
                    if( friends[i].categories==groupingIndex ){
                        utility.setValue(friends_info[i].uin+"nick", friends_info[i].nick )//保存昵称
                        mymodel2.append({"obj_friends": friends[i], "obj_info": friends_info[i]})//增加好友 
                    }
                }
            }

            property alias model: mymodel2
            height: text_name.implicitHeight
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
                        height: text_name.implicitHeight
                    }
                },
                State {
                    name: "unfold"
                    PropertyChanges {
                        target: root
                        height: text_name.implicitHeight+list2.contentHeight+10
                    }
                }
            ]
            
            Text{
                id: image_icon
                x:10
                anchors.verticalCenter: text_name.verticalCenter
                text: root.state == "close"?"+":"-"
                font.pointSize: 16
                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        root.stateSwitch()
                    }
                }
            }
        
            Text{
                id: text_name
                text: name
                anchors.left: image_icon.right
                anchors.leftMargin: 10
                font.pointSize: 10
                font.bold: true
            }
        
            ListView{
                id: list2
                interactive: false
                model: ListModel{
                    id:mymodel2
                }
                spacing: 10
                delegate: component2
                anchors.top: text_name.bottom
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
            property var info: obj_info
            property string account: utility.getValue(info.uin+"account", "")//真实qq号
            
            function getQQFinished(data){//获取好友真实qq后调用的函数
                data = JSON.parse(data)
                if( data.retcode==0 ){
                    account = data.result.account
                    utility.setValue(info.uin+"account", account)//保存真实qq
                    if( avatar.source=="qrc:/images/avatar.png" ){
                        myqq.downloadImage("http://q.qlogo.cn/headimg_dl?spec=40&dst_uin="+account, account, "40", getAvatarFinished)//下载头像
                        console.log(account+"的头像不存在:"+utility.getValue(info.uin+"avatar-40", "qrc:/images/avatar.png"))
                    }
                }
            }
            function getAvatarFinished( path ,name){
                var imageName = path+"/"+name+".png"
                utility.setValue(info.uin+name, imageName)//保存自己头像的地址
                avatar.source = imageName
            }
            function getQQSignatureFinished(data){//获取个性签名完成
                data = JSON.parse(data)
                if( data.retcode==0 ){
                    text_signature.text = data.result[0].lnick//显示个性签名
                    utility.setValue(info.uin+"signature", text_signature.text)
                }
            }

            Component.onCompleted: {
                if(account!=""){
                    if( avatar.source=="qrc:/images/avatar.png" ){
                        myqq.downloadImage("http://q.qlogo.cn/headimg_dl?spec=40&dst_uin="+account, account, "40", getAvatarFinished)//下载头像
                    }
                }else{
                    myqq.getFriendQQ( info.uin, getQQFinished )//获取好友的真实qq号
                }
                if( text_signature.text=="获取中..." ){
                    text_signature.text=""
                    myqq.getQQSignature( info.uin, getQQSignatureFinished )//获取个性签名
                }
            }

            MyImage{
                id: avatar
                x:10
                width:40
                maskSource: "qrc:/images/bit.bmp"
                source: utility.getValue(info.uin+"avatar-40", "qrc:/images/avatar.png")
                onLoadError: {
                    avatar.source = "qrc:/images/avatar.png"
                }
            }
            Text{
                id:text_nick
                anchors.top: avatar.top
                anchors.left: avatar.right
                anchors.leftMargin: 10
                font.pointSize: 14
                text: utility.getValue(info.uin+"alias", info.nick)
            }
            Text{
                id:text_signature//个性签名
                anchors.left: text_nick.left
                anchors.bottom: avatar.bottom
                font.pointSize: 8
                text: utility.getValue(info.uin+"signature", "获取中...")
            }
        }
    }
}
