import QtQuick 2.2
import QtQuick.Window 2.1
import mywindow 1.0

Window{
    id:root
    visible: true
    property bool is: false
    x: utility.mouseDesktopPos().x - 200
    y: utility.mouseDesktopPos().y + 13
    width: 450*myqq.windowScale
    height: 2/7*width
    
    flags: Qt.SplashScreen
    signal input(var arg)
    signal backspace(var arg)
    signal keyboardClose
    Rectangle {
        anchors.fill: parent
        radius: 3
        border.width:1
        border.color: "#F07000"
        gradient: Gradient {
                        GradientStop { position: 0 ; color: "#E7E5E4" }
                        GradientStop { position: 1 ; color: "#D9D7D6" }
                    }
    }
    SvgView{
        width: defaultSize.width*myqq.windowScale
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.margins: 2
        source: "qrc:/images/inputBox-close.svg"
        MouseArea{
            anchors.fill: parent
            onClicked: {
                root.close()
                keyboardClose()
            }
        }
    }

    Component.onCompleted: {
        object.createKeyboard()
    }
    QtObject{
        id: object
        property string ascii1_1: "0123456789`-=[]\\;',./"
        property string ascii1_2: ")!@#$%^&*(~_+{}|:\"<>?"
        property string ascii2_1: "abcdefghijklmnopqrstuvwxyz"
        property string ascii2_2: "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        
        function randomOrder (targetArray)
        {
            var arrayLength = targetArray.length//先创建一个正常顺序的数组
            var tempArray1 = new Array();
            for (var i = 0; i < arrayLength; i ++){
                tempArray1 [i] = i
            }
            var tempArray2 = new Array();//再根据上一个数组创建一个随机乱序的数组
            for (var i = 0; i < arrayLength; i ++){//从正常顺序数组中随机抽出元素
                tempArray2 [i] = tempArray1.splice (Math.floor (Math.random () * tempArray1.length) , 1)
            }
            var tempArray3 = new Array();//最后创建一个临时数组存储 根据上一个乱序的数组从targetArray中取得数据
            for (var i = 0; i < arrayLength; i ++){
                tempArray3 [i] = targetArray [tempArray2 [i]]
            }
            return tempArray3//返回最后得出的数组
        }
        function createObject(parent, data) {
            var component = Qt.createComponent("SoftKeyboardButton.qml");
            if (component.status == Component.Ready){
                return component.createObject(parent, data);
            }
        }
        function createKeyboard(){
            var data = {"text1": "Shift", "isShiftButton": true,"width": root.width/8, "height":root.width/17}
            var shift = createObject(row2, data)
            var temp = [0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20];
            temp = randomOrder(temp)//随机排序
            for( var i=0;i<11;++i ){
                data = {"width": root.width/17, "height":root.width/17,"text1": ascii1_1[temp[i]],"text2": ascii1_2[temp[i]],"switchButton": shift,"backFun": input}
                createObject(row1, data);
            }
            data = {"text1": "Backspace","width": root.width/6, "height":root.width/17,"backFun": backspace}
            var backsoace = createObject(row1,data);
            for( i=11;i<21;++i ){
                data = {"width": root.width/17, "height":root.width/17,"text1": ascii1_1[temp[i]],"text2": ascii1_2[temp[i]],"switchButton": shift,"backFun": input}
                createObject(row2, data);
            }
            data = {"text1": "CapsLock","width": root.width/6, "height":root.width/17,"isShiftButton":true}
            var capslock = createObject(row2,data);
            
            temp = [0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25];
            temp = randomOrder(temp)//随机排序
            for( i=0;i<13;++i ){
                data = {"width": root.width/17, "height":root.width/17,"text1": ascii2_1[temp[i]],"text2": ascii2_2[temp[i]],"switchButton": capslock,"backFun": input}
                createObject(row3, data);
            }
            for( i=13;i<26;++i ){
                data = {"width": root.width/17, "height":root.width/17,"text1": ascii2_1[temp[i]],"text2": ascii2_2[temp[i]],"switchButton": capslock,"backFun": input}
                createObject(row4, data);
            }
        }
    }

    Grid{
        anchors.centerIn: parent
        spacing: 3
        rows:4
        horizontalItemAlignment : Qt.AlignHCenter
        Row{id:row1;spacing: 3}
        Row{id:row2;spacing: 3}
        Row{id:row3;spacing: 3}
        Row{id:row4;spacing: 3}
    }
    
    onFocusObjectChanged: {
        if( is ){
            root.close()
            keyboardClose()
        }
        else
            is = true
    }
}


