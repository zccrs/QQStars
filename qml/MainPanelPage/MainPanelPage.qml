import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import mywindow 1.0

Rectangle{
    id: root
    radius: 10
    anchors.fill: parent
    border.width:1
    border.color:"#f07000"
    gradient: Gradient {
                    GradientStop { position: 0 ; color:  "#EEEDEC" }
                    GradientStop { position: 120/root.height ; color:  "#E7E5E4" }
                    GradientStop { position: 120/root.height+0.01 ; color:  "#f9f9f8" }
                    GradientStop { position: 1 ; color:  "#f9f9f8" }
                }
    SvgView{
        id:image_quit_icon
        source: "qrc:/images/button-quit.svg"
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.margins: 10
        MouseArea{
            anchors.fill: parent
            onClicked: Qt.quit()
        }
    }
    Text{
        id: text_nick
        x: 10
        anchors.top: image_quit_icon.bottom
        anchors.topMargin: 10
        font.pointSize: 16
        font.bold: true
        color: "black"
        text: myqq.userData.nick
    }
    Text{
        id: text_signature
        anchors.left: text_nick.left
        anchors.top: text_nick.bottom
        anchors.topMargin: 10
        color: "black"
        font.pointSize: 8
        Component.onCompleted: {
            myqq.getQQSignature(myqq.userQQ, getMeSignature)//去网络请求签名
        }

        function getMeSignature(data){//获得自己的个性签名成功后
            data = JSON.parse(data)
            if( data.retcode==0 ){
                text = data.result[0].lnick
            }
        }
    }
    AllListPage{
        anchors.top: text_signature.bottom
        anchors.topMargin: 1
        width: root.width-4
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 2
    }
}
