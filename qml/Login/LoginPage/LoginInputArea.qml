import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import mywindow 1.0

Item{
    id:root
    property bool qqlistopen: false
    //color: "red"
    TextField{
        id: input_qq
        font.pointSize: root.height/6
        KeyNavigation.down: input_password
        KeyNavigation.up: input_password
        KeyNavigation.tab: KeyNavigation.down
        text: utility.value("mainqq", "")
        
        validator: RegExpValidator{
            regExp: /[1-9][0-9]{4,11}/
        }

        style: TextFieldStyle {
            textColor: "black"
            background: Item{
                implicitWidth: border_qq.width-unfold_icon.width
                implicitHeight: 25/64*border_qq.height
                //color: "black"
                SvgView {
                    id: border_qq
                    width: defaultSize.width*myqq.windowScale
                    x: -5/220*width
                    y: -6/64*height
                    source: "qrc:/images/inputBox1.svg"
                    /*Rectangle{
                        anchors.fill: parent
                        color: "blue"
                    }*/
                }
                /*Rectangle{
                    visible: qqlistopen
                    width: (root.width-14)
                    height: (root.height/2)
                    x:2
                    y:6
                    radius: 5
                    color: "#F3F2F2"
                }*/
                
            }
        }
        onTextChanged: {
            myqq.userQQ=text
            //input_password.text = utility.stringUncrypt(myqq.value("password", ""), "xingchenQQ")//解密
            input_password.text = utility.stringUncrypt(myqq.userPassword, "xingchenQQ")//解密
        }
        Component.onCompleted: {
            input_qq.forceActiveFocus()
        }
    }
    TextField{
        id: input_password
        anchors.top: input_qq.bottom
        anchors.topMargin: 4/128*root.height
        echoMode: TextInput.Password
        font.pointSize: root.height/8
        KeyNavigation.up: input_qq
        KeyNavigation.down: input_qq
        KeyNavigation.tab: KeyNavigation.down
        validator: RegExpValidator{
            regExp: /[0-9a-zA-Z`~\!@\#\$%\^&\*\(\)\-\=_\+\[\]\{\}\\\|;\:'"<>\?,\.\/]{6,16}/
        }
        text: utility.stringUncrypt(myqq.userPassword, "xingchenQQ")//解密//utility.stringUncrypt(myqq.value("password", ""), "xingchenQQ")//解密
        onTextChanged: {
            if(myqq.rememberPassword)
                myqq.userPassword=text
        }

        style: TextFieldStyle {
            textColor: "black"
            background: Item{
                implicitWidth: border_password.width-soft_keyboard_icon.width
                implicitHeight: 23/64*border_password.height
                //color: "blue"
                SvgView {
                    id: border_password
                    x: -5/220*width
                    width: defaultSize.width*myqq.windowScale
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: -6/64*height
                    source: "qrc:/images/inputBox2.svg"
                }
            }
        }
    }
    SvgView {
        id: unfold_icon
        width: defaultSize.width*myqq.windowScale
        source: "qrc:/images/inputBox-more.svg"
        anchors.left: input_qq.right 
        anchors.leftMargin: -1/16*root.width
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
                        var data={"width":21/22*root.width,"x": utility.mouseDesktopPos().x-mouse.x-unfold_icon.x,"y": utility.mouseDesktopPos().y-mouse.y-unfold_icon.y+input_qq.height}
                        connection.target = component.createObject(input_qq, data);
                    }
                }
            }
        }
    }
    
    SvgView {
        id: soft_keyboard_icon
        width: defaultSize.width*myqq.windowScale
        source: "qrc:/images/soft-keyboard.svg"
        anchors.left: input_password.right
        anchors.leftMargin: -1/16*root.width
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
