set(QT_USE_QTNETWORK TRUE)
set(QT_USE_QTXML TRUE)
set(QT_USE_QTDBUS TRUE)

include(${QT_USE_FILE})

add_definitions(-O2)
add_definitions(-Wall)
add_definitions(-DQT_NO_DEBUG)
add_definitions(${QT_DEFINITIONS})
include_directories( ./ ../libxinx ../components ${CMAKE_CURRENT_BINARY_DIR} ${QT_INCLUDE_DIR})

set(xinx_SRCS
	aboutdialogimpl.cpp
	commitmessagedialogimpl.cpp
	configurationfile.cpp
	customdialogimpl.cpp
	dirrcsmodel.cpp
	filecontentdockwidget.cpp
	flattreeview.cpp
	iconprojectprovider.cpp
	main.cpp
	mainformimpl.cpp
	newprojectwizard.cpp
	projectdirectorydockwidget.cpp
	projectpropertyimpl.cpp
	rcslogdialogimpl.cpp
	replacedialogimpl.cpp
	runsnipetdialogimpl.cpp
	scriptmanager.cpp
	snipet.cpp
	snipetdialog.cpp
	snipetdockwidget.cpp
	snipetlist.cpp
	specifiquedlgimpl.cpp
	studioadaptor.cpp
	studiointerface.cpp
	tabeditor.cpp
	threadedconfigurationfile.cpp
	uniqueapplication.cpp
	xmlpresentationdockwidget.cpp
	xmlpresentationitem.cpp
)

SET(xinx_MOC_HDRS
	aboutdialogimpl.h
	commitmessagedialogimpl.h
	configurationfile.h
	customdialogimpl.h
	dirrcsmodel.h
	filecontentdockwidget.h
	flattreeview.h
	iconprojectprovider.h
	mainformimpl.h
	newprojectwizard.h
	projectdirectorydockwidget.h
	projectpropertyimpl.h
	rcslogdialogimpl.h
	replacedialogimpl.h
	runsnipetdialogimpl.h
	scriptmanager.h
	snipetdialog.h
	snipetdockwidget.h
	snipet.h
	snipetlist.h
	specifiquedlgimpl.h
	studioadaptor.h
	studiointerface.h
	tabeditor.h
	threadedconfigurationfile.h
	uniqueapplication.h
	xmlpresentationdockwidget.h
	xmlpresentationitem.h
	private/p_configurationfile.h
	private/p_customdialogimpl.h
	private/p_filecontentdockwidget.h
	private/p_flattreeview.h
	private/p_mainformimpl.h
	private/p_projectdirectorydockwidget.h
	private/p_threadedconfigurationfile.h
	private/p_uniqueapplication.h
	private/p_xmlpresentationdockwidget.h
)

SET(xinx_UIS
	ui/about.ui
	ui/commitmessages.ui
	ui/custom.ui
	ui/mainform.ui
	ui/newprojectwizard_project.ui
	ui/newprojectwizard_specifique.ui
	ui/projectdirectorywidget.ui
	ui/projectproperty.ui
	ui/rcslogform.ui
	ui/replace.ui
	ui/runsnipet.ui
	ui/snipetproperty.ui
	ui/specifiquedlg.ui
	ui/xmlpresentationwidget.ui
)


QT4_WRAP_CPP(xinx_MOC_SRCS ${xinx_MOC_HDRS})
QT4_WRAP_UI(xinx_UIS_H ${xinx_UIS})
QT4_ADD_RESOURCES(xinx_QRC_H application.qrc)

if(WIN32)
	SET(xinx_RCS
		rc/xinx.rc
	)
	add_custom_command( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/xinx_ico.obj
		COMMAND windres.exe -I${CMAKE_CURRENT_SOURCE_DIR} -o ${CMAKE_CURRENT_BINARY_DIR}/xinx_ico.obj -i${CMAKE_CURRENT_SOURCE_DIR}/${xinx_RCS}
	)
	set(xinx_RESS ${CMAKE_CURRENT_BINARY_DIR}/xinx_ico.obj)
endif(WIN32)

add_executable(xinx WIN32 ${xinx_SRCS} ${xinx_MOC_SRCS} ${xinx_UIS_H} ${xinx_QRC_H} ${xinx_RESS})
link_directories(../plugins ../libxinx ../components)
target_link_libraries(xinx ${QT_LIBRARIES} webplugin sharedxinx xinxcmp)

install(TARGETS xinx DESTINATION bin)
install(FILES xml/template.xnx DESTINATION xml)
