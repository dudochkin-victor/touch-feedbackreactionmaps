TEMPLATE = subdirs

ARCH = $$system(uname -m)

SUBDIRS += \
           ut_mfreactionmapstack \
           ut_colormap \
           ut_mfeedbackpalette \
           ut_mreactionmapconnection \
           ut_mreactionmap \
           ft_clientqtapp \
           ft_mfxlistener \
           ft_multiplemainwindowqtapp \
           ft_touchscreenmanaging \
           ft_multipleviewqtapp

QMAKE_STRIP = echo
include(shell.pri)
include(runtests.pri)

check.target = check
check.CONFIG = recursive
QMAKE_EXTRA_TARGETS += check


check-xml.target = check-xml
check-xml.CONFIG = recursive
QMAKE_EXTRA_TARGETS += check-xml

QMAKE_CLEAN += **/*.log.xml ./coverage.log.xml **/*.log
