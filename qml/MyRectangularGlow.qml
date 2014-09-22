import QtQuick 2.2

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
    
    property alias glowRadius: rootItem.glowRadius
    property alias spread: rootItem.spread
    property alias color: rootItem.color
    property alias cornerRadius: rootItem.cornerRadius
    property alias cached: rootItem.cached
    property alias glowOpacity: rootItem.opacity
    property int actualX: 0
    property int actualY: 0

    function glowTopHeight() {
        return glowRadius+cornerRadius*0.6*(1-spread)-biasY
    }

    function glowBottomHeight() {
        return rootItem.y+rootItem.height+2*glowRadius+cornerRadius-glowTopHeight()-root.height
    }

    function glowLeftWidth() {
        return glowRadius+cornerRadius*0.6*(1-spread)-biasX
    }

    function glowRightWidth() {
        return rootItem.x+rootItem.width+2*glowRadius+cornerRadius-glowLeftWidth()-root.width
    }
    
    function actualWidth(){
        return glowLeftWidth()+rootItem.width+glowRightWidth()
    }
    function actualHeight(){
        return glowTopHeight()+rootItem.height+glowBottomHeight()
    }

    RectangularGlow {
        id: rootItem
        x: (biasX>0?biasX:0)
        y: (biasY>0?biasY:0)
        width: root.width-biasX
        height: root.height-biasY
    }
}
