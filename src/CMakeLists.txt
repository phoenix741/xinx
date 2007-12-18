set(QT_USE_QTNETWORK TRUE)
set(QT_USE_QTXML TRUE)
set(QT_USE_QTDBUS TRUE)

include(${QT_USE_FILE})

add_definitions(-O2)
add_definitions(-Wall)
add_definitions(${QT_DEFINITIONS})
include_directories(./ ${CMAKE_CURRENT_BINARY_DIR} ${QT_INCLUDE_DIR})

set(xinx_SRCS
	specifiquedlgimpl.cpp
	filecontentstructure.cpp
	aboutdialogimpl.cpp
	appsettings.cpp
	commitmessagedialogimpl.cpp
	configurationfile.cpp
	connectionwebservicesdialogimpl.cpp
	customdialogimpl.cpp
	cvsfiles.cpp
	cvsthread.cpp
	directoryedit.cpp
	dirrcsmodel.cpp
	editor.cpp
	editorcompletion.cpp
	exceptions.cpp
	filecontentdockwidget.cpp
	filecontentitemmodel.cpp
	fileeditor.cpp
	filetypeinterface.cpp
	filetypejs.cpp
	filetypexml.cpp
	filetypexsl.cpp
	filewatcher.cpp
	flattreeview.cpp
	globals.cpp
	iconprojectprovider.cpp
	javascriptmodelcompleter.cpp
	javascriptparser.cpp
	jsfileeditor.cpp
	jshighlighter.cpp
	kcolorcombo.cpp
	main.cpp
	mainformimpl.cpp
	numberbar.cpp
	objectview.cpp
	projectdirectorydockwidget.cpp
	projectpropertyimpl.cpp
	rcs.cpp
	rcs_cvs.cpp
	rcslogdialogimpl.cpp
	replacedialogimpl.cpp
	runsnipetdialogimpl.cpp
	serviceresultdialogimpl.cpp
	snipet.cpp
	snipetdialog.cpp
	snipetlist.cpp
	snipetmodel.cpp
	soap.cpp
	syntaxhighlighter.cpp
	tabeditor.cpp
	texteditor.cpp
	threadedconfigurationfile.cpp
	uniqueapplication.cpp
	webservices.cpp
	webserviceseditor.cpp
	wsdl.cpp
	xinxconfig.cpp
	xmlfileeditor.cpp
	xmlhighlighter.cpp
	xmlpresentationdockwidget.cpp
	xmlpresentationitem.cpp
	xsllistview.cpp
	xslmodelcompleter.cpp
	xslproject.cpp
	xinxthread.cpp
	studioadaptor.cpp
	studiointerface.cpp
	xinxpluginsloader.cpp
	plugindialog.cpp
)

SET(xinx_MOC_HDRS
	plugindialog.h
	xinxpluginsloader.h
	specifiquedlgimpl.h
	filecontentstructure.h
	xinxthread.h
	aboutdialogimpl.h
	appsettings.h
	commitmessagedialogimpl.h
	configurationfile.h
	connectionwebservicesdialogimpl.h
	customdialogimpl.h
	cvsfiles.h
	cvsthread.h
	directoryedit.h
	dirrcsmodel.h
	editor.h
	editorcompletion.h
	exceptions.h
	filecontentdockwidget.h
	filecontentitemmodel.h
	fileeditor.h
	filetypeinterface.h
	filetypejs.h
	filetypexml.h
	filetypexsl.h
	filewatcher.h
	flattreeview.h
	globals.h
	iconprojectprovider.h
	javascriptmodelcompleter.h
	javascriptparser.h
	jsfileeditor.h
	jshighlighter.h
	kcolorcombo.h
	mainformimpl.h
	numberbar.h
	objectview.h
	private/p_configurationfile.h
	private/p_connectionwebservicesdialogimpl.h
	private/p_filecontentdockwidget.h
	private/p_fileeditor.h
	private/p_filetypeinterface.h
	private/p_filetypejs.h
	private/p_filetypexsl.h
	private/p_filewatcher.h
	private/p_mainformimpl.h
	private/p_projectdirectorydockwidget.h
	private/p_rcs_cvs.h
	private/p_threadedconfigurationfile.h
	private/p_uniqueapplication.h
	private/p_xmlpresentationdockwidget.h
	private/p_flattreeview.h
	private/p_runsnipetdialogimpl.h
	private/p_serviceresultdialogimpl.h
	private/p_snipetdialog.h
	private/p_snipetlist.h
	private/p_webserviceseditor.h
	projectdirectorydockwidget.h
	projectpropertyimpl.h
	rcs.h
	rcs_cvs.h
	rcslogdialogimpl.h
	replacedialogimpl.h
	runsnipetdialogimpl.h
	serviceresultdialogimpl.h
	snipet.h
	snipetdialog.h
	snipetlist.h
	snipetmodel.h
	soap.h
	syntaxhighlighter.h
	tabeditor.h
	texteditor.h
	threadedconfigurationfile.h
	uniqueapplication.h
	webservices.h
	webserviceseditor.h
	wsdl.h
	xinxconfig.h
	xmlfileeditor.h
	xmlhighlighter.h
	xmlpresentationdockwidget.h
	xmlpresentationitem.h
	xsllistview.h
	xslmodelcompleter.h
	xslproject.h
	studioadaptor.h
	studiointerface.h
)

SET(xinx_UIS
	../ui/specifiquedlg.ui
	../ui/about.ui
	../ui/commitmessages.ui
	../ui/custom.ui
	../ui/mainform.ui
	../ui/projectdirectorywidget.ui
	../ui/projectproperty.ui
	../ui/rcslogform.ui
	../ui/replace.ui
	../ui/resultatServices.ui
	../ui/runsnipet.ui
	../ui/servicesconnection.ui
	../ui/snipetproperty.ui
	../ui/xmlpresentationwidget.ui
	../ui/plugindialog.ui
)

SET(xinx_RCS
	../rc/xinx.rc
)

QT4_WRAP_CPP(xinx_MOC_SRCS ${xinx_MOC_HDRS})
QT4_WRAP_UI(xinx_UIS_H ${xinx_UIS})
QT4_ADD_RESOURCES(xinx_QRC_H ../application.qrc)

if(WIN32)
	add_custom_command( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/xinx_ico.obj
		COMMAND windres.exe -I${CMAKE_CURRENT_SOURCE_DIR} -o ${CMAKE_CURRENT_BINARY_DIR}/xinx_ico.obj -i${CMAKE_CURRENT_SOURCE_DIR}/${xinx_RCS}
	)
	set(xinx_RESS ${CMAKE_CURRENT_BINARY_DIR}/xinx_ico.obj)
endif(WIN32)

add_executable(xinx WIN32 ${xinx_SRCS} ${xinx_MOC_SRCS} ${xinx_UIS_H} ${xinx_QRC_H} ${xinx_RESS})
target_link_libraries(xinx ${QT_LIBRARIES})

install(TARGETS xinx DESTINATION bin)
install(FILES ../xml/completion.xnx DESTINATION xml)
install(FILES ../xml/template.xnx DESTINATION xml)
