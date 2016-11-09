#-------------------------------------------------
#
# Project created by QtCreator 2016-11-09T16:52:55
#
#-------------------------------------------------

QT       -= gui

TARGET = FFUser
TEMPLATE = lib
CONFIG += staticlib

win32{
    FFMPEGPATH = G:/Code/FFMPEG/ffmpeg
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
    ffdecode.cpp

HEADERS += ffuser.h \
    easy.h \
    ffmpeg.h \
    ffencode.h \
    ffdecode.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
