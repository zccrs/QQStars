import QtQuick 2.2
import QQItemInfo 1.0

GroupInfo{
    id: root
    property bool getImageing: false//记录是否正在获取图片
    
    userQQ: myqq.userQQ
    onCodeChanged: {
        //console.log(nick+"将要获取真实qq")
        if(code!=""){
            myqq.getFriendQQ(code, getQQFinished)//获得真实qq
        }
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
    function getAvatarFinished( error, path ,name){
        getImageing=false//将正在获取头像置为false
        if(error){//如果出错了
            getAvatar(40)//重新获取
            return
        }
        var imageName = path+"/"+name
        avatar40 = imageName
    }
    function getAvatar(size){
        if(account!=""&&!getImageing){
            getImageing = true//置为true，不然可能会多次请求头像
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
