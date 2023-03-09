QT       += core gui
QT += winextras
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QT+=gui
QT+=svg
CONFIG += c++14
QT+=multimedia
# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
#
SOURCES += \
    audio.cpp \
    audioin.cpp \
    audiorecoder.cpp \
    dxgimanager.cpp \
    main.cpp \
    pcm_to_aac.cpp \
    start.cpp \
    startpushbutton.cpp \
    stlabel.cpp \
    tovideo.cpp \
    widget.cpp

HEADERS += \
    audio.h \
    audioin.h \
    audiorecoder.h \
    dxgimanager.h \
    mix.h \
    pcm_to_aac.h \
    start.h \
    startpushbutton.h \
    stlabel.h \
    thread_pool.h \
    tovideo.h \
    widget.h

FORMS += \
    start.ui \
    widget.ui
LIBS += $$QMAKE_LIBS_GUI
#LIBS += -ld3d9 -lD3dx9
LIBS += -lD3D11 -lDXGI

DEFINES += __STDC_FORMAT_MACROS
#LIBS+=d3dx9.lib
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32: LIBS += -L$$PWD/../../ffmpeg/ffmpeg/lib/ -lavcodec

INCLUDEPATH += $$PWD/../../ffmpeg/ffmpeg/include
DEPENDPATH += $$PWD/../../ffmpeg/ffmpeg/include

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../../ffmpeg/ffmpeg/lib/avcodec.lib
#else:win32-g++: PRE_TARGETDEPS += $$PWD/../../ffmpeg/ffmpeg/lib/libavcodec.a

win32: LIBS += -L$$PWD/../../ffmpeg/ffmpeg/lib/ -lavdevice

INCLUDEPATH += $$PWD/../../ffmpeg/ffmpeg/include
DEPENDPATH += $$PWD/../../ffmpeg/ffmpeg/include

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../../ffmpeg/ffmpeg/lib/avdevice.lib
#else:win32-g++: PRE_TARGETDEPS += $$PWD/../../ffmpeg/ffmpeg/lib/libavdevice.a

win32: LIBS += -L$$PWD/../../ffmpeg/ffmpeg/lib/ -lavfilter

INCLUDEPATH += $$PWD/../../ffmpeg/ffmpeg/include
DEPENDPATH += $$PWD/../../ffmpeg/ffmpeg/include

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../../ffmpeg/ffmpeg/lib/avfilter.lib
#else:win32-g++: PRE_TARGETDEPS += $$PWD/../../ffmpeg/ffmpeg/lib/libavfilter.a

win32: LIBS += -L$$PWD/../../ffmpeg/ffmpeg/lib/ -lavformat

INCLUDEPATH += $$PWD/../../ffmpeg/ffmpeg/include
DEPENDPATH += $$PWD/../../ffmpeg/ffmpeg/include

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../../ffmpeg/ffmpeg/lib/avformat.lib
#else:win32-g++: PRE_TARGETDEPS += $$PWD/../../ffmpeg/ffmpeg/lib/libavformat.a

win32: LIBS += -L$$PWD/../../ffmpeg/ffmpeg/lib/ -lavutil

INCLUDEPATH += $$PWD/../../ffmpeg/ffmpeg/include
DEPENDPATH += $$PWD/../../ffmpeg/ffmpeg/include

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../../ffmpeg/ffmpeg/lib/avutil.lib
#else:win32-g++: PRE_TARGETDEPS += $$PWD/../../ffmpeg/ffmpeg/lib/libavutil.a

win32: LIBS += -L$$PWD/../../ffmpeg/ffmpeg/lib/ -lpostproc

INCLUDEPATH += $$PWD/../../ffmpeg/ffmpeg/include
DEPENDPATH += $$PWD/../../ffmpeg/ffmpeg/include

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../../ffmpeg/ffmpeg/lib/postproc.lib
#else:win32-g++: PRE_TARGETDEPS += $$PWD/../../ffmpeg/ffmpeg/lib/libpostproc.a

win32: LIBS += -L$$PWD/../../ffmpeg/ffmpeg/lib/ -lswresample

INCLUDEPATH += $$PWD/../../ffmpeg/ffmpeg/include
DEPENDPATH += $$PWD/../../ffmpeg/ffmpeg/include

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../../ffmpeg/ffmpeg/lib/swresample.lib
#else:win32-g++: PRE_TARGETDEPS += $$PWD/../../ffmpeg/ffmpeg/lib/libswresample.a

win32: LIBS += -L$$PWD/../../ffmpeg/ffmpeg/lib/ -lswscale

INCLUDEPATH += $$PWD/../../ffmpeg/ffmpeg/include
DEPENDPATH += $$PWD/../../ffmpeg/ffmpeg/include

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../../ffmpeg/ffmpeg/lib/swscale.lib
#else:win32-g++: PRE_TARGETDEPS += $$PWD/../../ffmpeg/ffmpeg/lib/libswscale.a

RESOURCES += \
    icon.qrc







