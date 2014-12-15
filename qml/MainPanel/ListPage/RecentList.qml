import QtQuick 2.2
import mywindow 1.0
import QQItemInfo 1.0
import "../"
import "../../QQItemInfo"
import "../../Utility"

Item{
    id: root
    clip:true
    
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
                var info
                switch(data[i].type) {
                case 0://0为好友
                    info=myqq.createFriendInfo(data[i].uin)
                    break
                case 1://1为群
                    info=myqq.createGroupInfo(data[i].uin)
                    break
                case 2://2为讨论组
                    info=myqq.createDiscuInfo(data[i].uin)
                    break
                default:break
                }
                mymodel.addEnd(info)//添加最近联系人
            }
        }
    }
    
    Connections{
        target: myqq
        onAddRecentContacts:{//收到增加最近联系人的信号
            mymodel.addBegin(info)
        }
    }

    MyScrollView{
        anchors.fill: parent
        Item{
            height: list.contentHeight+10
            width: root.width
            implicitHeight: height
            implicitWidth: width
            
            ListView{
               id: list
               
               anchors.fill: parent
               interactive: false
               model: ListModel{
                   id:mymodel
                   function addEnd(item_info){
                       for(var i=0; i<mymodel.count; ++i)
                           if(mymodel.get(i).obj_info == item_info){
                               return
                           }
                       mymodel.append({"obj_info": item_info})
                   }

                   function addBegin(item_info){//判断一个对象是否存在于list中，是的话返回他的index
                       for(var i=0; i<mymodel.count; ++i)
                           //console.debug(mymodel.get(i).obj_info+","+item_info)
                           if(mymodel.get(i).obj_info == item_info){
                               mymodel.move(i,0,1)
                               return
                           }
                       mymodel.append({"obj_info": item_info})
                   }
               }
               //spacing :10
               delegate: component
            }
        }
    }
    Component{
        id: component
        Item{
            id: item_root
            property QQItemInfo myinfo: obj_info
            
            width: parent.width
            height: avatar.height+16
            

            MyImage{
                id: avatar
                x:10
                width:40
                height: 40
                sourceSize.width: width
                anchors.verticalCenter: parent.verticalCenter
                cache: false
                grayscale: myinfo.mytype == QQItemInfo.Friend&&myinfo.state==FriendInfo.Offline//是否为黑白图
                maskSource: "qrc:/images/bit.bmp"
                source: myinfo.avatar40
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
                text: myinfo.aliasOrNick
            }
            Rectangle{
                id: rect_message_count
                width: text_message_count.implicitWidth+10
                height: text_message_count.implicitHeight+10
                anchors.bottom: parent.bottom
                anchors.right: parent.right
                anchors.rightMargin: 15
                anchors.bottomMargin: 5
                color: "red"
                radius: height
                visible: myinfo.unreadMessagesCount>0
                Text{
                    id: text_message_count
                    anchors.centerIn: parent
                    text: myinfo.unreadMessagesCount//未读消息的个数
                    color: "white"
                    onTextChanged: {
                        if(text == "100"){
                            text = "99+"
                        }
                    }
                }
            }
            Rectangle{
                height: 1
                anchors.left: avatar.left
                anchors.right: rect_message_count.right
                anchors.bottom: parent.bottom
                color: "#ddd"
            }

            MouseArea{
                anchors.fill: parent
                onDoubleClicked: {
                    myqq.addChatPage(myinfo.uin, myinfo.mytype)
                }
            }
        }
    }
}
