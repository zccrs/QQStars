import QtQuick 2.2
import mywindow 1.0
import QtQuick.Window 2.1

MyQuickWindow{
    id: root

    property bool removable: true//窗口可移动？
    property bool fixedSize: false//窗口是否固定大小？
    property bool fixedTopBorder: false//固定上边框，上边不可拉动
    property bool fixedBottomBorder: false//同上
    property bool fixedLeftBorder: false//同上
    property bool fixedRightBorder: false//同上
    property var setTopBorder: mySetTopBorder//用鼠标拉动上边框后调用的函数
    property var setBottomBorder: mySetBottomBorder//同上
    property var setLeftBorder: mySetLeftBorder//同上
    property var setRightBorder: mySetRightBorder//同上
    property bool dockableWindow: false//窗口可停靠？
    property bool windowGlow: true//开启窗口阴影？
    property alias windowGlowItem: glow//阴影Item
    property int windowShakeInterval: animation_shake.duration*16///窗口抖动的时间
    property bool centered: true//初次显示时是否居中
    property int contentItemAreaTop: -5//设定内容区域的上边界坐标
    property int contentItemAreaBottom: contentItemAreaTop+height+10//同上
    property int contentItemAreaLeft: -5//同上
    property int contentItemAreaRight: contentItemAreaLeft+width+10//同上
    signal manulPullLeftBorder//如果用户在窗口左边拉动改变了窗口大小
    signal manulPullRightBorder//同上
    signal manulPullTopBorder//同上
    signal manulPullBottomBorder//同上
    
    actualWidth: windowGlow?glow.actualWidth:width+2*mouse_left.width
    actualHeight: windowGlow?glow.actualHeight:height+2*mouse_top.height

    onContentItemAreaTopChanged: updateMousePenetrateArea()
    onContentItemAreaBottomChanged: updateMousePenetrateArea()
    onContentItemAreaLeftChanged: updateMousePenetrateArea()
    onContentItemAreaRightChanged: updateMousePenetrateArea()

    Component.onCompleted: {
        if(centered){
            var old_visible = visible
            visible = false
            actualX = Screen.desktopAvailableWidth/2 - actualWidth/2
            actualY = Screen.desktopAvailableHeight/2 - actualHeight/2
            visible = old_visible
        }
    }

    contentItem{
        x: windowGlow?glow.glowLeftWidth:mouse_left.width
        y: windowGlow?glow.glowTopHeight:mouse_top.height
    }

    Connections{
        target: windowGlow?contentItem:null
        onWidthChanged:{
            contentItem.width=width
        }
        onHeightChanged:{
            contentItem.height=height
        }
    }

    QtObject{
        id: obj
        property int windowShakeCount: 0
        property real back_glowOpacity
        Component.onCompleted: {
            back_glowOpacity = glow.glowOpacity
        }
    }

    function updateMousePenetrateArea(){
        var rect = Qt.rect(contentItemAreaLeft, contentItemAreaTop, 0, 0)
        rect.width = contentItemAreaRight - rect.x + 1
        rect.height = contentItemAreaBottom - rect.y + 1
        setMousePenetrateArea(rect)
    }

    function windowShake() {//抖动窗口
        if(obj.windowShakeCount>=root.windowShakeInterval/animation_shake.duration/4){
            obj.windowShakeCount=0
            return
        }

        ++obj.windowShakeCount
        showFront()//先把窗口显示在最前端
        animation_shake.property = "x"
        animation_shake.to = root.x-20
        animation_shake.backFun=function(){
            animation_shake.property = "y"
            animation_shake.to = root.y-20
            animation_shake.backFun=function(){
                animation_shake.property="x"
                animation_shake.to=root.x+20
                animation_shake.backFun=function(){
                    animation_shake.property="y"
                    animation_shake.to=root.y+20
                    animation_shake.backFun=function(){
                        windowShake()//进行递归调用
                    }
                    animation_shake.start()
                }
                animation_shake.start()
            }
            animation_shake.start()
        }
        animation_shake.start()
    }
    function showFront() {//显示到最前面
        if(root.visible) {
            if( root.visibility== MyQuickWindow.Minimized){
                root.show()
            }
            root.requestActivate()//让窗体显示到最前端
        }
    }

    function berthWindow(){
        if( root.windowStatus!=MyQuickWindow.BerthPrepare||animation.running ) {//如果不是准停状态
            return
        }
        if( root.x==root.borderLeft ){
            root.windowStatus = MyQuickWindow.BerthLeft
            animation.property = "x"
            animation.to = root.borderLeft-width+1
            animation.start()
        }else if( root.x==root.borderRight-root.width ){
            root.windowStatus = MyQuickWindow.BerthRight
            animation.property = "x"
            animation.to = root.borderRight-1
            animation.start()
        }else if( root.y==root.borderTop ){
            root.windowStatus = MyQuickWindow.BerthTop
            animation.property = "y"
            animation.to = root.borderTop-height+1
            animation.start()
        }
    }
    
    function showWindow(){///从停靠状态转为可停靠状态
        if( animation.running ) {//如果动画正在运行
            return
        }
        switch( windowStatus ){
        case MyQuickWindow.BerthTop:
        {
            animation.property = "y"
            animation.to = root.borderTop
            animation.start()
            root.windowStatus = MyQuickWindow.BerthPrepare//进入准停靠状态
            break;
        }
        case MyQuickWindow.BerthLeft:
        {
            animation.property = "x"
            animation.to = root.borderLeft
            animation.start()
            root.windowStatus = MyQuickWindow.BerthPrepare//进入准停靠状态
            break;
        }
        case MyQuickWindow.BerthRight:
        {
            animation.property = "x"
            animation.to = root.borderRight-root.width
            animation.start()
            root.windowStatus = MyQuickWindow.BerthPrepare//进入准停靠状态
            break;
        }
        default:break
        }
    }
    function mySetLeftBorder(arg){//当从左边改变窗口的width时
        if(!fixedLeftBorder){
            var temp = root.width
            root.width+=arg;
            temp = root.width-temp//计算出其中的差值
            if(temp!=0){
                root.x-=temp//改变窗口坐标
                manulPullLeftBorder()//发送拉动了左边界的信号
            }
        }
    }
    function mySetRightBorder(arg){//当从右边改变窗口的width时
        if(!fixedRightBorder){
            var temp = root.width
            root.width+=arg;
            temp = root.width-temp//计算出其中的差值
            if(temp!=0){
                manulPullRightBorder()//发送拉动了右边界的信号
            }
        }
    }
    function mySetTopBorder(arg){//当从上边改变窗口的width时
        if(!fixedTopBorder){
            var temp = root.height
            root.height+=arg;
            temp = root.height-temp//计算出其中的差值
            if(temp!=0){
                root.y-=temp//改变窗口坐标
                manulPullTopBorder()//发送拉动了上边界的信号
            }
        }
    }
    function mySetBottomBorder(arg){//当从下边改变窗口的width时
        if(!fixedBottomBorder){
            var temp = root.height
            root.height+=arg;
            temp = root.height-temp//计算出其中的差值
            if(temp!=0){
                manulPullBottomBorder()//发送拉动了下边界的信号
            }
        }
    }

    NumberAnimation{
        id: animation
        target: root
        duration: 100
    }
    NumberAnimation{
        id: animation_shake
        target: root
        duration: 50
        easing.type: Easing.OutInElastic
        property var backFun: null
        onStopped: {
            if(backFun)
                backFun()
        }
    }
    NumberAnimation{
        id: animation_shake_x
        target: root
        duration: 100
        property: "x"
        easing.type: Easing.OutInElastic
        property var backFun: null
        onStopped: {
            if(backFun)
                backFun()
        }
    }
    NumberAnimation{
        id: animation_shake_y
        target: root
        duration: 100
        property: "y"
        easing.type: Easing.OutInElastic
        property var backFun: null
        onStopped: {
            if(backFun)
                backFun()
        }
    }
    
    MyRectangularGlow{
        id: glow
        visible: windowGlow&&root.windowActive
        x:0
        y:0
        glowRadius: 50
        glowOpacity: 0.75
        biasY: 20
        spread: 0.1
        color: "black"
        width: root.width
        height:root.height
    }
    
    MyRectangularGlow{
        id: glow_inactive//当窗口处于非活跃状态时显示的阴影
        anchors.fill: glow
        glowRadius: glow.glowRadius/2
        spread: glow.spread
        color: glow.color
        glowOpacity: glow.glowOpacity
        cornerRadius: glow.cornerRadius
        biasX: glow.biasX
        biasY: glow.biasY
        cached: glow.cached
        visible: windowGlow&&(!root.windowActive)
    }
    
    MouseArea{
        id: mouse_main

        property real pressedX: 0
        property real pressedY: 0
        property point pressedCursorPos

        enabled: removable
        anchors.fill: parent
        hoverEnabled: dockableWindow
        
        onPressed: {
            pressedX = root.x
            pressedY = root.y
            pressedCursorPos = cursorPos
        }
        onEntered: {
            if(dockableWindow){
                showWindow()//将窗口从停靠地方显示出来
            }
        }

        onExited: {
            if(dockableWindow){//如果开启了窗口停靠
                mouse_main_connections.target = root//接收全局鼠标改变的信号
            }
        }

        onReleased: {
            if(dockableWindow&&(!animation.running)&&(root.windowStatus==MyQuickWindow.StopCenter||root.windowStatus==MyQuickWindow.BerthPrepare)){
                if( root.x<=root.borderLeft ){
                    root.x=root.borderLeft
                    root.windowStatus = MyQuickWindow.BerthPrepare//进入准停靠状态
                }else if( root.x>root.borderRight-root.width ){
                    root.x = root.borderRight-root.width
                    root.windowStatus = MyQuickWindow.BerthPrepare//进入准停靠状态
                }else if( root.y<root.borderTop ){
                    root.y = root.borderTop
                    root.windowStatus = MyQuickWindow.BerthPrepare//进入准停靠状态
                }else if( root.windowStatus == MyQuickWindow.BerthPrepare )
                    root.windowStatus = MyQuickWindow.StopCenter//失去准停靠状态
            }
        }

        onPositionChanged: {
            if( mouse_main.pressed ){//如果鼠标被按下
                root.x = (cursorPos.x - pressedCursorPos.x + pressedX)//改变窗口的位置
                root.y = (cursorPos.y - pressedCursorPos.y + pressedY)//改变窗口的位置
            }else{
                var x = arg.x-root.x
                var y = arg.y-root.y
                if(!(x<=contentItemAreaRight&&x>=contentItemAreaLeft
                     &&y<=contentItemAreaBottom&&y>=contentItemAreaTop)){
                    berthWindow()//调用函数让窗口进行停靠
                    mouse_main_connections.target = null//关闭接收全局鼠标改变的信号
                }
            }
        }
    }
    MouseArea{//接收窗口上部的鼠标事件，用于朝上拉动窗口来改变窗口的大小
        id: mouse_top
        enabled: noBorder&&!fixedSize&&!fixedTopBorder&&root.windowStatus==MyQuickWindow.StopCenter
        cursorShape :enabled?Qt.SizeVerCursor:Qt.ArrowCursor//鼠标样式
        anchors.bottom: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        width: root.width-6
        height: 3
        z:1
        property real pressedX: 0
        property real pressedY: 0
        onPressed: {
            pressedX = mouseX
            pressedY = mouseY
        }
        onPositionChanged: {
            var num_temp = pressedY-mouseY
            setTopBorder(num_temp)
        }
    }
    MouseArea{//接收窗口下部的鼠标事件，用于朝下拉动窗口来改变窗口的大小
        id: mouse_bottom
        enabled: noBorder&&!fixedSize&&!fixedBottomBorder&&root.windowStatus==MyQuickWindow.StopCenter
        cursorShape :enabled?Qt.SizeVerCursor:Qt.ArrowCursor
        anchors.top: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        width: root.width-6
        height: 3
        z:1
        property real pressedX: 0
        property real pressedY: 0
        onPressed: {
            pressedY = mouseY
        }
        onPositionChanged: {
            var num_temp = mouseY-pressedY
            setBottomBorder(num_temp)
        }
    }
    MouseArea{//接收窗口左部的鼠标事件，用于朝左拉动窗口来改变窗口的大小
        id: mouse_left
        enabled: noBorder&&!fixedSize&&!fixedLeftBorder&&root.windowStatus==MyQuickWindow.StopCenter
        cursorShape :enabled?Qt.SizeHorCursor:Qt.ArrowCursor
        anchors.right: parent.left
        anchors.verticalCenter: parent.verticalCenter
        height: root.height-6
        width: 3
        z:1
        property real pressedX: 0
        property real pressedY: 0

        onPressed: {
            pressedX = mouseX
        }
        onPositionChanged: {
            var num_temp = pressedX-mouseX
            setLeftBorder(num_temp)
        }
    }
    MouseArea{//接收窗口右部的鼠标事件，用于朝右拉动窗口来改变窗口的大小
        id: mouse_right
        enabled: noBorder&&!fixedSize&&!fixedRightBorder&&root.windowStatus==MyQuickWindow.StopCenter
        cursorShape :enabled?Qt.SizeHorCursor:Qt.ArrowCursor
        anchors.left: parent.right
        anchors.verticalCenter: parent.verticalCenter
        height: root.height-6
        width: 3
        z:1
        property real pressedX: 0
        property real pressedY: 0
        onPressed: {
            pressedX = mouseX
        }
        onPositionChanged: {
            var num_temp = mouseX-pressedX
            setRightBorder(num_temp)
        }
    }
    MouseArea{//接收窗口左上部的鼠标事件，用于朝左拉动窗口来改变窗口的大小
        enabled: mouse_left.enabled&&mouse_top.enabled
        cursorShape :enabled?Qt.SizeFDiagCursor:Qt.ArrowCursor
        anchors.right: parent.left
        anchors.bottom: parent.top
        height: 5
        width: 5
        z:1
        property real pressedX: 0
        property real pressedY: 0
        onPressed: {
            pressedX = mouseX
            pressedY = mouseY
        }
        onPositionChanged: {
            var num_temp1 = pressedX-mouseX
            setLeftBorder(num_temp1)
            var num_temp2 = pressedY-mouseY
            setTopBorder(num_temp2)
        }
    }
    MouseArea{//接收窗口右上部的鼠标事件，用于朝左拉动窗口来改变窗口的大小
        enabled: mouse_right.enabled&&mouse_top.enabled
        cursorShape :enabled?Qt.SizeBDiagCursor:Qt.ArrowCursor
        anchors.left: parent.right
        anchors.bottom: parent.top
        height: 5
        width: 5
        z:1
        property real pressedX: 0
        property real pressedY: 0
        onPressed: {
            pressedX = mouseX
            pressedY = mouseY
        }
        onPositionChanged: {
            var num_temp1 = mouseX-pressedX
            setRightBorder(num_temp1)
            var num_temp2 = pressedY-mouseY
            setTopBorder(num_temp2)
        }
    }
    MouseArea{//接收窗口左下部的鼠标事件，用于朝左拉动窗口来改变窗口的大小
        enabled:mouse_left.enabled&&mouse_bottom.enabled
        cursorShape :enabled?Qt.SizeBDiagCursor:Qt.ArrowCursor
        anchors.right: parent.left
        anchors.top: parent.bottom
        height: 5
        width: 5
        z:1
        property real pressedX: 0
        property real pressedY: 0
        onPressed: {
            pressedX = mouseX
            pressedY = mouseY
        }
        onPositionChanged: {
            var num_temp1 = pressedX-mouseX
            setLeftBorder(num_temp1)
            var num_temp2 = mouseY-pressedY
            setBottomBorder(num_temp2)
        }
    }
    MouseArea{//接收窗口右下部的鼠标事件，用于朝左拉动窗口来改变窗口的大小
        enabled: mouse_right.enabled&&mouse_bottom.enabled
        cursorShape :enabled?Qt.SizeFDiagCursor:Qt.ArrowCursor
        anchors.left: parent.right
        anchors.top: parent.bottom
        height: 5
        width: 5
        z:1
        property real pressedX: 0
        property real pressedY: 0
        onPressed: {
            pressedX = mouseX
            pressedY = mouseY
        }
        onPositionChanged: {
            var num_temp1 = mouseX-pressedX
            setRightBorder(num_temp1)
            var num_temp2 = mouseY-pressedY
            setBottomBorder(num_temp2)
        }
    }
}
