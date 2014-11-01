import QQItemInfo 1.0
import QtQuick 2.2

FriendInfo{
    id: root
    
    property bool getImageing: false//记录是否正在获取图片
    
    userQQ: myqq.userQQ
    onUinChanged: {//如果uin变了
        myqq.getFriendQQ( uin, getQQFinished )//获取好友的真实qq号
    }
    
    function getUserDataFinished(error, data){//获取资料完成
        if(error){//如果出错了
            myqq.getUserData(uin, getUserDataFinished)//去获取好友资料
            return
        }
        
        data = JSON.parse(data)
        if( data.retcode==0 ){
            nick = data.result.nick//设置昵称
        }
    }

    function getQQFinished(error, data){//获取好友真实qq后调用的函数
        //console.log(root.uin+"获得真实QQ结束："+data)
        if(error){
            myqq.getFriendQQ( uin, getQQFinished )//获取好友的真实qq号
            return
        }
        
        data = JSON.parse(data)
        if( data.retcode==0 ){
            account = data.result.account
            if( avatar40=="qrc:/images/avatar.png" )
                getAvatar(40)//去获取头像
        }
    }
    function getAvatarFinished( error, path ,name){
        getImageing = false//获取图像结束
        if(error){//如果出错了
            getAvatar(40)//重新获取
            return
        }

        var imageName = path+"/"+name
        avatar40 = imageName//保存自己头像的地址
        //console.log(nick+"获取头像完成："+imageName)
    }
    function getQQSignatureFinished(error, data){//获取个性签名完成
        if(error){
            myqq.getQQSignature( uin, getQQSignatureFinished )//获取个性签名
            return
        }
        
        data = JSON.parse(data)
        if( data.retcode==0 ){
            QQSignature = data.result[0].lnick//储存个性签名
        }
    }
    onHttpGetQQSignature:{//如果收到c++端发来的去获取个性签名的信号
        myqq.getQQSignature( uin, getQQSignatureFinished )//获取个性签名
    }
    function getAvatar(size){
        if(account!=""&&!getImageing){
            getImageing = true//正在获取头像
            myqq.downloadImage(QQItemInfo.Friend
                               , "http://q.qlogo.cn/headimg_dl?spec="+String(size)+"&dst_uin="+account
                               , account, String(size), getAvatarFinished)//下载头像
        }
    }
   
    onAvatar40Changed: {
        if( avatar40=="qrc:/images/avatar.png" )
            getAvatar(40)
    }
}
