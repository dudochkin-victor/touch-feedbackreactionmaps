TEMPLATE = lib
DESTDIR = ../..
TARGET = meegoreactionmap

DEPENDPATH += .

INCLUDEPATH += . \
    ../common

QT += network
CONFIG += meegofeedback
QMAKE_CXXFLAGS_RELEASE += -finline-functions
QMAKE_LFLAGS += "-Wl,--as-needed"
CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT
VERSION = 0.13.0
PUBLIC_HEADERS = mreactionmap.h

contains( COV_OPTION, on ) {
    include(../coverage.pri)
}

HEADERS = mreactionmap.h \
    mreactionmap_p.h \
    mfeedbackpalette.h \
    mreactionmapconnection.h \
    $$PUBLIC_HEADERS \
    ../common/mfsettings.h \
    ../common/mfcommondata.h

SOURCES = mreactionmap.cpp \
    mreactionmap_p.cpp \
    mfeedbackpalette.cpp \
    mreactionmapconnection.cpp \
    ../common/mfsettings.cpp

headers.path = /usr/include/meegoreactionmap
headers.files = $$PUBLIC_HEADERS
target.path = /usr/lib

install_prf.path = $$[QT_INSTALL_DATA]/mkspecs/features
install_prf.files = meegoreactionmap.prf

INSTALLS += target headers install_prf

QMAKE_EXTRA_TARGETS += check
check.depends = $$DESTDIR/lib$${TARGET}.so.$${VERSION}
check.commands = $$system(true)

QMAKE_EXTRA_TARGETS += check-xml
check-xml.depends = $$DESTDIR/lib$${TARGET}.so.$${VERSION}
check-xml.commands = $$system(true)
