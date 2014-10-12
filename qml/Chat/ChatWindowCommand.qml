import QtQuick 2.2
import utility 1.0
import mywindow 1.0
import QQItemInfo 1.0
import "../Utility"

MyWindow{
    id: root
    minHeight: 500
    minWidth: 600+left_bar.width
    visible: true
    onVisibleChanged: {
        if(visible)
            root.showFront()//显示到最屏幕最前端
    }

    noBorder: true//无边框的
    removable: true//可移动的
    fixedSize: false//固定大小的
    dockableWindow: false//可停靠的
    topHint: false//窗口保持在最前端
    noNotifyIcon:false//隐藏任务栏图标
    color: "transparent"
    windowGlowItem.color: "black"//"#f07000"
    property ChatWindow currentShowPage//记录当前显示中的聊天页面
    
    function setCurrentShowPage(page){
        //console.log(page+","+currentShowPage)
        if(currentShowPage!==page&&page){//判断是否page就是当前活动页面
            page.visible = true
            if(currentShowPage)
                currentShowPage.visible = false//先将旧的page设置为隐藏
            currentShowPage = page//
            //console.log(page+","+currentShowPage)
        }
    }

    Item{
        objectName: "ChatWindowCommandItem"
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.left: left_bar.right
    }
    Connections{
        target: myqq
        onAddChatPage:{
            setCurrentShowPage(item)//设置当前显示的页面为item
            left_bar.addItem(item)//增加左栏
        }
        onActiveChatPageChanged:{//如果活跃的Page改变为item
            setCurrentShowPage(item)//设置当前显示的页面为item
        }
    }
    Rectangle{
        anchors.left: left_bar.right
        color: "#eee"
        width: left_bar.radius
        height: width
        visible: left_bar.width>0
    }
    Rectangle{
        anchors.bottom: left_bar.bottom
        anchors.left: left_bar.right
        color: "#eee"
        width: left_bar.radius
        height: width
        visible: left_bar.width>0
    }
    SvgView{
        id:image_quit_icon
        width: defaultSize.width*myqq.windowScale
        source: "qrc:/images/button-quit.svg"
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.margins: 10
        MouseArea{
            anchors.fill: parent
            onClicked: {
                root.close()
            }
        }
    }
    SvgView{
        id:image_minimize_icon
        width: defaultSize.width*myqq.windowScale
        source: "qrc:/images/button-minimize.svg"
        anchors.top: image_quit_icon.top
        anchors.left: image_quit_icon.right
        MouseArea{
            anchors.fill: parent
            onClicked: {
                root.showMinimized()
            }
        }
    }
    
    Rectangle{
        id:left_bar
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        color:"#ccc"
        radius:10
        
        Behavior on width{
            NumberAnimation{
                duration: 500
            }
        }

        function addItem(item){
            var data={
                "obj_item": item
            }
            mymodel.append(data)//增加条目
        }
        
        Rectangle{
            anchors.right: parent.right
            color: parent.color
            width: parent.radius
            height: width
            visible: left_bar.width>0
        }
        Rectangle{
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            color: parent.color
            width: parent.radius
            height: width
            visible: left_bar.width>0
        }

        MyScrollView{
            anchors.fill: parent
            Item{
                height: list.contentHeight+10
                width: left_bar.width
                implicitHeight: height
                implicitWidth: width
                ListView{
                   id: list
                   interactive: false
                   anchors.fill: parent
                   anchors.margins: 5
                   model: ListModel{
                       id:mymodel
                       onCountChanged: {
                           console.log(count)
                           if(count == 1)//如果没有了Item就让左栏的width为0
                               left_bar.width = 0
                           else if(count==2){
                               left_bar.width=150
                           }
                       }
                   }
                   spacing :10
                   delegate: component
                }
            }
        }
        
        Component{
            id: component
            Item{
                id: item_root
                width: parent.width
                height: avatar.height
                property ChatWindow my: obj_item
                Rectangle{
                    id: rect_hover
                    width: parent.width+10
                    height: parent.height+10
                    anchors.centerIn: parent
                    color: root.currentShowPage==my?"#f07000":"#eee"
                    radius: 5
                    opacity: 0.8
                    visible: item_mouse.hovered||root.currentShowPage==my
                }

                MouseArea{
                    id: item_mouse
                    anchors.fill: parent
                    hoverEnabled: true
                    property bool hovered
                    onEntered: {
                        hovered = true
                    }
                    onExited: {
                        hovered = false
                    }
                    onClicked: {
                        setCurrentShowPage(my)//将自己设置为活跃页面
                    }
                }
                
                MyImage{
                    id: avatar
                    x:10
                    width:35
                    source: my.myinfo.avatar40
                    maskSource: "qrc:/images/bit.bmp"
                }
                Text{
                    id:text_nick
                    anchors.verticalCenter: avatar.verticalCenter
                    anchors.left: avatar.right
                    anchors.leftMargin: 10
                    //font.pointSize: 14
                    text: my.myinfo.aliasOrNick
                }
                
                SvgView{
                    id:image_clost_page
                    width: defaultSize.width*myqq.windowScale
                    source: "qrc:/images/button-quit.svg"
                    anchors.right: rect_hover.right
                    anchors.rightMargin: 5
                    anchors.verticalCenter: parent.verticalCenter
                    visible: item_mouse.hovered
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            //setCurrentShowPage(list.children[0].my)//设置可见活跃为第一个
                            mymodel.remove(index)//移除自己
                            myqq.removeChatPage(my.myuin, my.type)//移除聊天页面
                        }
                    }
                }
            }
        }
    }
}
