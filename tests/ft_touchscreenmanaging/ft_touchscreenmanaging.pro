include(../common_top.pri)

TEMPLATE = app
CONFIG += console meegofeedback link_pkgconfig
QT += network
PKGCONFIG += xcb
TARGET = ft_touchscreenmanaging
INCLUDEPATH += ../../src/mfsource ../../src/common
SRCDIR = ../../src

SOURCES += \
    main.cpp \
    mftouchscreenlistener_mock.cpp \
    mfxlistener_mock.cpp \
    mfserver_mock.cpp \
    $$SRCDIR/mfsource/mfreactorsource.cpp \
    $$SRCDIR/mfsource/mfreactionmap.cpp \
    $$SRCDIR/mfsource/mfreactionmapstack.cpp \
    $$SRCDIR/common/mfsettings.cpp

HEADERS += \
    mftouchscreenlistener_mock.h \
    mfxlistener_mock.h \
    mfserver_mock.h \
    $$SRCDIR/mfsource/mftouchscreenlistener.h \
    $$SRCDIR/mfsource/mfreactorsource.h \
    $$SRCDIR/mfsource/mfreactionmap.h \
    $$SRCDIR/mfsource/mfreactionmapstack.h \
    $$SRCDIR/mfsource/mfserver.h \
    $$SRCDIR/mfsource/mfxlistener.h \
    $$SRCDIR/common/mfsettings.h

target.path = $$[QT_INSTALL_LIBS]/libmeegoreactionmap-tests
INSTALLS += target

include(../common_bot.pri)
