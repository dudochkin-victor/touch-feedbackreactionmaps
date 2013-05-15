include(check.pri)

INCLUDEPATH += ../../src/common
#DEPENDPATH = $$INCLUDEPATH
CONFIG += debug
QT += testlib
QMAKE_LFLAGS += "-Wl,--as-needed"
TEMPLATE = app
# DEFINES += QT_NO_DEBUG_OUTPUT
target.path = /usr/lib/tests/meegofeedback-reactionmaps-tests
INSTALLS += target
