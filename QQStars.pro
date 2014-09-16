TEMPLATE = app
TARGET = QQStars
QT += widgets network webkit svg qml quick

INCLUDEPATH += \
    src \
    src/qxtglobalshortcut

SOURCES += main.cpp \
    src/mynetworkaccessmanagerfactory.cpp \
    src/utility.cpp \
    src/mywindow.cpp \
    src/mywebsocket.cpp \
    src/qqstars.cpp \
    src/systemtrayicon.cpp \
    src/threaddownloadimage.cpp \
    src/mysvgview.cpp \ 
    src/myimage.cpp

RESOURCES += \ # qml.qrc
    images.qrc

qml_data.source = qml
qml_data.target = ./
DEPLOYMENTFOLDERS += qml_data

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
    src/mywebsocket.h \
    src/qqstars.h \
    src/systemtrayicon.h \
    src/threaddownloadimage.h \
    src/mysvgview.h \ 
    src/myimage.h

OTHER_FILES += \
            qml/LoginPage/LoginCheckBox.qml \
            qml/LoginPage/LoginPage.qml \
            qml/LoginPage/main.qml \
    qml/LoginPage/LoginInputArea.qml \
    qml/MyWindow.qml \
    qml/SystemTray.qml \
    qml/WarningWindow.qml \
    qml/menuStyle.css \
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
    qml/MyButton.qml \
    qml/LoginPage/SettingPage.qml \
    qml/CodeInput.qml \
    qml/MyComboBox.qml \
    qml/api.js \
    qml/QQApi.qml \
    qml/MyComboBoxComponent.qml \
    qml/des.js

FORMS +=
