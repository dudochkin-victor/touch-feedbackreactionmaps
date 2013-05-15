include(../common_top.pri)
TARGET = ut_mreactionmap

COPY_TESTED_SOURCES = ../../src/client/mreactionmap.cpp \
                      ../../src/client/mreactionmap_p.cpp
copysourcefiles.input = COPY_TESTED_SOURCES
copysourcefiles.output = ${QMAKE_FILE_BASE}.cpp
copysourcefiles.commands = $$QMAKE_COPY ${QMAKE_FILE_IN} ${QMAKE_FILE_BASE}.cpp
copysourcefiles.variable_out = SOURCES
copysourcefiles.CONFIG = target_predeps no_link
QMAKE_EXTRA_COMPILERS += copysourcefiles

TEST_SOURCES = $$COPY_TESTED_SOURCES

COPY_TESTED_HEADERS = ../../src/client/mreactionmap.h \
                      ../../src/client/mreactionmap_p.h
copyheaderfiles.input = COPY_TESTED_HEADERS
copyheaderfiles.output = ${QMAKE_FILE_BASE}.h .moc/moc_${QMAKE_FILE_BASE}.cpp
copyheaderfiles.commands = $$QMAKE_COPY ${QMAKE_FILE_IN} ${QMAKE_FILE_BASE}.h
copyheaderfiles.variable_out = HEADERS
copyheaderfiles.CONFIG = target_predeps no_link
QMAKE_EXTRA_COMPILERS += copyheaderfiles

MOC_TESTED_HEADERS = ../../src/client/mreactionmap_p.h
mocheaderfiles.input = MOC_TESTED_HEADERS
mocheaderfiles.output = moc_${QMAKE_FILE_BASE}.cpp
mocheaderfiles.commands = $$QMAKE_MOC $(DEFINES) $(INCPATH) ${QMAKE_FILE_BASE}.h -o moc_${QMAKE_FILE_BASE}.cpp
mocheaderfiles.variable_out = SOURCES
mocheaderfiles.CONFIG = target_predeps no_link
QMAKE_EXTRA_COMPILERS += mocheaderfiles

SOURCES += ut_mreactionmap.cpp \
    qcoreapplication_mock.cpp \
    mfeedbackpalette_mock.cpp \
    mreactionmapconnection_mock.cpp \
    ../../src/common/mfsettings.cpp

HEADERS += ut_mreactionmap.h \
    mfeedbackpalette.h \
    mfeedbackpalette_mock.h \
    mreactionmapconnection.h \
    mreactionmapconnection_mock.h \
    ../../src/common/mfsettings.h

include(../common_bot.pri)

target.path = $$[QT_INSTALL_LIBS]/libmeegoreactionmap-tests
INSTALLS += target
