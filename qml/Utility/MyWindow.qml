import QtQuick 2.2
import mywindow 1.0
import QtQuick.Window 2.1

MyQuickWindow{
    id: root
    property bool removable: true//窗口可移动？
    property bool  fixedSize: true//窗口可改变大小？
    property bool dockableWindow: false//窗口可停靠？
    property bool windowGlow: true//开启窗口阴影？
    property alias windowGlowItem: glow//阴影Item
    property int windowShakeInterval: animation_shake.duration*12///窗口抖动的时间
    property bool centered: true
    property int minWidth: 0
    property int maxWidth: 999999//正无穷
    property int minHeight: 0
    property int maxHeight: 999999//正无穷
    
    width: Math.max(minWidth, 300)
    height: Math.max(minHeight, 300)
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
                        root.windowShake()
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

    function widthIsValidity( num ) {//判断width是否合法
        if( num<=maxWidth&&num>=minWidth )
            return true
        else
            return false
    }
    function heightIsValidity( num ) {//判断height是否合法
        if( num<=maxHeight&&num>=minHeight )
            return true
        else
            return false
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
        glowRadius: 20
        spread: 0.1
        color: "black"
        width: root.width
        height:root.height
    }
    
    MyRectangularGlow{
        id: glow_inactive
        anchors.fill: glow
        glowRadius: glow.glowRadius/2
        spread: glow.spread
        color: glow.color
        glowOpacity: glow.glowOpacity
        cornerRadius: glow.cornerRadius
        cached: glow.cached
        visible: windowGlow&&(!root.windowActive)
    }
    
    MouseArea{
        id: mouse_main
        enabled: removable
        //anchors.fill: parent
        width: root.actualWidth
        height: root.actualHeight
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
    MouseArea{
        enabled: !fixedSize&&root.windowStatus==MyQuickWindow.StopCenter
        cursorShape :enabled?Qt.SizeVerCursor:Qt.ArrowCursor
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
            if(heightIsValidity(root.height+num_temp)) {
                root.height += num_temp
                root.y += mouseY-pressedY
            }
        }
    }
    MouseArea{
        enabled: !fixedSize&&root.windowStatus==MyQuickWindow.StopCenter
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
            if(heightIsValidity(root.height+num_temp)) {
                root.height += num_temp
            }
        }
    }
    MouseArea{
        enabled: !fixedSize&&root.windowStatus==MyQuickWindow.StopCenter
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
            if(widthIsValidity(root.width+num_temp)) {
                root.width += num_temp
                root.x += mouseX-pressedX
            }
        }
    }
    MouseArea{
        enabled: !fixedSize&&root.windowStatus==MyQuickWindow.StopCenter
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
            if(widthIsValidity(root.width+num_temp)) {
                root.width += num_temp
            }
        }
    }
    MouseArea{
        enabled: !fixedSize&&root.windowStatus==MyQuickWindow.StopCenter
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
            var num_temp1 = pressedX-mouseX
            var num_temp2 = pressedY-mouseY
            if(widthIsValidity(root.width+num_temp1)) {
                root.width += num_temp1
                root.x += mouseX-pressedX
            }
            if(heightIsValidity(root.height+num_temp2)) {
                root.height += num_temp2
                root.y += mouseY-pressedY
            }
        }
    }
    MouseArea{
        enabled: !fixedSize&&root.windowStatus==MyQuickWindow.StopCenter
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
            var num_temp1 = mouseX-pressedX
            var num_temp2 = pressedY-mouseY
            if(widthIsValidity(root.width+num_temp1)) {
                root.width += num_temp1
            }
            if(heightIsValidity(root.height+num_temp2)) {
                root.height += num_temp2 
                root.y += mouseY-pressedY
            }
        }
    }
    MouseArea{
        enabled:!fixedSize&&root.windowStatus==MyQuickWindow.StopCenter
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
            var num_temp1 = pressedX-mouseX
            var num_temp2 = mouseY-pressedY
            if(widthIsValidity(root.width+num_temp1)) {
                root.width += num_temp1
                root.x += mouseX-pressedX
            }
            if(heightIsValidity(root.height+num_temp2)) {
                root.height += num_temp2 
            }
        }
    }
    MouseArea{
        enabled: !fixedSize&&root.windowStatus==MyQuickWindow.StopCenter
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
            var num_temp1 = mouseX-pressedX
            var num_temp2 = mouseY-pressedY
            if(widthIsValidity(root.width+num_temp1)) {
                root.width += num_temp1
            }
            if(heightIsValidity(root.height+num_temp2)) {
                root.height += num_temp2 
            }
        }
    }
}
