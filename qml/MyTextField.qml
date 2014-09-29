import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

Item{
    id: root
    property alias title: text.text
    property alias text: input.text
    property alias font: input.font
    property alias field: input
    Text {
        id: text
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        font.pointSize: root.height/2
    }
    TextField{
        id: input
        anchors.left: text.right
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        font.pointSize:text.font.pointSize
        style: TextFieldStyle {
                textColor: "black"
                background: BorderImage {
                    source: "qrc:/images/background_input.png"
                    border.left: 5; border.top: 5
                    border.right: 5; border.bottom: 5
                }
            }
    }
    Rectangle{
        radius: 1
        anchors.top: input.top
        anchors.topMargin: 2
        anchors.bottom: input.bottom
        anchors.bottomMargin: 0
        anchors.left: input.left
        anchors.leftMargin: 1
        anchors.right: parent.right
        anchors.rightMargin: 1
        
        visible: !root.enabled
        color: "#f0efef"
        opacity: 0.75
    }
}
