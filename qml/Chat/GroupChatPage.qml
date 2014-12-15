import QtQuick 2.2
import utility 1.0
import mywindow 1.0
import QQItemInfo 1.0
import "../Utility"

ChatPage{
    id: root
    myinfo: myuin!=""?myqq.createGroupInfo(myuin):null
    
    onMyinfoChanged: {
        if(!myinfo)
            return
        
        if(myinfo.membersCount>0){//如果成员列表已经存在就直接添加到list中
            for(var i=0;i<myinfo.membersCount;++i){
                mymodel.append({"obj_info": myinfo.getMemberInfoByIndex(i)})
            }
        }else if(myinfo.code!=""){//去获取群列表
            myqq.getGroupMembersList(getMembersListFinished, myinfo.code)
        }
    }
    function getMembersListFinished(error, data){//获取群成员列表完成
        if(error){
            myqq.getGroupMembersList(getMembersListFinished, myinfo.code)//如果出错就再次获取
            console.debug("获取群成员列表出错："+data)
            return
        }

        data = JSON.parse(data)
        if( data.retcode == 0 ){
            myinfo.announcement = data.result.ginfo.memo//设置群公告
            
            var members = data.result.cards
            for ( var i in members ){
                root.myinfo.setMemberCard(members[i].muin, members[i].card)//储存群名片
            }
            
            members = data.result.minfo
            for(var j in members){
                var info = myqq.createFriendInfo(members[j].uin)
                info.nick = members[j].nick//储存群成员昵称
                root.myinfo.addMember(info)//将群成员添加进去
            }
            
            members = data.result.stats
            for(j in members){//设置群成员的在线状态
                info = myqq.createFriendInfo(members[j].uin)
                switch(members[j].stat){
                case 10://在线
                    info.state = FriendInfo.Online
                    break;
                case 60://Q我吧
                    info.state = FriendInfo.Callme
                    break;
                case 30://离开
                    info.state = FriendInfo.Away
                    break;
                case 40://隐身
                    info.state = FriendInfo.Hidden
                    break;
                case 50://忙碌
                    info.state = FriendInfo.Busy
                    break;
                case 70://请勿打扰
                    info.state = FriendInfo.Silent
                    break;
                default: break;
                }
            }
        }
    }
    
    Item{
        parent: rightBar
        anchors.fill: parent
        
        Rectangle{
            id: group_announcement//群公告
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            height: width
            anchors.margins: 10
            anchors.leftMargin: 0
            radius: 5
            color: "#f5f5f5"
            border.width: 2
            border.color: "#ddd"
            
            Text{
                id: text_announcement
                text: myinfo.announcement//群公告
                anchors.fill: parent
                anchors.margins: 10
                wrapMode: Text.WordWrap
                elide:Text.ElideRight
            }
        }
        
        Rectangle{
            anchors.top: group_announcement.bottom
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: 10
            anchors.leftMargin: 0
            radius: 5
            color: "#f5f5f5"
            border.width: 2
            border.color: "#ddd"
            
            MyScrollView{
                id: scroll_list
                anchors.fill: parent
    
                Item{
                    height: mymodel.count*30+10
                    width: scroll_list.width-25
                    x:5
                    implicitHeight: height
                    implicitWidth: width

                    ListView{
                        id: list_member
                        anchors.fill: parent
                        
                        model: ListModel{
                            id: mymodel
                        }
                        delegate: Item{
                            id: item_root
                            property FriendInfo myinfo: obj_info
                            
                            height: 30
                            width: parent.width
                            MyImage{
                                id: image_avatar
                                width: 24
                                height: 24
                                sourceSize.width: width
                                anchors.verticalCenter: parent.verticalCenter
                                maskSource: "qrc:/images/bit.bmp"
                                grayscale: item_root.myinfo.state==FriendInfo.Offline
                                source: item_root.myinfo.avatar40
                            }
                            Text{
                                anchors.verticalCenter: image_avatar.verticalCenter
                                anchors.left: image_avatar.right
                                anchors.leftMargin: 5
                                anchors.right: parent.right
                                elide:Text.ElideRight
                                text: root.myinfo.getMemberCardByUin(
                                          item_root.myinfo.uin, 
                                          item_root.myinfo.aliasOrNick)+
                                      "("+item_root.myinfo.account+")"
                            }
                            MouseArea{
                                anchors.fill: parent
                                onDoubleClicked: {
                                    myqq.addChatPage(item_root.myinfo.uin, QQItemInfo.Friend)//双击开始聊天
                                }
                            }
                        }
                    }
                    Connections{
                        target: root.myinfo
                        onMemberIncrease:{//增加item
                            mymodel.append({"obj_info": info})
                        }
                        onMemberReduce:{//移除item
                            mymodel.remove(index)
                        }
                    }
                }
            }
        }
    }
}
