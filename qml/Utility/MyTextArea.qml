import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import MyTextEditPlugin 1.0

TextArea{
    id: root
    backgroundVisible : false
    textFormat :TextEdit.RichText
    
    /*TextEditPlayGif{
        cachePath: myqq.getMovieImageFrameCachePath()
        target: textDocument
    }*/

    style: TextAreaStyle {
            id: scroll_style
            textColor: "black"
            frame: Rectangle{
                color: "#f5f5f5"
                radius: 5
                border.width: 2
                border.color: "#ddd"
            }
            property int hovered_count: 0
            property bool hovered: hovered_count>0
    
            handle: Rectangle{
                radius: 10
                implicitWidth: 10
                property bool hovered: styleData.hovered
                onHoveredChanged: {
                    if( hovered )
                        scroll_style.hovered_count++
                    else
                        scroll_style.hovered_count--
                }
    
                color: scroll_style.hovered?"#808080":"#BDBFBE"
            }
            scrollBarBackground:Rectangle{
                implicitWidth: 10
                color:"#f0f0f0"
                radius: 10
                property bool hovered: styleData.hovered
                onHoveredChanged: {
                    if( hovered )
                        scroll_style.hovered_count++
                    else
                        scroll_style.hovered_count--
                }
                opacity: scroll_style.hovered?1:0
                Behavior on opacity{
                    NumberAnimation{
                        duration: 200
                    }
                }
            }
            decrementControl : Item{
                implicitHeight: 10
                implicitWidth: 10
                Image{
                    source: "qrc:/images/list_arrow_up.png"
                    property bool hovered: styleData.hovered
                    anchors.centerIn: parent
                    onHoveredChanged: {
                        if( hovered )
                            scroll_style.hovered_count++
                        else
                            scroll_style.hovered_count--
                    }
                    opacity: scroll_style.hovered?1:0
                    Behavior on opacity{
                        NumberAnimation{
                            duration: 200
                        }
                    }
                }
            }
    
            incrementControl : Item{
                implicitHeight: 10
                implicitWidth: 10
                Image{
                    anchors.centerIn: parent
                    source: "qrc:/images/list_arrow_down.png"
                    property bool hovered: styleData.hovered
                    onHoveredChanged: {
                        if( hovered )
                            scroll_style.hovered_count++
                        else
                            scroll_style.hovered_count--
                    }
                    opacity: scroll_style.hovered?1:0
                    Behavior on opacity{
                        NumberAnimation{
                            duration: 200
                        }
                    }
                }
            }
        }
}
