TEMPLATE = subdirs
SUBDIRS += external_qcodeedit \
	xinx_components \
	xinx_library \
	xinx_test1 \
	xinx_test2 \
	xinx_test4 \
	xinx_test5 \
	xinx_test6 \
	plugins_core \
	plugins_cvsplugin \
	plugins_svnplugin \
	plugins_services \
	plugins_generix \
	xinx_application
	
external_qcodeedit.subdir = ext/qcodeedit

xinx_components.subdir = components
xinx_components.depends = external_qcodeedit

xinx_library.subdir = framework
xinx_library.depends = external_qcodeedit xinx_components

xinx_test1.subdir = unittest/guixinxcodeedit
xinx_test1.depends = external_qcodeedit xinx_components xinx_library
xinx_test2.subdir = unittest/testxinxcodeedit
xinx_test2.depends = external_qcodeedit xinx_components xinx_library
xinx_test4.subdir = unittest/testsnipets
xinx_test4.depends = external_qcodeedit xinx_components xinx_library
xinx_test5.subdir = unittest/testutils
xinx_test5.depends = external_qcodeedit xinx_components xinx_library
xinx_test6.subdir = unittest/testcontentview2
xinx_test6.depends = external_qcodeedit xinx_components xinx_library plugins_core

plugins_core.subdir = plugins/core

plugins_cvsplugin.depends = xinx_components
plugins_cvsplugin.subdir = plugins/cvsplugin
plugins_cvsplugin.depends = xinx_components xinx_library
plugins_svnplugin.subdir = plugins/svnplugin
plugins_svnplugin.depends = xinx_components xinx_library
plugins_services.subdir = plugins/services
plugins_services.depends = external_qcodeedit xinx_components xinx_library plugins_core
plugins_generix.subdir = plugins/generix
plugins_generix.depends = xinx_components xinx_library

xinx_application.subdir = xinx
xinx_application.depends = external_qcodeedit xinx_components xinx_library plugins_core

dox.target = doc
dox.commands = doxygen Doxyfile
dox.depends =

QMAKE_EXTRA_UNIX_TARGETS += dox
