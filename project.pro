TEMPLATE = subdirs
SUBDIRS += external_qcodeedit \
	xinx_components \
	xinx_library \
	plugins_xmlpres \
	plugins_webplugin \
	plugins_services \
	xinx_application
	
external_qcodeedit.subdir = ext/qcodeedit

xinx_components.subdir = components
xinx_components.depends = external_qcodeedit

xinx_library.subdir = libxinx
xinx_library.depends = external_qcodeedit xinx_components

xinx_test1.subdir = unittest/guixinxcodeedit
xinx_test1.depends = external_qcodeedit xinx_components xinx_library
xinx_test2.subdir = unittest/testxinxcodeedit
xinx_test2.depends = external_qcodeedit xinx_components xinx_library
xinx_test3.subdir = unittest/testcontentview
xinx_test3.depends = external_qcodeedit xinx_components xinx_library

plugins_xmlpres.subdir = plugins/xmlpres
plugins_webplugin.subdir = plugins/webplugin
plugins_cvsplugin.depends = xinx_components
plugins_cvsplugin.subdir = plugins/cvsplugin
plugins_cvsplugin.depends = xinx_components xinx_library
plugins_svnplugin.subdir = plugins/svnplugin
plugins_svnplugin.depends = xinx_components xinx_library
plugins_services.subdir = plugins/services
plugins_services.depends = external_qcodeedit xinx_components xinx_library plugins_webplugin
plugins_dictionary.subdir = plugins/dictionary
plugins_dictionary.depends = xinx_components xinx_library

xinx_application.subdir = xinx
xinx_application.depends = external_qcodeedit xinx_components xinx_library plugins_xmlpres plugins_webplugin

xinx_wizard.subdir = xinxprojectwizard
xinx_wizard.depends = external_qcodeedit xinx_components xinx_library

dox.target = doc
dox.commands = doxygen Doxyfile
dox.depends =

QMAKE_EXTRA_UNIX_TARGETS += dox
