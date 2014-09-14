import QtQuick 2.2
import QtQuick.Window 2.1
import mywindow 1.0

Window{
    id:root
    height: list.contentHeight
    flags: Qt.SplashScreen
    color: "transparent"
    visible: true
    property bool isCanClose: false
    signal listClose
    signal clicked( var qq )
    property int highlightIndex: 0//初始化为第0个
    
    Component{
        id: component
        Image{
            id: background_iamge
            source: index == mymodel.count-1?"qrc:/images/list_item_bottom.png":"qrc:/images/list_item.png"
            width: root.width
            height: {
                if(root.highlightIndex == index)
                    return 5/21*width
                else
                    return Math.max(5/21*width-10*Math.max(root.highlightIndex-index, index-root.highlightIndex), 4/21*width)
            }
            property bool is: false
            Behavior on height{
                NumberAnimation{
                    duration: 200
                }
            }

            Rectangle{
                id: background_rect
                anchors.top: parent.top
                anchors.topMargin: 1
                x:2
                width: parent.width-4
                anchors.bottom: parent.bottom
                anchors.bottomMargin: index == mymodel.count-1?2:1
                color: root.highlightIndex == index?"#fd7000":"#F3F2F2"
            }

            MyImage{
                id: image
                maskSource: "qrc:/images/bit.bmp"
                source: imageSrc
                x:5
                width: parent.height-10
                anchors.verticalCenter: parent.verticalCenter
                onLoadError: {
                    source = "qrc:/images/avatar.png"
                }
            }
            Text{
                id:text
                font.pointSize: parent.height/6
                anchors.left: image.right
                anchors.leftMargin: 10
                anchors.verticalCenter: parent.verticalCenter
                text: root.highlightIndex==index?"<font color=\"black\">"+nick+"</font>"+"<br><br><font color=\"white\">"+uin+"</font>":uin
                color: "black"
            }
            MouseArea{
                anchors.fill: parent
                hoverEnabled: true
                onEntered: {
                    root.highlightIndex = index
                }
                
                onClicked: {
                    root.clicked(uin)
                    root.close()
                    listClose()
                }
            }
            
            SvgView{
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                anchors.rightMargin: 20
                width: 1/3*parent.height
                //sourceSize.width: width
                visible: root.highlightIndex == index
                source: "qrc:/images/button-quit.svg"
                property string qq: uin
                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        mymodel.remove(index)
                        var data = utility.getValue("qq", "")
                        data = data.replace(new RegExp(","+parent.qq), "")
                        utility.setValue("qq", data)//将qq号码从里边删除
                        utility.setValue(parent.qq+"password", "")//将密码置空
                    }
                }
            }
        }
    }
    Component.onCompleted: {
        var data = utility.getValue("qq", "")
        data = data.split(",")
        for( var i=1;i<data.length;++i ){
            mymodel.append({"imageSrc": utility.getValue(data[i]+"avatar-100", "qrc:/images/avatar.png"), "nick":utility.getValue(data[i]+"nick",""), "uin": data[i]})
        }
    }

    ListView{
        id:list
        interactive: false
        anchors.fill: parent
        model: ListModel{id:mymodel}
        delegate: component
    }
    
    onFocusObjectChanged: {
        if( isCanClose ){
            root.close()
            listClose()
        } else
            isCanClose = true
    }
}
