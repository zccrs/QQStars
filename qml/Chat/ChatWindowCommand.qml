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
        if(currentShowPage!==page){//判断是否page就是当前活动页面
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
        }
        onActiveChatPageChanged:{//如果活跃的Page改变为item
            setCurrentShowPage(item)//设置当前显示的页面为item
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
    
    Item{
        id:left_bar
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.bottom: parent.bottom
    }
}
