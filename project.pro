TEMPLATE = subdirs
SUBDIRS += libxinx \
    components \
    plugins \
    xinx \
    xinxprojectwizard

dox.target = doc
dox.commands = doxygen Doxyfile
dox.depends =

QMAKE_EXTRA_UNIX_TARGETS += dox