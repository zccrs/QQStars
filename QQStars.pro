TEMPLATE = app
TARGET = QQStars
QT += widgets network webkit svg qml quick concurrent #gui-private

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
    src/mysocket.cpp \
    src/mymessagebox.cpp

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
    src/mysocket.h \
    src/mymessagebox.h

OTHER_FILES += \
            qml/LoginPage/LoginCheckBox.qml \
            qml/LoginPage/LoginPage.qml \
            qml/LoginPage/main.qml \
    qml/LoginPage/LoginInputArea.qml \
    qml/MyWindow.qml \
    qml/SystemTray.qml \
    qml/MyMessageBox.qml \
    style/menuStyle.css \
    style/messageBoxStyle.css \
    qml/LoginPage/SoftKeyboard.qml \
    qml/LoginPage/SoftKeyboardButton.qml \
    qml/LoginPage/AccountList.qml \
    qml/MainPanelPage/main.qml \
    qml/MainPanelPage/GroupList.qml \
    qml/MainPanelPage/RecentList.qml \
    qml/MainPanelPage/MyScrollView.qml \
    qml/MainPanelPage/FriendList.qml \
    qml/MainPanelPage/AllListPage.qml \
    qml/MainPanelPage/MainPanelPage.qml \
    qml/LoginPage/SettingPage.qml \
    qml/CodeInput.qml \
    qml/MyComboBox.qml \
    qml/api.js \
    qml/QQApi.qml \
    qml/MyComboBoxComponent.qml \
    qml/MyRectangularGlow.qml \
    qml/MyTextField.qml \
    qml/MainPanelPage/ChatWindowCommand.qml \
    qml/MyTextArea.qml \
    qml/MyLoginButton.qml \
    qml/MyButton.qml \
    qml/MainPanelPage/MessageListComponent.qml \
    qml/MainPanelPage/ChatWindow.qml \
    qml/MainPanelPage/GroupChatWindow.qml \
    qml/MainPanelPage/FriendChatWindow.qml
FORMS +=
