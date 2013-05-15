include(../common_top.pri)
TARGET = ut_mfeedbackpalette

INCLUDEPATH += ../../src/client

TEST_SOURCES = ../../src/client/mfeedbackpalette.cpp

SOURCES += ut_mfeedbackpalette.cpp \
    mfutil_mock.cpp \
    $$TEST_SOURCES

HEADERS += ut_mfeedbackpalette.h \
    mfutil.h \
    mfutil_mock.h \
    ../../src/client/mfeedbackpalette.h

include(../common_bot.pri)

target.path = $$[QT_INSTALL_LIBS]/libmeegoreactionmap-tests
INSTALLS += target
