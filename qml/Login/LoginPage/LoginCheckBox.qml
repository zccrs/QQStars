import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import mywindow 1.0

CheckBox {
    id:root
    
    style: CheckBoxStyle {
        indicator: Item{
            implicitHeight: image.implicitHeight
            implicitWidth: image.implicitWidth
            SvgView{
                width: defaultSize.width*myqq.windowScale
                source: control.checked?"qrc:/images/checkBox-select.svg":"qrc:/images/checkBox-unselect.svg"
            }
            SvgView{
                id: image
                width: defaultSize.width*myqq.windowScale
                source: "qrc:/images/checkBox-hover.svg"
                visible: mouse.hover
                Component.onCompleted: root.height = width
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
