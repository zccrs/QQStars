import QtQuick 2.2
import QQItemInfo 1.0

GroupInfo{
    userQQ: myqq.userQQ
    onCodeChanged: {
        //console.log(nick+"将要获取真实qq")
        if(code!="")
            myqq.getFriendQQ(code, getQQFinished)//获得真实qq
    }
    
    onAccountChanged: {
        if(account==""&&code!="")
            myqq.getFriendQQ(nick, getQQFinished)//获得真实qq
    }

    function getQQFinished(error, data){//获取真实群号后调用的函数
        if(error){
            myqq.getFriendQQ(code, getQQFinished)
            return
        }

        data = JSON.parse(data)
        if( data.retcode==0 ){
            account = data.result.account
            //console.log(code+"的真实qq："+account)
            if(avatar40=="qrc:/images/avatar.png")
                getAvatar(40)
        }
    }
    function getAvatarFinished( path ,name){
        var imageName = path+"/"+name+".png"
        avatar40 = imageName
    }
    function getAvatar(size){
        if(account!=""){
            myqq.downloadImage(QQItemInfo.Group
                               , "http://p.qlogo.cn/gh/"+account+"/"+account+"/"+String(size)
                               , account, String(size), getAvatarFinished)//下载头像
        }
    }
    onAvatar40Changed: {
        if(avatar40=="qrc:/images/avatar.png")
            getAvatar(40)
    }
}
