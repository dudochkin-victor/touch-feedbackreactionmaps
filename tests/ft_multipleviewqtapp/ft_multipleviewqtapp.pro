TEMPLATE = app
TARGET =
DEPENDPATH += .

QMAKE_LFLAGS += "-Wl,--as-needed"

INCLUDEPATH += ../../src/client
LIBS += ../../libmeegoreactionmap.so

# Input
SOURCES += main.cpp testapplication.cpp
HEADERS += testapplication.h

QMAKE_EXTRA_TARGETS += check
check.depends = $$TARGET
check.commands = $$system(true)

QMAKE_EXTRA_TARGETS += check-xml
check-xml.depends = $$TARGET
check-xml.commands = $$system(true)
