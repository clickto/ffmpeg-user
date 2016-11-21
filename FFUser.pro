#-------------------------------------------------
#
# Project created by QtCreator 2016-11-09T16:52:55
#
#-------------------------------------------------

QT  = gui core
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
#CONFIG(debug, debug|release) {
#    TARGET = FFUserD
#} else {
#    TARGET = FFUser
#}
#TEMPLATE = lib
#CONFIG += staticlib
CONFIG += c++11
win32{
    FFMPEGPATH = $$_PRO_FILE_PWD_/ffmpeg/win32
}
unix{
    FFMPEGPATH = /usr/local/ffmpeg/shared
}
LIBS += -L$${FFMPEGPATH}/lib -lavcodec \
    -lavdevice \
    -lavfilter \
    -lavformat \
    -lavutil \
    -lpostproc \
    -lswresample \
    -lswscale
INCLUDEPATH += $${FFMPEGPATH}/include
DEPENDPATH += $${FFMPEGPATH}/include


SOURCES += ffuser.cpp \
    ffmpeg.cpp \
    ffencode.cpp \
    ffdecode.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += ffuser.h \
    easy.h \
    ffmpeg.h \
    ffencode.h \
    ffdecode.h \
    mainwindow.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS += \
    mainwindow.ui
