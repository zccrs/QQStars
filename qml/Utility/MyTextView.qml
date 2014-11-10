import QtQuick 2.2
import QtWebKit 3.0

Item{
    id:root
    property string html: ""
    property alias url: webview.url
    
    implicitWidth: webview.contentWidth
    implicitHeight: webview.contentHeight
    onHtmlChanged: {
        webview.loadHtml(html)
    }
    
    WebView{
        id: webview
        anchors.fill: parent
    }
}
