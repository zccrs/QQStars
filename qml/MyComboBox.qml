import QtQuick 2.2
import mywindow 1.0

Item{
    id: root
    signal accepted
    property int currentIndex
    property var currentValue
    property ListModel model
    property MyComboBoxComponent mycomboboxcomponent

    onCurrentIndexChanged: {
        mytext.text = model.get(currentIndex).text
        currentValue = model.get(currentIndex).value
    }

    property alias currentText: mytext.text
    property bool hovered: false
    property bool pressed: false
    
    
    BorderImage {
        id: background
        width: parent.width
        height: mytext.implicitHeight+10
        source: "qrc:/images/background_input.png"
        border.left: 5; border.top: 5
        border.right: 5; border.bottom: 5
        Text {
            id: mytext
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 5
            text: model.get(0).text
        }
        SvgView {
            source: "qrc:/images/inputBox-more.svg"
            rotation: root.pressed?180:0
            anchors.right: parent.right
            anchors.rightMargin: 5
            anchors.verticalCenter: parent.verticalCenter
        }
        MouseArea{
            anchors.fill: parent
            hoverEnabled: true
            onEntered: root.hovered=true
            onExited: root.hovered=false
            onClicked: {
                root.pressed=!root.pressed
                if( root.pressed ){
                    var component = Qt.createComponent("MyComboBoxComponent.qml");
                    if (component.status == Component.Ready){
                        var temp = utility.mouseDesktopPos();
                        var data = {"root":root, "mymodel":model, "x":temp.x-mouse.x, "y":temp.y+root.height-mouse.y+5, "width": root.width, "height": root.height*model.count}
                        mycomboboxcomponent = component.createObject(root, data);
                    }
                }
            }
        }
    }
}
