<<<<<<< .mine
import QtQuick 2.2
import QtGraphicalEffects 1.0

Item{
    id: root
    x: actualX+glowLeftWidth()
    y:actualY+glowTopHeight()
    
    width: item.width
    height: item.height
    property Item item: null
    onItemChanged: {
        item.parent=root
    }
    property int biasX: 0
    property int biasY: 0
    
    property alias glowRadius: glow.glowRadius
    property alias spread: glow.spread
    property alias color: glow.color
    property alias cornerRadius: glow.cornerRadius
    property alias cached: glow.cached
    property alias glowOpacity: glow.opacity
    property int actualX: 0
    property int actualY: 0

    function glowTopHeight() {
        return glowRadius+cornerRadius*0.6*(1-spread)-biasY
    }

    function glowBottomHeight() {
        return glow.y+glow.height+2*glowRadius+cornerRadius-glowTopHeight()-root.height
    }

    function glowLeftWidth() {
        return glowRadius+cornerRadius*0.6*(1-spread)-biasX
    }

    function glowRightWidth() {
        return glow.x+glow.width+2*glowRadius+cornerRadius-glowLeftWidth()-root.width
    }
    
    function actualWidth(){
        return glowLeftWidth()+glow.width+glowRightWidth()
    }
    function actualHeight(){
        return glowTopHeight()+glow.height+glowBottomHeight()
    }
    Component.onCompleted: {
        console.log(glowRightWidth())
        console.log(glowBottomHeight())
    }

    RectangularGlow {
        id: glow
        x: (biasX>0?biasX:0)
        y: (biasY>0?biasY:0)
        width: root.width-biasX
        height: root.height-biasY
    }
}
=======
import QtQuick 2.2
import QtGraphicalEffects 1.0

RectangularGlow {
    id: effect
    width: root.implicitWidth
    height: root.implicitHeight-glowRadius+10
    anchors.right: parent.right
    anchors.rightMargin: glowRadius+15
    y:glowRadius+15
    glowRadius: 50
    spread: 0.1
    color: "black"
    opacity: 0.75
}
>>>>>>> .r19
