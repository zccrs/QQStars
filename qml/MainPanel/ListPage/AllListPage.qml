import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

TabView {
    id: frame
    property int old_current:0
    
    Tab {
        active: true
        id: friend_list
        FriendList{
            id: rect1
            width: parent.width
            height: parent.height
            NumberAnimation{
                id: animat1
                target: rect1
                duration: 200
                running: false
                easing.type: Easing.InCubic
                property: "x"
                from: -width
                to:0
            }
            Connections{
                target: frame
                onCurrentIndexChanged: {
                    if(frame.currentIndex==0){
                        animat1.start()
                        frame.old_current = 0
                    }
                }
            }
        }
    }
    Tab {
        active: true
        GroupList{
            id:rect2
            width: parent.width
            height: parent.height
            NumberAnimation{
                id: animat2
                target: rect2
                duration: 200
                running: false
                easing.type: Easing.InCubic
                property: "x"
                from: frame.old_current==0?width:-width
                to:0
            }
            Connections{
                target: frame
                onCurrentIndexChanged: {
                    if(frame.currentIndex==1){
                        animat2.start()
                        frame.old_current = 1
                    }
                }
            }
        }
    }
    Tab { 
        active: true
        id: recent_list
        RecentList{
            id:rect3
            width: parent.width
            height: parent.height
            
            NumberAnimation{
                id: animat3
                target: rect3
                duration: 200
                running: false
                easing.type: Easing.InCubic
                property: "x"
                from: width
                to:0
            }
            Connections{
                target: frame
                onCurrentIndexChanged: {
                    if(frame.currentIndex==2){
                        animat3.start()
                        frame.old_current = 2
                    }
                }
            }
        }
        
    }

    style: TabViewStyle {
        tabOverlap: 10
        tabsMovable : true
        tabsAlignment:Qt.AlignHCenter
        tab: Item{
            implicitHeight: height
            implicitWidth: width
            width: control.width/3
            
            height: icon.implicitHeight
            Image{
                id: icon
                anchors.horizontalCenter: parent.horizontalCenter
                source: {
                    if(index==0)
                        return "qrc:/images/contact_press.png"
                    else if(index==1)
                        return "qrc:/images/group_press.png"
                    else
                        return "qrc:/images/TempSession_press.png"
                }
            }
        }

        //rightCorner :Rectangle { color: "steelblue" }
        frame: Item {}
    }
}
