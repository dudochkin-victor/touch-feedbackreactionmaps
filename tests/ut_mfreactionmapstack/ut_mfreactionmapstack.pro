include(../common_top.pri)
TARGET = ut_mfreactionmapstack
INCLUDEPATH += ../../src/mfsource
DEFINES += FR_LOCAL_REACTIONMAP
TEST_SOURCES = ../../src/mfsource/mfreactionmapstack.cpp
SOURCES += ut_mfreactionmapstack.cpp \
    ../../src/mfsource/mfreactionmap.cpp \
    ../../src/common/mfsettings.cpp \
    mfutil_mock.cpp \
    $$TEST_SOURCES
HEADERS += ut_mfreactionmapstack.h \
    mfutil.h \
    mfutil_mock.h \
    mfsession.h \
    mfsession_mock.h \
    mffeedback.h \
    mffeedback_mock.h \
    ut_mfreactionmapstack.h \
    ../../src/mfsource/mfreactionmapstack.h \
    ../../src/mfsource/mfreactionmap.h \
    ../../src/common/mfsettings.h \
    mffeedback_mock.h \
    mfsession_mock.h
include(../common_bot.pri)

target.path = $$[QT_INSTALL_LIBS]/libmeegoreactionmap-tests
INSTALLS += target
