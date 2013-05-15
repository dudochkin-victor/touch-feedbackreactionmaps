include(../common_top.pri)
TARGET = ut_colormap

INCLUDEPATH += ../../tools/meegoreactionmap-viewer

TEST_SOURCES = \
    ../../tools/meegoreactionmap-viewer/colormap.cpp \

SOURCES += \
    ut_colormap.cpp \
    $$TEST_SOURCES

HEADERS += \
    ut_colormap.h \
    ../../tools/meegoreactionmap-viewer/colormap.h \

include(../common_bot.pri)

target.path = $$[QT_INSTALL_LIBS]/libmeegoreactionmap-tests
INSTALLS += target
