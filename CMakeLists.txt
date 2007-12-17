project(xinx)
cmake_minimum_required(VERSION 2.4.0)
find_package(Qt4 REQUIRED) # find and setup Qt4 for this project
find_package(Doxygen COMPONENTS) # find and setup Qt4 for this project

set(QT_USE_QTNETWORK TRUE)
set(QT_USE_QTXML TRUE)
set(QT_USE_QTDBUS TRUE)
include(${QT_USE_FILE})

set(xinx_SRCS
	src/specifiquedlgimpl.cpp
	src/filecontentstructure.cpp
	src/aboutdialogimpl.cpp
	src/appsettings.cpp
	src/commitmessagedialogimpl.cpp
	src/configurationfile.cpp
	src/connectionwebservicesdialogimpl.cpp
	src/customdialogimpl.cpp
	src/cvsfiles.cpp
	src/cvsthread.cpp
	src/directoryedit.cpp
	src/dirrcsmodel.cpp
	src/editor.cpp
	src/editorcompletion.cpp
	src/exceptions.cpp
	src/filecontentdockwidget.cpp
	src/filecontentitemmodel.cpp
	src/fileeditor.cpp
	src/filetypeinterface.cpp
	src/filetypejs.cpp
	src/filetypexml.cpp
	src/filetypexsl.cpp
	src/filewatcher.cpp
	src/flattreeview.cpp
	src/globals.cpp
	src/iconprojectprovider.cpp
	src/javascriptmodelcompleter.cpp
	src/javascriptparser.cpp
	src/jsfileeditor.cpp
	src/jshighlighter.cpp
	src/kcolorcombo.cpp
	src/main.cpp
	src/mainformimpl.cpp
	src/numberbar.cpp
	src/objectview.cpp
	src/projectdirectorydockwidget.cpp
	src/projectpropertyimpl.cpp
	src/rcs.cpp
	src/rcs_cvs.cpp
	src/rcslogdialogimpl.cpp
	src/replacedialogimpl.cpp
	src/runsnipetdialogimpl.cpp
	src/serviceresultdialogimpl.cpp
	src/snipet.cpp
	src/snipetdialog.cpp
	src/snipetlist.cpp
	src/snipetmodel.cpp
	src/soap.cpp
	src/syntaxhighlighter.cpp
	src/tabeditor.cpp
	src/texteditor.cpp
	src/threadedconfigurationfile.cpp
	src/uniqueapplication.cpp
	src/webservices.cpp
	src/webserviceseditor.cpp
	src/wsdl.cpp
	src/xinxconfig.cpp
	src/xmlfileeditor.cpp
	src/xmlhighlighter.cpp
	src/xmlpresentationdockwidget.cpp
	src/xmlpresentationitem.cpp
	src/xsllistview.cpp
	src/xslmodelcompleter.cpp
	src/xslproject.cpp
	src/xinxthread.cpp
	src/studioadaptor.cpp
	src/studiointerface.cpp
)

SET(xinx_MOC_HDRS
	src/specifiquedlgimpl.h
	src/filecontentstructure.h
	src/xinxthread.h
	src/aboutdialogimpl.h
	src/appsettings.h
	src/commitmessagedialogimpl.h
	src/configurationfile.h
	src/connectionwebservicesdialogimpl.h
	src/customdialogimpl.h
	src/cvsfiles.h
	src/cvsthread.h
	src/directoryedit.h
	src/dirrcsmodel.h
	src/editor.h
	src/editorcompletion.h
	src/exceptions.h
	src/filecontentdockwidget.h
	src/filecontentitemmodel.h
	src/fileeditor.h
	src/filetypeinterface.h
	src/filetypejs.h
	src/filetypexml.h
	src/filetypexsl.h
	src/filewatcher.h
	src/flattreeview.h
	src/globals.h
	src/iconprojectprovider.h
	src/javascriptmodelcompleter.h
	src/javascriptparser.h
	src/jsfileeditor.h
	src/jshighlighter.h
	src/kcolorcombo.h
	src/mainformimpl.h
	src/numberbar.h
	src/objectview.h
	src/private/p_configurationfile.h
	src/private/p_connectionwebservicesdialogimpl.h
	src/private/p_filecontentdockwidget.h
	src/private/p_fileeditor.h
	src/private/p_filetypeinterface.h
	src/private/p_filetypejs.h
	src/private/p_filetypexsl.h
	src/private/p_filewatcher.h
	src/private/p_mainformimpl.h
	src/private/p_projectdirectorydockwidget.h
	src/private/p_rcs_cvs.h
	src/private/p_threadedconfigurationfile.h
	src/private/p_uniqueapplication.h
	src/private/p_xmlpresentationdockwidget.h
	src/private/p_flattreeview.h
	src/private/p_runsnipetdialogimpl.h
	src/private/p_serviceresultdialogimpl.h
	src/private/p_snipetdialog.h
	src/private/p_snipetlist.h
	src/private/p_webserviceseditor.h
	src/projectdirectorydockwidget.h
	src/projectpropertyimpl.h
	src/rcs.h
	src/rcs_cvs.h
	src/rcslogdialogimpl.h
	src/replacedialogimpl.h
	src/runsnipetdialogimpl.h
	src/serviceresultdialogimpl.h
	src/snipet.h
	src/snipetdialog.h
	src/snipetlist.h
	src/snipetmodel.h
	src/soap.h
	src/syntaxhighlighter.h
	src/tabeditor.h
	src/texteditor.h
	src/threadedconfigurationfile.h
	src/uniqueapplication.h
	src/webservices.h
	src/webserviceseditor.h
	src/wsdl.h
	src/xinxconfig.h
	src/xmlfileeditor.h
	src/xmlhighlighter.h
	src/xmlpresentationdockwidget.h
	src/xmlpresentationitem.h
	src/xsllistview.h
	src/xslmodelcompleter.h
	src/xslproject.h
	src/studioadaptor.h
	src/studiointerface.h
)

SET(xinx_UIS
	ui/specifiquedlg.ui
	ui/about.ui
	ui/commitmessages.ui
	ui/custom.ui
	ui/mainform.ui
	ui/projectdirectorywidget.ui
	ui/projectproperty.ui
	ui/rcslogform.ui
	ui/replace.ui
	ui/resultatServices.ui
	ui/runsnipet.ui
	ui/servicesconnection.ui
	ui/snipetproperty.ui
	ui/xmlpresentationwidget.ui
)

SET(xinx_RCS
	rc/xinx.rc
)

QT4_WRAP_CPP(xinx_MOC_SRCS ${xinx_MOC_HDRS})
QT4_WRAP_UI(xinx_UIS_H ${xinx_UIS})
QT4_ADD_RESOURCES(xinx_QRC_H application.qrc)

if(WIN32)
add_custom_command( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/xinx_ico.obj
	COMMAND windres.exe -I${CMAKE_CURRENT_SOURCE_DIR} -o ${CMAKE_CURRENT_BINARY_DIR}/xinx_ico.obj -i${CMAKE_CURRENT_SOURCE_DIR}/${xinx_RCS}
)
set(xinx_RESS
	${CMAKE_CURRENT_BINARY_DIR}/xinx_ico.obj
)
endif(WIN32)

add_definitions(-Wall ${QT_DEFINITIONS})

include_directories(
	src/
	${CMAKE_CURRENT_BINARY_DIR}
	${QT_INCLUDE_DIR}
)
add_executable(xinx WIN32 ${xinx_SRCS} ${xinx_MOC_SRCS} ${xinx_UIS_H} ${xinx_QRC_H} ${xinx_RESS})
target_link_libraries(xinx ${QT_LIBRARIES})

install(TARGETS xinx DESTINATION bin)
install(FILES xml/completion.xnx DESTINATION xml)
install(FILES xml/template.xnx DESTINATION xml)
