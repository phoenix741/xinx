TEMPLATE = subdirs

CONFIG += release

SUBDIRS += lib example

example.depends = lib

CONFIG(debug, debug|release) {
	# placeholder
	QCODE_EDIT_EXTRA_DEFINES += _DEBUG_BUILD_
} else {
	# placeholder
	QCODE_EDIT_EXTRA_DEFINES += _RELEASE_BUILD_
}

include(install.pri)
