INCLUDEPATH += $$PWD  
DEPENDPATH += $$PWD  
 
HEADERS += $$PWD/qxtglobal.h \  
           $$PWD/qxtglobalshortcut.h \  
           $$PWD/qxtglobalshortcut_p.h \
           $$PWD/myshortcut.h
SOURCES += $$PWD/qxtglobalshortcut.cpp \
           $$PWD/myshortcut.cpp
win32{  
    SOURCES += $$PWD/qxtglobalshortcut_win.cpp  
    LIBS += -luser32  
}  
unix{
    QT += x11extras
    LIBS += -lX11
    SOURCES += $$PWD/qxtglobalshortcut_x11.cpp
}
mac{
    SOURCES += $$PWD/qxtglobalshortcut_mac.cpp 
}
