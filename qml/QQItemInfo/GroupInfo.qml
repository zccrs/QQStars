import QtQuick 2.2
import QQItemInfo 1.0

GroupInfo{
    userQQ: myqq.userQQ
    property string code
    function getQQFinished(error, data){//获取真实群号后调用的函数
        if(error){
            myqq.getFriendQQ(code, getQQFinished)
            return
        }

        data = JSON.parse(data)
        if( data.retcode==0 ){
            account = data.result.account
            account = account
            if( avatar40=="qrc:/images/avatar.png" )//如果头像不存在
                myqq.downloadImage("http://p.qlogo.cn/gh/"+account+"/"+account+"/40", "group_"+uin, "40", getAvatarFinished)//下载头像
        }
    }
    function getAvatarFinished( path ,name){
        var imageName = path+"/"+name+".png"
        avatar40 = imageName
    }

    Component.onCompleted: {
        if(account==""){
            myqq.getFriendQQ(code, getQQFinished)
        }else{
            if( avatar40=="qrc:/images/avatar.png" )//如果头像不存在
                myqq.downloadImage("http://p.qlogo.cn/gh/"+account+"/"+account+"/40", "group_"+uin, "40", getAvatarFinished)//下载头像
        }
    }
}
