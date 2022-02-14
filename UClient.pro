QT += core gui
QT += multimedia
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Client/tcpclient.cpp \
    Client/udpclient.cpp \
    Controls/ctrlexpandbtn.cpp \
    Controls/ctrlinfowidget.cpp \
    Controls/ctrllivecentre.cpp \
    Controls/ctrllivecontrol.cpp \
    Controls/ctrlmenu.cpp \
    Tools/toolaudio.cpp \
    Tools/toolgrapwindow.cpp \
    Tools/tooljudgeequipment.cpp \
    Tools/toolnetstate.cpp \
    Tools/toolsound.cpp \
    Tools/toolvideo.cpp \
    VideoWidget//videopanel.cpp \
    VideoWidget/videowidget.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    Client/sockcmd.h \
    Client/tcpclient.h \
    Client/udpclient.h \
    Controls/ctrlexpandbtn.h \
    Controls/ctrlinfowidget.h \
    Controls/ctrllivecentre.h \
    Controls/ctrllivecontrol.h \
    Controls/ctrlmenu.h \
    Tools/toolaudio.h \
    Tools/toolgrapwindow.h \
    Tools/tooljudgeequipment.h \
    Tools/toolnetstate.h \
    Tools/toolscreen.h \
    Tools/toolsound.h \
    Tools/toolvideo.h \
    VideoWidget/videopanel.h \
    VideoWidget/videowidget.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

LIBS += -lpthread libwsock32 libws2_32
LIBS += -lWinMM

INCLUDEPATH += C:\Opencv\OpenCV-MinGW-Build-OpenCV-4.5.2-x64\include
LIBS += C:\Opencv\opencv\opencv_build\lib\libopencv_*.a

#CONFIG      += console

RESOURCES += \
    resource.qrc
