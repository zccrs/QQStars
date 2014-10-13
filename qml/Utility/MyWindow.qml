import QtQuick 2.2
import mywindow 1.0
import QtQuick.Window 2.1

MyQuickWindow{
    id: root
    property bool removable: true//窗口可移动？
    property bool fixedSize: false//窗口是否固定大小？
    property bool fixedTopBorder: false//固定上边框，上边不可拉动
    property bool fixedBottomBorder: fase//同上
    property bool fixedLeftBorder: false//同上
    property bool fixedRightBorder: false//同上
    property bool dockableWindow: false//窗口可停靠？
    property bool windowGlow: true//开启窗口阴影？
    property alias windowGlowItem: glow//阴影Item
    property int windowShakeInterval: animation_shake.duration*16///窗口抖动的时间
    property bool centered: true//初次显示时是否居中
    signal manulPullLeftBorder//如果用户在窗口左边拉动改变了窗口大小
    signal manulPullRightBorder//同上
    signal manulPullTopBorder//同上
    signal manulPullBottomBorder//同上
    
    width: 300
    height: 300
    actualWidth: windowGlow?glow.actualWidth:width
    actualHeight: windowGlow?glow.actualHeight:height
    Component.onCompleted: {
        if(centered){
            actualX = Screen.desktopAvailableWidth/2-actualWidth/2
            actualY = Screen.desktopAvailableHeight/2-actualHeight/2
        }
    }

    contentItem{
        x: windowGlow?glow.glowLeftWidth:0
        y: windowGlow?glow.glowTopHeight:0
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

    NumberAnimation{
        id: animation
        target: root
        duration: 100
    }
    NumberAnimation{
        id: animation_shake
        target: root
        duration: 20
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
        enabled: removable
        anchors.fill: parent
        //width: root.actualWidth
        //height: root.actualHeight
        property real pressedX: 0
        property real pressedY: 0
        hoverEnabled: dockableWindow
        onPressed: {
            pressedX = mouseX
            pressedY = mouseY
        }
        onEntered: showWindow()
        onExited: berthWindow()

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
            if( mouse_main.pressed ){
                root.x += (mouseX-pressedX)
                root.y += (mouseY-pressedY)
            }
        }
    }
    MouseArea{//接收窗口上部的鼠标事件，用于朝上拉动窗口来改变窗口的大小
        id: mouse_top
        enabled: !fixedSize&&!fixedTopBorder&&root.windowStatus==MyQuickWindow.StopCenter
        cursorShape :enabled?Qt.SizeVerCursor:Qt.ArrowCursor//鼠标样式
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        width: root.width-4
        height: 2
        property real pressedX: 0
        property real pressedY: 0
        onPressed: {
            pressedX = mouseX
            pressedY = mouseY
        }
        onPositionChanged: {
            var num_temp = pressedY-mouseY
            root.height += num_temp
            root.y += mouseY-pressedY
            manulPullTopBorder()//发送信号，说明用户手动向上拉动了窗口
        }
    }
    MouseArea{//接收窗口下部的鼠标事件，用于朝下拉动窗口来改变窗口的大小
        id: mouse_bottom
        enabled: !fixedSize&&!fixedBottomBorder&&root.windowStatus==MyQuickWindow.StopCenter
        cursorShape :enabled?Qt.SizeVerCursor:Qt.ArrowCursor
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        width: root.width-4
        height: 2
        property real pressedX: 0
        property real pressedY: 0
        onPressed: {
            pressedY = mouseY
        }
        onPositionChanged: {
            var num_temp = mouseY-pressedY
            root.height += num_temp
            manulPullBottomBorder()//发送信号，说明用户手动向下拉动了窗口
        }
    }
    MouseArea{//接收窗口左部的鼠标事件，用于朝左拉动窗口来改变窗口的大小
        id: mouse_left
        enabled: !fixedSize&&!fixedLeftBorder&&root.windowStatus==MyQuickWindow.StopCenter
        cursorShape :enabled?Qt.SizeHorCursor:Qt.ArrowCursor
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        height: root.height-4
        width: 2
        property real pressedX: 0
        property real pressedY: 0
        onPressed: {
            pressedX = mouseX
        }
        onPositionChanged: {
            var num_temp = pressedX-mouseX
            root.width += num_temp
            root.x += mouseX-pressedX
            manulPullLeftBorder()//发送信号，说明用户手动向左拉动了窗口
        }
    }
    MouseArea{//接收窗口右部的鼠标事件，用于朝右拉动窗口来改变窗口的大小
        id: mouse_right
        enabled: !fixedSize&&!fixedRightBorder&&root.windowStatus==MyQuickWindow.StopCenter
        cursorShape :enabled?Qt.SizeHorCursor:Qt.ArrowCursor
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        height: root.height-4
        width: 2
        property real pressedX: 0
        property real pressedY: 0
        onPressed: {
            pressedX = mouseX
        }
        onPositionChanged: {
            var num_temp = mouseX-pressedX
            root.width += num_temp
            manulPullRightBorder()//发送信号，说明用户手动向右拉动了窗口
        }
    }
    MouseArea{//接收窗口左上部的鼠标事件，用于朝左拉动窗口来改变窗口的大小
        enabled: mouse_left.enabled&&mouse_top.enabled
        cursorShape :enabled?Qt.SizeFDiagCursor:Qt.ArrowCursor
        height: 4
        width: 4
        property real pressedX: 0
        property real pressedY: 0
        onPressed: {
            pressedX = mouseX
            pressedY = mouseY
        }
        onPositionChanged: {
            if(!fixedLeftBorder){//判断是否固定了左边框
                var num_temp1 = pressedX-mouseX
                root.width += num_temp1
                root.x += mouseX-pressedX
                manulPullLeftBorder()//发送信号，说明用户手动向左拉动了窗口
            }
            if(!fixedTopBorder){//判断是否固定了右边框
                var num_temp2 = pressedY-mouseY
                root.height += num_temp2
                root.y += mouseY-pressedY
                manulPullTopBorder()//发送信号，说明用户手动向上拉动了窗口
            }
        }
    }
    MouseArea{//接收窗口右上部的鼠标事件，用于朝左拉动窗口来改变窗口的大小
        enabled: mouse_right.enabled&&mouse_top.enabled
        cursorShape :enabled?Qt.SizeBDiagCursor:Qt.ArrowCursor
        anchors.right: parent.right
        height: 4
        width: 4
        property real pressedX: 0
        property real pressedY: 0
        onPressed: {
            pressedX = mouseX
            pressedY = mouseY
        }
        onPositionChanged: {
            if(!fixedRightBorder){
                var num_temp1 = mouseX-pressedX
                root.width += num_temp1
                manulPullRightBorder()//发送信号，说明用户手动向右拉动了窗口
            }

            if(!fixedRightBorder){
                var num_temp2 = pressedY-mouseY
                root.height += num_temp2 
                root.y += mouseY-pressedY
                manulPullTopBorder()//发送信号，说明用户手动向上拉动了窗口
            }
        }
    }
    MouseArea{//接收窗口左下部的鼠标事件，用于朝左拉动窗口来改变窗口的大小
        enabled:mouse_left.enabled&&mouse_bottom.enabled
        cursorShape :enabled?Qt.SizeBDiagCursor:Qt.ArrowCursor
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        height: 4
        width: 4
        property real pressedX: 0
        property real pressedY: 0
        onPressed: {
            pressedX = mouseX
            pressedY = mouseY
        }
        onPositionChanged: {
            if(!fixedLeftBorder){
                var num_temp1 = pressedX-mouseX
                root.width += num_temp1
                root.x += mouseX-pressedX
                manulPullLeftBorder()//发送信号，说明用户手动向左拉动了窗口
            }
            if(!fixedBottomBorder){
                var num_temp2 = mouseY-pressedY
                root.height += num_temp2 
                manulPullBottomBorder()//发送信号，说明用户手动向下拉动了窗口
            }
        }
    }
    MouseArea{//接收窗口右下部的鼠标事件，用于朝左拉动窗口来改变窗口的大小
        enabled: mouse_right.enabled&&mouse_bottom.enabled
        cursorShape :enabled?Qt.SizeFDiagCursor:Qt.ArrowCursor
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: 4
        width: 4
        property real pressedX: 0
        property real pressedY: 0
        onPressed: {
            pressedX = mouseX
            pressedY = mouseY
        }
        onPositionChanged: {
            if(!fixedRightBorder){
                var num_temp1 = mouseX-pressedX
                root.width += num_temp1
                manulPullRightBorder()//发送信号，说明用户手动向右拉动了窗口
            }
            if(!fixedBottomBorder){
                var num_temp2 = mouseY-pressedY
                root.height += num_temp2 
                manulPullBottomBorder()//发送信号，说明用户手动向下拉动了窗口
            }
        }
    }
}
