TEMPLATE = app
INCLUDEPATH += ../../src/common \
    ../../src/client

QMAKE_LFLAGS += "-Wl,--as-needed"

# Use pkg-config or something instead of hardcoded paths
INCLUDEPATH += /usr/include/meegofeedback
LIBS += -lmeegofeedback
SOURCES += mreactionmapviewer.cpp \
    colormap.cpp \
    main.cpp \
    ../../src/client/mfeedbackpalette.cpp \
    ../../src/common/mfsettings.cpp
HEADERS += mreactionmapviewer.h \
    colormap.h \
    ../../src/client/mfeedbackpalette.h \
    ../../src/common/mfsettings.h
target.path = /usr/bin
INSTALLS += target
QMAKE_EXTRA_TARGETS += check
check.depends = $$TARGET
check.commands = $$system(true)
QMAKE_EXTRA_TARGETS += check-xml
check-xml.depends = $$TARGET
check-xml.commands = $$system(true)
