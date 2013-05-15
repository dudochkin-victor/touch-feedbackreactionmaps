include(../mfsource/mfsource_common.pri)

TEMPLATE = lib
DESTDIR = ../..
TARGET = meegofeedback-reactionmap-xrecord

# Use XRecord implementation of MfTouchScreenListener
message(Using *XRecord* implementation of MfTouchScreenListener)
CONFIG += x11
PKGCONFIG += xtst
SOURCES += \
           ../mfsource/mftouchscreenlistener_xrecord.cpp \
           ../mfsource/mfxrecordthread.cpp
HEADERS += \
           ../mfsource/mfxrecordthread.h

