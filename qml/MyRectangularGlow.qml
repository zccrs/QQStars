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
