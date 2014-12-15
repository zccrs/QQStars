import QtQuick 2.2
import utility 1.0
import mywindow 1.0
import QQItemInfo 1.0
import "../Utility"

MyWindow{
    id: root
    
    property ChatPage currentShowPage//记录当前显示中的聊天页面
    property int chatPageWidth: myqq.value("chatPageWidth", 600)//获取聊天页面的width, 初始化为600，聊天也的width
    
    visible: true
    minimumHeight: 500
    minimumWidth: item_chatPage.minWidth+left_bar.width//设置最小宽度
    width: left_bar.width+item_chatPage.width
    noBorder: true//无边框的
    removable: true//可移动的
    fixedSize: false//固定大小的
    dockableWindow: false//可停靠的
    topHint: false//窗口保持在最前端
    noNotifyIcon:false//隐藏任务栏图标
    color: "transparent"
    windowGlowItem.color: "black"//"#f07000"
    windowIcon: currentShowPage?currentShowPage.myinfo.avatar40:"qrc:/images/avatar.png"
    
    setLeftBorder: function(arg){
        if(left_bar.isOpen&&left_bar.setBarDefaultWidth(left_bar.defaultWidth+arg)){//如果窗口大小设置成功
            root.mySetLeftBorder(arg)//设置窗口位置
        }else if(!left_bar.isOpen){
            if(item_chatPage.setPageWidth(chatPageWidth+arg)){
                root.mySetLeftBorder(arg)
            }
        }
    }
    setRightBorder: function(arg){
        if(item_chatPage.setPageWidth(chatPageWidth+arg)){
            root.mySetRightBorder(arg)
        }
    }
    onVisibleChanged: {
        if(visible)
            root.showFront()//显示到最屏幕最前端
    }
    
    
    function setCurrentShowPage(page){
        //console.log(page+","+currentShowPage)
        if(currentShowPage!==page&&page){//判断是否page就是当前活动页面
            page.myinfo.isActiveChatPage=true//将他信息中是否为活跃页面的属性设置为true
            page.visible = true
            if(currentShowPage){
                currentShowPage.visible = false//先将旧的page设置为隐藏
                currentShowPage.myinfo.isActiveChatPage=false//将他信息中是否为活跃页面的属性设置为false
            }
            currentShowPage = page//
        }
    }

    Item{
        id: item_chatPage
        
        property int maxWidth: 99999
        property int minWidth: 500
        
        objectName: "ChatWindowCommandItem"
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: left_bar.right
        width: chatPageWidth

        function setPageWidth(arg){
            if(arg<=maxWidth&&arg>=minWidth){
                chatPageWidth = arg
                return true
            }else{
                return false
            }
        }
        
        Rectangle{
            anchors.left: parent.left
            color: "#eee"
            width: 10
            height: width
            visible: left_bar.width>0
        }
        Rectangle{
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            color: "#eee"
            width: 10
            height: width
            visible: left_bar.width>0
        }
    }
    Connections{
        target: myqq
        onAddChatPageToWindow:{
            setCurrentShowPage(item)//设置当前显示的页面为item
            left_bar.addItem(item)//增加左栏
        }
        onActiveChatPageChanged:{//如果活跃的Page改变为item
            //console.log(item)
            setCurrentShowPage(item)//设置当前显示的页面为item
        }
    }
    
    Rectangle{//用来管理当前聊天窗口内聊天页面的左栏
        id:left_bar
        
        property bool isOpen: false//记录此栏是否处于打开状态
        property int maxWidth:200
        property int minWidth:70
        property int defaultWidth: myqq.value("chatWindowLeftBarWidth", 150)//获取上次储存的值
        
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        color:"#ddd"
        radius:10
        clip: true
        
        onDefaultWidthChanged: {
            if(isOpen){//如果是打开状态
                setBarWidth(defaultWidth)//如果默认宽度改变就设置此栏的当前width
            }
        }
        onWidthChanged: {
            root.width = width+chatPageWidth//设置窗口的大小
        }

        function openBar(){
            console.debug("调用了打开侧栏："+isOpen)
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
        NumberAnimation{//动画控件
            id: animation_width
            target: left_bar
            running: false
            duration: 300
            property: "width"
        }
        Rectangle{
            anchors.right: parent.right
            color: left_bar.color
            width: parent.radius
            height: width
            visible: parent.width>0
        }
        Rectangle{
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            color: left_bar.color
            width: parent.radius
            height: width
            visible: parent.width>0
        }

        MyScrollView{
            anchors.fill: parent
            anchors.topMargin: image_quit_icon.height+15
            Item{
                height: mymodel.count*45+10
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
                property ChatPage my: obj_item
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
                    height: 35
                    sourceSize.width: width
                    grayscale: {//头像是否显示为黑白
                        if(my)
                            return my.myinfo.state == FriendInfo.Offline
                        return false
                    }

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
                    text: my?my.myinfo.aliasOrNick:""
                }
                Rectangle{
                    width: image_close_page.width+16
                    height: image_close_page.height+10
                    anchors.centerIn: image_close_page
                    color: "#eee"
                    radius: 5
                    opacity: 0.75
                    visible: image_close_page.visible&&root.currentShowPage!=my
                }
                Rectangle{
                    width: text_message_count.implicitWidth+10
                    height: image_close_page.width
                    anchors.right: rect_hover.right
                    anchors.rightMargin: 5
                    anchors.verticalCenter: parent.verticalCenter
                    color: "red"
                    radius: height
                    visible: my?my.myinfo.unreadMessagesCount>0:false
                    Text{
                        id: text_message_count
                        anchors.centerIn: parent
                        text: my?my.myinfo.unreadMessagesCount:"0"//未读消息的个数
                        color: "white"
                        onTextChanged: {
                            if(text == "100"){
                                text = "99+"
                            }
                        }
                    }
                }
                SvgView{
                    id:image_close_page
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
        MouseArea{//接收这栏右边的鼠标事件
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
                left_bar.setBarDefaultWidth(left_bar.defaultWidth+num_temp)//设置左栏的width
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
