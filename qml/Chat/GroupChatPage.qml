import QtQuick 2.2
import utility 1.0

ChatPage{
    id: root
    property bool existMembers: false
    myinfo: myuin!=""?myqq.createGroupInfo(myuin):null
    
    onMyinfoChanged: {
        if(!existMembers&&myinfo&&myinfo.code!=""){
            myqq.getGroupMembersList(getMembersListFinished, myinfo.code)
            //console.debug("开始获取群列表")
            //开始获取群列表
        }
    }
    function getMembersListFinished(error, data){//获取群成员列表完成
        if(error){
            myqq.getGroupMembersList(getMembersListFinished, code)//如果出错就再次获取
            console.debug("获取群成员列表出错："+data)
            return
        }
        
        data = JSON.parse(data)
        if( data.retcode == 0 ){
            existMembers = true//记得设置为真，不然每次进群都会获取群成员列表
            var members = data.result.minfo
            //console.debug("群成员列表获取完成1:"+members.length)
            for(var i in members){
                var info = myqq.createFriendInfo(members[i].uin)
                info.nick = members[i].nick//储存昵称
            }
            //console.debug("群成员列表获取完成2")
        }
    }
}
