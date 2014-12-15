TEMPLATE = app
TARGET = QQStars

QT += widgets network webkit svg qml quick concurrent sql quick-private#widgets-private core-private gui-private
QT += webkitwidgets

INCLUDEPATH += \
    src \
    src/qxtglobalshortcut \
    src/qqstars \
    src/mywidgets \
    src/utility

SOURCES += src/main.cpp \
    src/utility/mynetworkaccessmanagerfactory.cpp \
    src/utility/utility.cpp \
    src/mywidgets/mywindow.cpp \
    src/qqstars/qqstars.cpp \
    src/mywidgets/systemtrayicon.cpp \
    src/mywidgets/mysvgview.cpp \ 
    src/mywidgets/myimage.cpp \
    src/mywidgets/mymessagebox.cpp \
    src/utility/myhttprequest.cpp \
    src/qqstars/qqiteminfo.cpp \
    src/utility/downloadimage.cpp \
    src/utility/texteditplaygif.cpp

RESOURCES += \
    images.qrc \
    other.qrc \
    faces.qrc \
    qml.qrc \
    style.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)
include (src/qxtglobalshortcut/qxtglobalshortcut.pri)
include (qmlapplicationviewer.pri)
qtcAddDeployment()
HEADERS += \
    src/utility/mynetworkaccessmanagerfactory.h \
    src/utility/utility.h \
    src/mywidgets/mywindow.h \
    src/qqstars/qqstars.h \
    src/mywidgets/systemtrayicon.h \
    src/mywidgets/mysvgview.h \ 
    src/mywidgets/myimage.h \
    src/mywidgets/mymessagebox.h \
    src/utility/myhttprequest.h \
    src/qqstars/qqiteminfo.h \
    src/utility/downloadimage.h \
    src/utility/texteditplaygif.h

OTHER_FILES += \
    qml/Utility/CodeInput.qml \
    qml/Utility/MyButton.qml \
    qml/Utility/MyMessageBox.qml \
    qml/Utility/MyTextArea.qml \
    qml/Utility/MyTextField.qml \
    qml/Utility/SystemTray.qml \
    qml/Api/api.js \
    qml/Api/QQApi.qml \
    qml/Login/KeyboardPage/SoftKeyboard.qml \
    qml/Login/KeyboardPage/SoftKeyboardButton.qml \
    qml/Login/main.qml \
    qml/Login/SettingPage.qml \
    qml/MainPanel/ChatWindow/qqshow.png \
    qml/MainPanel/ListPage/AllListPage.qml \
    qml/MainPanel/ListPage/FriendList.qml \
    qml/MainPanel/ListPage/GroupList.qml \
    qml/MainPanel/ListPage/RecentList.qml \
    qml/MainPanel/main.qml \
    qml/MainPanel/MainPanelPage.qml \
    qml/Utility/ComboBox/MyComboBox.qml \
    qml/Utility/ComboBox/MyComboBoxComponent.qml \
    qml/Login/MyLoginButton.qml \
    qml/Chat/ChatWindowCommand.qml \
    qml/Chat/MessageListComponent.qml \
    qml/Chat/qqshow.png \
    qml/Utility/KeyboardPage/SoftKeyboard.qml \
    qml/Utility/KeyboardPage/SoftKeyboardButton.qml \
    qml/Utility/MyRectangularGlow.qml \
    qml/Utility/MyWindow.qml \
    qml/Utility/MyScrollView.qml \
    qml/Login/LoginPanel/AccountList.qml \
    qml/Login/LoginPanel/LoginCheckBox.qml \
    qml/Login/LoginPanel/LoginInputArea.qml \
    qml/Login/LoginPanel/LoginPage.qml \
    qml/QQItemInfo/DiscuInfo.qml \
    qml/QQItemInfo/FriendInfo.qml \
    qml/QQItemInfo/GroupInfo.qml \
    qml/MainPanel/ListPage/DiscuList.qml \
    qml/MainPanel/ListPage/GroupAndDiscuPage.qml \
    style/menuStyle.css \
    style/messageBoxStyle.css \
    qml/Chat/FriendChatPage.qml \
    qml/Chat/GroupChatPage.qml \
    qml/Chat/DiscuChatPage.qml \
    qml/Chat/ChatPage.qml \
    qml/Utility/TrayMessageWindow.qml \
    qml/Login/textedit.html \
    qml/Utility/MyTextView.qml
            
FORMS +=
