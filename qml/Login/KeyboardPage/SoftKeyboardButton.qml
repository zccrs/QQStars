import QtQuick 2.2

Rectangle{
    id: root
    border.width: 1//control.activeFocus ? 2 : 1
    border.color: "#888"
    radius: 4
    property string text1: ""
    property string text2: ""
    property Item switchButton: null
    property bool shiftText: false
    property bool isShiftButton: false
    property bool isLetter: false
    property var backFun//回调函数，按下按键就调用这个函数
    signal trigger(var arg)
    onTrigger: {
        if( backFun )
            backFun(arg)
    }

    Connections{
        target: (isShiftButton||isLetter)?null:switchButton
        onTrigger: {
            shiftText = !shiftText
        }
    }

    Text{
        text:text1
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.margins: 5
        visible: text2!=""
        color: shiftText?"#999":"black"
        font.bold: shiftText?false:true
        font.pointSize: root.width/3
    }
    Text{
        text:text2
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.margins: 5
        visible: text2!=""
        color: !shiftText?"#999":"black"
        font.bold: !shiftText?false:true
        font.pointSize: root.width/3
    }
    
    Text{
        text: text1
        anchors.centerIn: parent
        visible: text2==""
        font.pointSize: root.width/7
    }

    MouseArea{
        id:mouse
        anchors.fill: parent
        hoverEnabled: true
        property bool mouseArea: false
        onEntered: {
            mouseArea = true
        }
        onExited: {
            mouseArea = false
        }

        onClicked: {
            if(isShiftButton){
                shiftText =!shiftText
                trigger(text1)
            }else if( text2=="" )
                trigger(text1)
            else{
                if( shiftText )
                    trigger(text2)
                else
                    trigger(text1)
            }
        }
    }
    gradient: Gradient {
                    GradientStop { 
                        position: 0
                        color: {
                            if(isShiftButton&&shiftText){
                                return "#f09000"
                            }else
                                return (mouse.pressed||mouse.mouseArea)?"#f09000":"#fff"
                        }
                    }
                    GradientStop { 
                        position: 1 
                        color: {
                            if(isShiftButton&&shiftText){
                                return "#f07000"
                            }else
                                return (mouse.pressed||mouse.mouseArea)?"#f07000":"#eee"
                        }
                    }
                }
}
