import QtQuick 2.2
import QtQuick.Window 2.0

Window{
    id: rootwindow
    flags: Qt.SplashScreen
    color: "transparent"
    property bool isCanClose: false
    property ListModel mymodel
    property var root
    
    onFocusObjectChanged: {
        if( isCanClose ){
            root.isComboBoxCloseing=true
            close()
        } else
            isCanClose = true
    }
    
    Rectangle{
        anchors.fill: parent
        color: "white"
        radius: 5
        border.width: 1
        border.color: "#f07000"
    }

    ListView{
        id: list
        interactive: false
        anchors.fill: parent
        model: mymodel
        delegate: Component{
            Item{
                width: root.width
                height: root.height
                Rectangle{
                    width: parent.width-4
                    height: parent.height-4
                    anchors.centerIn: parent
                    color: "#f07900"
                    radius: 5
                    visible: mymouse.hovered
                }

                Text {
                    id: text
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    anchors.leftMargin: 5
                    text: model.text
                    color: mymouse.hovered?"white":"black"
                    font.pointSize: parent.height/2
                }
                MouseArea{
                    id: mymouse
                    anchors.fill: parent
                    hoverEnabled: true
                    property bool hovered: false
                    
                    onEntered: {
                        hovered = true
                    }

                    onExited: {
                        hovered = false
                    }

                    onClicked: {
                        root.pressed=false
                        root.currentIndex = index
                        root.accepted()
                        rootwindow.close()
                    }
                }
            }
        }
    }
}
