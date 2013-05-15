CONFIG += ordered
TEMPLATE    = subdirs
SUBDIRS     = src \
              tests \
              tools

include (conf/conf.pri)
include (shared.pri)

isEqual( IN_PWD, $${OUT_PWD} ) {
    IS_OUT_OF_SOURCE = 0
} else {
    IS_OUT_OF_SOURCE = 1
}

include (doc/doc.pri)

QMAKE_EXTRA_TARGETS += setup

QMAKE_CLEAN += build-stamp configure-stamp artifacts/*.deb tests/*/*.log.xml tests/*/*.log *.log.xml *.log **/*.gcda
QMAKE_DISTCLEAN += build-stamp configure-stamp artifacts/*.deb tests/*/*.log.xml tests/*/*.log *.log.xml *.log **/*.gcda

check.target = check
check.CONFIG = recursive
QMAKE_EXTRA_TARGETS += check

check-xml.target = check-xml
check-xml.CONFIG = recursive
QMAKE_EXTRA_TARGETS += check-xml
