include(${QT_USE_FILE})

set(services_SRCS
	connectionwebservicesdialogimpl.cpp
	serviceresultdialogimpl.cpp
	servicesplugin.cpp
	servicesprojectpropertyimpl.cpp
	servicesprojectwizard.cpp
	soap.cpp
	webservices.cpp
	webserviceseditor.cpp
	wsdl.cpp
)

SET(services_MOC_HDRS
	connectionwebservicesdialogimpl.h
	serviceresultdialogimpl.h
	servicesplugin.h
	servicesprojectpropertyimpl.h
	servicesprojectwizard.h
	soap.h
	webserviceseditor.h
	webservices.h
	wsdl.h
)

SET(services_MOC_QRCS
	servicesplugin.qrc
)

SET(services_UIS
	ui/newprojectwizard_serviceslist.ui
	ui/newprojectwizard_services.ui
	ui/resultatServices.ui
	ui/servicesconnection.ui
	ui/servicesprojectproperty.ui
)

include_directories(../../components ../webplugin/xsl ../../libxinx ${CMAKE_CURRENT_BINARY_DIR} ${QT_INCLUDE_DIR})

QT4_WRAP_CPP(services_MOC_SRCS ${services_MOC_HDRS})
QT4_WRAP_UI(services_UIS_H ${services_UIS})
QT4_ADD_RESOURCES(services_QRC_H ${services_MOC_QRCS})

add_definitions(-O2)
add_definitions(-Wall)
add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_PLUGIN)
add_definitions(-DQT_NO_DEBUG)
add_definitions(-DQT_SHARED)

link_directories(../../components ../../libxinx ../)
add_library(services SHARED ${services_SRCS} ${services_MOC_SRCS} ${services_UIS_H} ${services_QRC_H})
target_link_libraries(services ${QT_LIBRARIES} xinxcmp webplugin sharedxinx)

