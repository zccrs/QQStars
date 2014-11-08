import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQuick.Controls.Private 1.0

ScrollView {
    id: area
    
    property alias activeFocusOnPress: edit.activeFocusOnPress
    property alias baseUrl: edit.baseUrl
    readonly property alias canPaste: edit.canPaste
    readonly property alias canRedo: edit.canRedo
    readonly property alias canUndo: edit.canUndo
    property alias textColor: edit.color
    property alias cursorPosition: edit.cursorPosition
    property alias font: edit.font
    property alias horizontalAlignment: edit.horizontalAlignment
    readonly property alias effectiveHorizontalAlignment: edit.effectiveHorizontalAlignment
    property alias verticalAlignment: edit.verticalAlignment
    property alias inputMethodHints: edit.inputMethodHints
    readonly property alias length: edit.length
    readonly property alias lineCount: edit.lineCount
    property alias readOnly: edit.readOnly
    Accessible.readOnly: readOnly
    readonly property alias selectedText: edit.selectedText
    readonly property alias selectionEnd: edit.selectionEnd
    readonly property alias selectionStart: edit.selectionStart
    property bool tabChangesFocus: false
    property alias text: edit.text
    property alias textFormat: edit.textFormat
    property alias wrapMode: edit.wrapMode
    property alias selectByMouse: edit.selectByMouse
    property alias selectByKeyboard: edit.selectByKeyboard
    signal linkActivated(string link)
    signal linkHovered(string link)
    readonly property alias hoveredLink: edit.hoveredLink
    property alias textDocument: edit.textDocument
    default property alias data: area.data
    property alias textMargin: edit.textMargin
    
    frameVisible: true
    activeFocusOnTab: true
    Accessible.role: Accessible.EditableText
    
    function append (string) {
        edit.append(string)
        __verticalScrollBar.value = __verticalScrollBar.maximumValue
    }

    function copy() {
        edit.copy();
    }

    function cut() {
        edit.cut();
    }

    function deselect() {
        edit.deselect();
    }

    function getFormattedText(start, end) {
        return edit.getFormattedText(start, end);
    }
    
    function getText(start, end) {
        return edit.getText(start, end);
    }

    function insert(position, text) {
        edit.insert(position, text);
    }

    function isRightToLeft(start, end) {
        return edit.isRightToLeft(start, end);
    }

    function moveCursorSelection(position, mode) {
        edit.moveCursorSelection(position, mode);
    }

    function paste() {
        edit.paste();
    }

    function positionAt(x, y) {
        return edit.positionAt(x, y);
    }

    function positionToRectangle(position) {
        return edit.positionToRectangle(position);
    }

    function redo() {
        edit.redo();
    }

    function remove(start, end) {
        return edit.remove(start, end);
    }

    function select(start, end) {
        edit.select(start, end);
    }

    function selectAll() {
        edit.selectAll();
    }

    function selectWord() {
        edit.selectWord();
    }

    function undo() {
        edit.undo();
    }

    style: ScrollViewStyle {
        id: scroll_style
        //textColor: "black"
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

    Flickable {
        id: flickable

        interactive: false
        anchors.fill: parent

        TextEdit {
            id: edit
            focus: true

            property int layoutRecursionDepth: 0
            textFormat :TextEdit.RichText
            function doLayout() {
                // scrollbars affect the document/viewport size and vice versa, so we
                // must allow the layout loop to recurse twice until the sizes stabilize
                if (layoutRecursionDepth <= 2) {
                    layoutRecursionDepth++

                    if (wrapMode == TextEdit.NoWrap) {
                        __horizontalScrollBar.visible = edit.contentWidth > viewport.width
                        edit.width = Math.max(viewport.width, edit.contentWidth)
                    } else {
                        __horizontalScrollBar.visible = false
                        edit.width = viewport.width
                    }
                    edit.height = Math.max(viewport.height, edit.contentHeight)

                    flickable.contentWidth = edit.contentWidth
                    flickable.contentHeight = edit.contentHeight

                    layoutRecursionDepth--
                }
            }
            
            Connections {
                target: area.viewport
                onWidthChanged: edit.doLayout()
                onHeightChanged: edit.doLayout()
            }
            onContentWidthChanged: edit.doLayout()
            onContentHeightChanged: edit.doLayout()
            onWrapModeChanged: edit.doLayout()

            renderType: Text.NativeRendering
            font: TextSingleton.font
            color: "black"
            selectionColor: "#888"
            selectedTextColor: "white"
            wrapMode: TextEdit.WordWrap
            textMargin: 4

            selectByMouse: Qt.platform.os !== "android" // Workaround for QTBUG-36515
            readOnly: false

            Keys.forwardTo: area

            KeyNavigation.priority: KeyNavigation.BeforeItem
            KeyNavigation.tab: area.tabChangesFocus ? area.KeyNavigation.tab : null
            KeyNavigation.backtab: area.tabChangesFocus ? area.KeyNavigation.backtab : null

            // keep textcursor within scroll view
            onCursorPositionChanged: {
                if (cursorRectangle.y >= flickableItem.contentY + viewport.height - cursorRectangle.height - textMargin) {
                    // moving down
                    flickableItem.contentY = cursorRectangle.y - viewport.height +  cursorRectangle.height + textMargin
                } else if (cursorRectangle.y < flickableItem.contentY) {
                    // moving up
                    flickableItem.contentY = cursorRectangle.y - textMargin
                }

                if (cursorRectangle.x >= flickableItem.contentX + viewport.width - textMargin) {
                    // moving right
                    flickableItem.contentX = cursorRectangle.x - viewport.width + textMargin
                } else if (cursorRectangle.x < flickableItem.contentX) {
                    // moving left
                    flickableItem.contentX = cursorRectangle.x - textMargin
                }
            }
            onLinkActivated: area.linkActivated(link)
            onLinkHovered: area.linkHovered(link)

            MouseArea {
                parent: area.viewport
                anchors.fill: parent
                cursorShape: edit.hoveredLink ? Qt.PointingHandCursor : Qt.IBeamCursor
                acceptedButtons: Qt.NoButton
            }
        }
    }

    Keys.onPressed: {
        if (event.key == Qt.Key_PageUp) {
            __verticalScrollBar.value -= area.height
        } else if (event.key == Qt.Key_PageDown)
            __verticalScrollBar.value += area.height
    }
}
