import QtQuick 2.2
import mywindow 1.0

SvgView{
    id:root
    //sourceSize.width: width
    property alias text: button_text.text
    property alias font: button_text.font
    source: {
        if( mouse.pressed )
            return "qrc:/images/button-login-press.svg"
        else if(mouse.hover)
            return "qrc:/images/button-login-hover.svg"
        else
            return "qrc:/images/button-login.svg"
    }

    signal clicked
    Rectangle{
        radius: 10
        anchors.fill: parent
        color: "#888"
        visible: !root.enabled
    }

    Text {
        id: button_text
        anchors.centerIn: parent
        color: "white"
    }
    MouseArea{
        id:mouse
        enabled: root.enabled
        property bool hover: false
        anchors.fill: parent
        hoverEnabled: true
        onEntered: {
            hover = true
        }
        onExited: {
            hover = false
        }
        onClicked: {
            root.clicked()
        }
    }
}
