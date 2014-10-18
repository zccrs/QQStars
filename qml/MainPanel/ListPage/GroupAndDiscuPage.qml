import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

Item{
    TabView{
        anchors.fill: parent
        Tab{
            title: "QQ群"
            active: true
            GroupList{
                width: parent.width
                height: parent.height
            }
        }
        Tab{
            title: "讨论组"
            active: true
            DiscuList{
                width: parent.width
                height: parent.height
            }
        }
        style: TabViewStyle {
            //tabOverlap: 10
            frameOverlap: -10
            tabsMovable : true
            tabsAlignment:Qt.AlignHCenter
            tab:Rectangle {
                color: styleData.selected ? "#f07000" :"#eee"
                border.color:  "#888"
                border.width: 1
                implicitWidth: Math.max(text.width + 4, 80)
                implicitHeight: 20
                radius: 10
                Rectangle{
                    height: parent.implicitHeight
                    width: height
                    x:index==1?0:parent.width-width
                    color: parent.color
                    border.width: parent.border.width
                    border.color: parent.border.color
                    Rectangle{
                        height: parent.height-2*width
                        width: parent.border.width
                        anchors.verticalCenter: parent.verticalCenter
                        x:index==0?0:parent.width-width
                        color: parent.color
                    }
                }

                Text {
                    id: text
                    anchors.centerIn: parent
                    text: styleData.title
                    color: styleData.selected ? "white" : "black"
                }
            }
            frame: Item {}
        }
    }
}
