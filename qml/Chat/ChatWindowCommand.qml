import QtQuick 2.2
import utility 1.0
import mywindow 1.0
import QQItemInfo 1.0
import "../Utility"

MyWindow{
    id: root
    minimumHeight: 500
    fixedLeftBorder: true//固定左边边框(不能从左边拉动改变窗口大小)
    fixedRightBorder: true//固定右边边框(不能从右边拉动改变窗口大小)
    width: chatPageWidth+left_bar.width
    visible: true
    onVisibleChanged: {
        if(visible)
            root.showFront()//显示到最屏幕最前端
    }
    windowIcon: {
        if(currentShowPage)
            return currentShowPage.myinfo.avatar40
        else
            return "qrc:/images/avatar.png"
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
    property int chatPageWidth: myqq.value("chatPageWidth", 600)//获取聊天页面的width, 初始化为600，聊天也的width
    
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
        id: item_chatPage
        objectName: "ChatWindowCommandItem"
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: left_bar.right
        width: Math.max(chatPageWidth, minWidth)
        property int minWidth: 600
        property int maxWidth: 9999999
        onWidthChanged: {
            console.log(width)
        }

        function setPageWidth(arg){
            if(arg<=maxWidth&&arg>=minWidth){
                chatPageWidth = arg
            }
        }

        MouseArea{//接收窗口页面的鼠标事件
            enabled: !root.fixedSize&&root.windowStatus==MyQuickWindow.StopCenter
            cursorShape :enabled?Qt.SizeHorCursor:Qt.ArrowCursor
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            height: root.height
            width: 2
            property real pressedX: 0
            property real pressedY: 0
            onPressed: {
                pressedX = mouseX
            }
            onPositionChanged: {
                var num_temp = mouseX-pressedX
                item_chatPage.setPageWidth(item_chatPage.width+num_temp)
            }
        }
    }
    Connections{
        target: myqq
        onAddChatPage:{
            setCurrentShowPage(item)//设置当前显示的页面为item
            left_bar.addItem(item)//增加左栏
        }
        onActiveChatPageChanged:{//如果活跃的Page改变为item
            console.log(item)
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
    
    Rectangle{//用来管理当前聊天窗口内聊天页面的左栏
        id:left_bar
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        color:"#ddd"
        radius:10
        property bool isOpen: false//记录此栏是否处于打开状态
        property int maxWidth:200
        property int minWidth:70
        property int defaultWidth: myqq.value("chatWindowLeftBarWidth", 150)//获取上次储存的值
        onDefaultWidthChanged: {
            if(isOpen){//如果是打开状态
                setBarWidth(defaultWidth)//如果默认宽度改变就设置此栏的当前width
            }
        }
        
        NumberAnimation{//动画控件
            id: animation_width
            target: left_bar
            running: false
            duration: 300
            property: "width"
        }

        function openBar(){
            if(!animation_width.running&&(!isOpen)){
                isOpen = true
                animation_width.to = defaultWidth
                animation_width.start()//启动动画
            }
        }
        
        function hideBar(){
            if(!animation_width.running&&isOpen){
                isOpen = false
                animation_width.to = 0
                animation_width.start()//启动动画
            }
        }
        
        function setBarDefaultWidth(arg){//设置默认的width
            if(arg<=maxWidth&&arg>=minWidth){
                defaultWidth = arg
                return true
            }else{
                return false
            }
        }

        function addItem(item){
            var data={
                "obj_item": item
            }
            mymodel.append(data)//增加条目
        }
        function setBarWidth(arg){
            if(arg<=maxWidth&&arg>=minWidth){
                width = arg
            }
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
            anchors.topMargin: image_quit_icon.height+15
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
                           if(count <= 1){//如果没有了Item就让左栏的width为0
                               left_bar.hideBar()//隐藏此栏
                           }else if(count==2){
                               left_bar.openBar()//打开此栏
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
                    source: {
                        if(my!==null)
                            return my.myinfo.avatar40
                        else
                            return ""
                    }

                    maskSource: "qrc:/images/bit.bmp"
                }
                Text{
                    id:text_nick
                    anchors.verticalCenter: avatar.verticalCenter
                    anchors.left: avatar.right
                    anchors.leftMargin: 10
                    //font.pointSize: 14
                    text: {
                        if(my!=null)
                            return my.myinfo.aliasOrNick
                        else
                            return ""
                    }
                }
                Rectangle{
                    width: image_clost_page.width+16
                    height: image_clost_page.height+10
                    anchors.centerIn: image_clost_page
                    color: "#eee"
                    radius: 5
                    opacity: 0.75
                    visible: image_clost_page.visible&&root.currentShowPage!=my
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
                            mymodel.remove(index)//移除自己
                            myqq.removeChatPage(my.myuin, my.type)//移除聊天页面
                        }
                    }
                }
            }
        }
        MouseArea{//接收窗口左部的鼠标事件
            enabled: !root.fixedSize&&root.windowStatus==MyQuickWindow.StopCenter
            cursorShape :enabled?Qt.SizeHorCursor:Qt.ArrowCursor
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            height: root.height
            width: 2
            property real pressedX: 0
            property real pressedY: 0
            onPressed: {
                pressedX = mouseX
            }
            onPositionChanged: {
                var num_temp = pressedX-mouseX
                if(left_bar.setBarDefaultWidth(left_bar.defaultWidth+num_temp)){//如果窗口大小设置成功
                    root.x += mouseX-pressedX//设置窗口位置
                }
            }
        }
        MouseArea{//接收窗口页面的鼠标事件
            enabled: !root.fixedSize&&root.windowStatus==MyQuickWindow.StopCenter
            cursorShape :enabled?Qt.SizeHorCursor:Qt.ArrowCursor
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            height: root.height
            width: 2
            property real pressedX: 0
            property real pressedY: 0
            onPressed: {
                pressedX = mouseX
            }
            onPositionChanged: {
                var num_temp = mouseX-pressedX
                left_bar.setBarDefaultWidth(left_bar.defaultWidth+num_temp)
            }
        }
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
                root.close()//关闭窗口
                mymodel.clear()//清除所有model
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
}
