import QtQuick 2.2
import mywindow 1.0
import "../"
import "../../QQItemInfo"

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
            property var info: obj_info
            //property string account: myinfo.account//myqq.value(info.uin+"account", "")//真实qq号
            
            RecentInfo{
                id: myinfo
                uin: info.uin
                type: info.type
            }

            /*onInfoChanged: {
                if(myqq.value(info.uin+"nick", "")==""){
                    myqq.getFriendInfo( info.uin, getFriendInfoFinished )//获取昵称
                    myqq.getFriendQQ( info.uin, getQQFinished )//获取真实qq号
                }
            }
            function getAvatarFinished( path ,name ){
                var imageName = path+"/"+name+".png"
                myqq.setValue(info.uin+name, imageName)//保存自己头像的地址
                avatar.source = imageName
            }
            function getQQFinished(error, data){//获取好友真实qq后调用的函数
                if(error){
                    myqq.getFriendQQ( info.uin, getQQFinished )//获取好友的真实qq号
                    return
                }
                
                data = JSON.parse(data)
                if( data.retcode==0 ){
                    account = data.result.account
                    myqq.setValue(info.uin+"account", account)//保存真实qq
                    if( avatar.source=="qrc:/images/avatar.png" ){ //如果头像不存在
                        if(info.type==0)
                            myqq.downloadImage("http://q.qlogo.cn/headimg_dl?spec=40&dst_uin="+account, "friend_"+info.uin, "40", getAvatarFinished)//下载头像
                        else
                            myqq.downloadImage("http://p.qlogo.cn/gh/"+account+"/"+account+"/40", "group_"+info.uin, "40", getAvatarFinished)//下载头像
                    }
                }
            }
            function getFriendInfoFinished( error, data ) {
                if(error){
                    myqq.getFriendInfo( info.uin, getFriendInfoFinished )//获取好友资料，从中提取昵称
                    return
                }
                
                data = JSON.parse(data)
                if(data.retcode==0) {
                    data = data.result
                    myqq.setValue(info.uin+"nick", data.nick)
                    text_nick.text = data.nick//显示昵称
                }
            }
            Component.onCompleted: {
                if(account!=""){
                    if( avatar.source=="qrc:/images/avatar.png" ){
                        if(info.type==0)
                            myqq.downloadImage("http://q.qlogo.cn/headimg_dl?spec=40&dst_uin="+account, "friend_"+info.uin, "40", getAvatarFinished)//下载头像
                        else
                            myqq.downloadImage("http://p.qlogo.cn/gh/"+account+"/"+account+"/40", "group_"+info.uin, "40", getAvatarFinished)//下载头像
                    }
                }else{
                    myqq.getFriendQQ( info.uin, getQQFinished )//获取好友的真实qq号
                }
            }*/
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
                text: myinfo.aliasOrNick//myqq.value(info.uin+"alias", myqq.value(info.uin+"nick", ""))
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
