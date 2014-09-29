import QtQuick 2.2
import mywindow 1.0

MyQuickWindow{
    id: root
    property bool removable: true
    property bool  fixedSize: true
    property bool dockableWindow: false
    property int minWidth: 0
    property int maxWidth: 999999//正无穷
    property int minHeight: 0
    property int maxHeight: 999999//正无穷
    width: Math.max(minWidth, 300)
    height: Math.max(minHeight, 300)

    function widthIsValidity( num ) {
        if( num<=maxWidth&&num>=minWidth )
            return true
        else
            return false
    }
    function heightIsValidity( num ) {
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

    MouseArea{
        id: mouse_main
        enabled: removable
        anchors.fill: parent
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
