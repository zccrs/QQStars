import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import mywindow 1.0

CheckBox {
    id:root
    height:2/45*main.width
    style: CheckBoxStyle {
        indicator: Item{
            implicitHeight: image.implicitHeight
            implicitWidth: image.implicitWidth
            SvgView{
                //sourceSize.width: width
                width: 2/45*main.width
                source: control.checked?"qrc:/images/checkBox-select.svg":"qrc:/images/checkBox-unselect.svg"
            }
            SvgView{
                id: image
                //sourceSize.width: width
                width: 2/45*main.width
                source: "qrc:/images/checkBox-hover.svg"
                visible: mouse.hover
            }
        }
        label: Text{
            text: control.text
            font.pointSize: root.height/2
        }
    }
    MouseArea{
        id:mouse
        anchors.fill: parent
        property bool hover: false
        hoverEnabled: true
        onEntered: {
            hover = true
        }
        onExited: hover = false
        onClicked: {
            parent.checked=!parent.checked
        }
    }
}
