import QtQuick 2.2
import utility 1.0
import QQItemInfo 1.0
import mywindow 1.0
import "../Utility"

ChatPage{
    id: root
    myinfo: myuin!=""?myqq.createDiscuInfo(myuin):null
    
    onMyinfoChanged: {
        if(!myinfo)
            return
        
        if(myinfo.membersCount>0){//如果成员列表已经存在就直接添加到list中
            for(var i=0;i<myinfo.membersCount;++i){
                mymodel.append({"obj_info": myinfo.getMemberInfoByIndex(i)})
            }
        }else if(myinfo.uin!=""){//去获取群列表
            myqq.getDiscusMemberList(getMembersListFinished, myinfo.uin)
        }
    }
    
    function getMembersListFinished(error, data){//获取群成员列表完成
        if(error){
            myqq.getDiscusMemberList(getMembersListFinished, myinfo.uin)//如果出错就再次获取
            console.debug("获取讨论组成员列表出错："+data)
            return
        }
        
        data = JSON.parse(data)
        if( data.retcode == 0 ){
            var members = data.result.mem_info

            for(var j in members){
                //console.debug(members[j].uin+","+members[j].nick)
                var info = myqq.createFriendInfo(members[j].uin)
                //console.debug(members[j].uin+","+members[j].nick)
                info.nick = members[j].nick//储存讨论组成员昵称
                //console.debug(members[j].uin+","+members[j].nick)
                root.myinfo.addMember(info)//将讨论组成员添加进去
                //console.debug(members[j].uin+","+members[j].nick)
            }
            
            members = data.result.mem_status
            for( j in members ){//设置讨论组成员的在线状态
                info = myqq.createFriendInfo(members[j].uin)
                info.stateToString = members[j].status
            }
        }
    }
    
    Rectangle{
        parent: rightBar
        anchors.fill: parent
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
                height: list_member.contentHeight+10
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
                            grayscale: item_root.myinfo.state==FriendInfo.Offline
                            maskSource: "qrc:/images/bit.bmp"
                            source: item_root.myinfo.avatar40
                        }
                        Text{
                            anchors.verticalCenter: image_avatar.verticalCenter
                            anchors.left: image_avatar.right
                            anchors.leftMargin: 5
                            anchors.right: parent.right
                            elide:Text.ElideRight
                            text: item_root.myinfo.aliasOrNick+"("+item_root.myinfo.account+")"
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
