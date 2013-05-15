include(mfsource_common.pri)

TEMPLATE = lib
DESTDIR = ../..
TARGET = meegofeedback-reactionmap

# Find out which implementation of MfTouchScreenListener to use
# NOTE: XRecord version is built separately in ../mfsource_xrecord
contains(TOUCHSCREEN_LISTENER_IMPL, dummy) {
    message(Using *dummy* implementation of MfTouchScreenListener)
    SOURCES += mftouchscreenlistener_dummy.cpp
}
else {
   contains(TOUCHSCREEN_LISTENER_IMPL, xcore) {
        message(Using *core X* implementation of MfTouchScreenListener)
        CONFIG += x11
        SOURCES += \
        mftouchscreenlistener_xcore.cpp \
        mfxcorethread.cpp
        HEADERS += \
        mfxcorethread.h
    }
    else {
        # defaults to kernel input event
        message(Using *kernel input event* implementation of MfTouchScreenListener)
        SOURCES += \
        mfkernelthread.cpp \
        mftouchscreenlistener_kernel.cpp \
        mfdefaulttranslator.cpp
        HEADERS += \
        mfkernelthread.h \
        mfdefaulttranslator.h
    }
}
