TEMPLATE = app
TARGET = QQStars
QT += widgets network webkit svg qml quick concurrent sql#gui-private

INCLUDEPATH += \
    src \
    src/qxtglobalshortcut \
    src/aes

SOURCES += main.cpp \
    src/mynetworkaccessmanagerfactory.cpp \
    src/utility.cpp \
    src/mywindow.cpp \
    src/qqstars.cpp \
    src/systemtrayicon.cpp \
    src/threaddownloadimage.cpp \
    src/mysvgview.cpp \ 
    src/myimage.cpp \
    src/mymessagebox.cpp \
    src/myhttprequest.cpp \
    src/myshortcut.cpp

RESOURCES += \ # qml.qrc
    images.qrc \
    other.qrc

qml_data.source = qml
qml_data.target = ./

style_data.source = style
style_data.target = ./

DEPLOYMENTFOLDERS += qml_data style_data

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)
include (src/qxtglobalshortcut/qxtglobalshortcut.pri)
include (qmlapplicationviewer.pri)
qtcAddDeployment()
HEADERS += \
    src/mynetworkaccessmanagerfactory.h \
    src/utility.h \
    src/mywindow.h \
    src/qqstars.h \
    src/systemtrayicon.h \
    src/threaddownloadimage.h \
    src/mysvgview.h \ 
    src/myimage.h \
    src/mymessagebox.h \
    src/myhttprequest.h \
    src/myshortcut.h

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
    qml/Chat/ChatWindow.qml \
    qml/Chat/ChatWindowCommand.qml \
    qml/Chat/FriendChatWindow.qml \
    qml/Chat/GroupChatWindow.qml \
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
    qml/Chat/DiscuChatWindow.qml \
    style/menuStyle.css \
    style/messageBoxStyle.css
            
FORMS +=
