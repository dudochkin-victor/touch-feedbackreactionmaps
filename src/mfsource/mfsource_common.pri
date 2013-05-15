DEPENDPATH += .
INCLUDEPATH += . \
    ../common
CONFIG += link_pkgconfig plugin meegofeedback
QT += network dbus
PKGCONFIG += xcb
QMAKE_CXXFLAGS += -finline-functions -fno-strict-aliasing
QMAKE_LFLAGS += "-Wl,--as-needed"
CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT
else:DEFINES += MF_DEBUG

contains( COV_OPTION, on ) {
    include(../coverage.pri)
}

# Input
HEADERS += \
    ../mfsource/mfreactionmap.h \
    ../mfsource/mfreactionmapstack.h \
    ../mfsource/mfreactorsource.h \
    ../mfsource/mfserver.h \
    ../mfsource/mftouchscreenlistener.h \
    ../mfsource/mftouchscreentranslator.h \
    ../mfsource/mfxlistener_p.h \
    ../mfsource/mfxlistener.h \
    ../mfsource/mfconnection.h \
    ../common/mfsettings.h \
    ../common/mfcommondata.h

SOURCES += \
    ../mfsource/mfreactionmap.cpp \
    ../mfsource/mfreactionmapstack.cpp \
    ../mfsource/mfreactorsource.cpp \
    ../mfsource/mfserver.cpp \
    ../mfsource/mfxlistener_p.cpp \
    ../mfsource/mfxlistener.cpp \
    ../mfsource/mfconnection.cpp \
    ../common/mfsettings.cpp

QMAKE_EXTRA_TARGETS += check
check.depends = $$DESTDIR/lib$${TARGET}.so
check.commands = $$system(true)

QMAKE_EXTRA_TARGETS += check-xml
check-xml.depends = $$DESTDIR/lib$${TARGET}.so
check-xml.commands = $$system(true)

target.path = /usr/lib/meegofeedbackd
INSTALLS += target

