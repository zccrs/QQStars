import QtQuick 2.2
import QQItemInfo 1.0

RecentInfo{
    property int type: RecentInfo.Friend
    userQQ: myqq.userQQ
    
    /*onInfoChanged: {
        if(myqq.value(info.uin+"nick", "")==""){
            myqq.getFriendInfo( info.uin, getFriendInfoFinished )//获取昵称
            myqq.getFriendQQ( info.uin, getQQFinished )//获取真实qq号
        }
    }*/
    onNickChanged:{
        if(nick==""){
            myqq.getFriendInfo( info.uin, getFriendInfoFinished )//获取昵称
        }
    }

    function getAvatarFinished( path ,name ){
        var imageName = path+"/"+name+".png"
        avatar40 = imageName
    }
    function getQQFinished(error, data){//获取好友真实qq后调用的函数
        if(error){
            myqq.getFriendQQ( uin, getQQFinished )//获取好友的真实qq号
            return
        }
        
        data = JSON.parse(data)
        if( data.retcode==0 ){
            account = data.result.account
        }
    }
    function getFriendInfoFinished( error, data ) {
        if(error){
            myqq.getFriendInfo( uin, getFriendInfoFinished )//获取好友资料，从中提取昵称
            return
        }
        
        data = JSON.parse(data)
        if(data.retcode==0) {
            data = data.result
            nick = data.nick
        }
    }
    onAccountChanged:{
        if(account!=""){
            if( avatar40=="qrc:/images/avatar.png" ){
                if(type==RecentInfo.Friend)
                    myqq.downloadImage("http://q.qlogo.cn/headimg_dl?spec=40&dst_uin="+account, "friend_"+uin, "40", getAvatarFinished)//下载头像
                else if(type==RecentInfo.Group)
                    myqq.downloadImage("http://p.qlogo.cn/gh/"+account+"/"+account+"/40", "group_"+uin, "40", getAvatarFinished)//下载头像
            }
        }else{
            myqq.getFriendQQ( uin, getQQFinished )//获取好友的真实qq号
        }
    }
}
