include(../common_top.pri)
TARGET = ut_mreactionmapconnection

COPY_TESTED_SOURCES = ../../src/client/mreactionmapconnection.cpp
copysourcefiles.input = COPY_TESTED_SOURCES
copysourcefiles.output = ${QMAKE_FILE_BASE}.cpp
copysourcefiles.commands = $$QMAKE_COPY ${QMAKE_FILE_IN} ${QMAKE_FILE_BASE}.cpp
copysourcefiles.variable_out = SOURCES
copysourcefiles.CONFIG = target_predeps no_link
QMAKE_EXTRA_COMPILERS += copysourcefiles

TEST_SOURCES = $$COPY_TESTED_SOURCES

COPY_TESTED_HEADERS = ../../src/client/mreactionmapconnection.h
copyheaderfiles.input = COPY_TESTED_HEADERS
copyheaderfiles.output = ${QMAKE_FILE_BASE}.h
copyheaderfiles.commands = $$QMAKE_COPY ${QMAKE_FILE_IN} ${QMAKE_FILE_BASE}.h
copyheaderfiles.variable_out = HEADERS
copyheaderfiles.CONFIG = target_predeps no_link
QMAKE_EXTRA_COMPILERS += copyheaderfiles

MOC_TESTED_HEADERS = ../../src/client/mreactionmapconnection.h
mocheaderfiles.input = MOC_TESTED_HEADERS
mocheaderfiles.output = moc_${QMAKE_FILE_BASE}.cpp
mocheaderfiles.commands = $$QMAKE_MOC $(DEFINES) $(INCPATH) ${QMAKE_FILE_BASE}.h -o moc_${QMAKE_FILE_BASE}.cpp
mocheaderfiles.variable_out = SOURCES
mocheaderfiles.CONFIG = target_predeps no_link
QMAKE_EXTRA_COMPILERS += mocheaderfiles

SOURCES += ut_mreactionmapconnection.cpp \
    mreactionmap_p_mock.cpp \
    qlocalsocket_mock.cpp

HEADERS += ut_mreactionmapconnection.h \
    mreactionmap_p.h \
    mreactionmap_p_mock.h \
    QLocalSocket \
    qlocalsocket.h \
    qlocalsocket_mock.h \
    ../../src/common/mfcommondata.h

include(../common_bot.pri)

target.path = $$[QT_INSTALL_LIBS]/libmeegoreactionmap-tests
INSTALLS += target
