TARGET = ft_mfxlistener

INCLUDEPATH += ../../src/mfsource ../../src/common
CONFIG += link_pkgconfig
PKGCONFIG += xcb

QMAKE_CXXFLAGS += -fno-strict-aliasing
QMAKE_LFLAGS += "-Wl,--as-needed"

SOURCES += \
    main.cpp \
    logger.cpp \
    mfutil_mock.cpp \
    ../../src/mfsource/mfxlistener.cpp \
    ../../src/mfsource/mfxlistener_p.cpp \
    ../../src/mfsource/mfreactionmapstack.cpp \
    ../../src/mfsource/mfreactionmap.cpp \
    ../../src/common/mfsettings.cpp

HEADERS += \
    logger.h \
    mfutil.h \
    mfutil_mock.h \
    mffeedback.h \
    mffeedback_mock.h \
    mfsession.h \
    mfsession_mock.h \
    ../../src/mfsource/mfxlistener.h \
    ../../src/mfsource/mfxlistener_p.h \
    ../../src/mfsource/mfreactionmapstack.h \
    ../../src/mfsource/mfreactionmap.h \
    ../../src/common/mfcommondata.h \
    ../../src/common/mfsettings.h

target.path = $$[QT_INSTALL_LIBS]/libmeegoreactionmap-tests
INSTALLS += target

QMAKE_EXTRA_TARGETS += check
check.depends = $$TARGET
check.commands = $$system(true)

QMAKE_EXTRA_TARGETS += check-xml
check-xml.depends = $$TARGET
check-xml.commands = $$system(true)
