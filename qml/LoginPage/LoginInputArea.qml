import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import mywindow 1.0
//import "../des.js" as Des

Item{
    id:root
    width: 7/15*main.width
    height: 28/105*width
    property bool qqlistopen: false
    
    TextField{
        id: input_qq
        font.pointSize: implicitHeight/3
        KeyNavigation.down: input_password
        KeyNavigation.up: input_password
        KeyNavigation.tab: KeyNavigation.down
        text: utility.getValue("mainqq", "")
        style: TextFieldStyle {
            textColor: "black"
            background: Item{
                implicitWidth: border_qq.width-unfold_icon.width
                implicitHeight: border_qq.height
                
                BorderImage {
                    id: border_qq
                    width: root.width
                    height: root.height/2
                    border { left: 4; top: 4; right: 4; bottom: 4 }
                    source: "qrc:/images/inputBox1.png"
                }
                Rectangle{
                    visible: qqlistopen
                    width: parent.width+unfold_icon.width-5
                    height: parent.height-3
                    x:2
                    y:2
                    radius: 5
                    color: "#F3F2F2"
                }
                
            }
        }
        onTextChanged: {
            myqq.userQQ=text
            //input_password.text = Des.des(myqq.getValue("password", ""), false)//解密
        }
        Component.onCompleted: {
            input_qq.forceActiveFocus()
        }
    }
    TextField{
        id: input_password
        anchors.top: input_qq.bottom
        echoMode: TextInput.Password
        font.pointSize: implicitHeight/3
        KeyNavigation.up: input_qq
        KeyNavigation.down: input_qq
        KeyNavigation.tab: KeyNavigation.down
        //text: Des.des(myqq.getValue("password", ""), false)//解密
        style: TextFieldStyle {
            textColor: "black"
            background: Item{
                implicitWidth: border_password.width-soft_keyboard_icon.width
                implicitHeight: border_password.height
                BorderImage {
                    id: border_password
                    width: root.width
                    height: root.height/2
                    border { left: 4; top: 4; right: 4; bottom: 4 }
                    source: "qrc:/images/inputBox2.png"
                }
            }
        }
        onTextChanged: myqq.userPassword=text
    }
    SvgView {
        id: unfold_icon

        width: 1/25*main.width
        source: "qrc:/images/inputBox-more.svg"
        anchors.left: input_qq.right 
        anchors.leftMargin: -5
        anchors.verticalCenter: input_qq.verticalCenter
        property bool isCloseing: false
        onIsCloseingChanged: {
            if(isCloseing&&!mymouse1.hovered) {
                unfold_icon.rotation = 0
                isCloseing=false
                qqlistopen = false
            }
        }
        
        Connections{
            id: connection
            target: null
            onClicked:{
                input_qq.text = qq//填充qq号码
            }
            onListClose:{
                unfold_icon.isCloseing=true
            }
        }

        MouseArea{
            id: mymouse1
            anchors.fill: parent
            hoverEnabled: true
            property bool hovered: false
            
            onEntered: hovered = true
            onExited: hovered = false
            onClicked: {
                if( unfold_icon.isCloseing ){
                    unfold_icon.rotation = 0
                    unfold_icon.isCloseing=false
                    qqlistopen = false
                }else if( unfold_icon.rotation == 0 ){
                    unfold_icon.rotation = 180
                    qqlistopen = true
                    var component = Qt.createComponent("AccountList.qml");
                    if (component.status == Component.Ready){
                        var data={"width":root.width,"x": utility.mouseDesktopPos().x-mouse.x-unfold_icon.x+1,"y": utility.mouseDesktopPos().y-mouse.y-unfold_icon.y+input_qq.height}
                        connection.target = component.createObject(input_qq, data);
                    }
                }
            }
        }
    }
    
    SvgView {
        id: soft_keyboard_icon
        //sourceSize.width: width
        width: 1/25*main.width
        source: "qrc:/images/soft-keyboard.svg"
        anchors.left: input_password.right
        anchors.leftMargin: -5
        anchors.verticalCenter: input_password.verticalCenter
        property bool keyboardClose: true
        property bool isCloseing: false
        onIsCloseingChanged: {
            if(isCloseing&&!mymouse2.hovered) {
                keyboardClose=true
                isCloseing=false
            }
        }
        Connections{
            id: connections
            target: null
            onInput:{
                input_password.text+=arg
            }
            onBackspace:{
                input_password.text = input_password.text.substr(0, input_password.text.length-1)
            }
            onKeyboardClose:{
                soft_keyboard_icon.isCloseing=true
            }
        }
        
        MouseArea{
            id: mymouse2
            anchors.fill: parent
            property bool hovered: false
            hoverEnabled: true
            onEntered: hovered = true
            onExited: hovered = false
            onClicked: {
                if(soft_keyboard_icon.isCloseing){
                    soft_keyboard_icon.keyboardClose=true
                    soft_keyboard_icon.isCloseing=false
                }else if( soft_keyboard_icon.keyboardClose){
                    soft_keyboard_icon.keyboardClose=false
                    var component = Qt.createComponent("SoftKeyboard.qml");
                    if (component.status == Component.Ready){
                        var sprite = component.createObject(input_password);
                        connections.target = sprite
                    }
                }
            }
        }
    }
}
