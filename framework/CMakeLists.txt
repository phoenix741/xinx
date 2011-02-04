find_package(Qt4 REQUIRED)
find_package(QCodeEdit REQUIRED)
find_package(Ideality REQUIRED)
find_package(LibXml2 REQUIRED)
find_package(LibXslt REQUIRED)
find_package(Subversion)

if(Subversion_FOUND)
	Subversion_WC_INFO(${PROJECT_SOURCE_DIR} XINX_FRAMEWORK)
else(Subversion_FOUND)
	set(XINX_FRAMEWORK_WC_REVISION 0)
endif(Subversion_FOUND)
configure_file(core/version.h.in core/version.h)

set(USE_COMPONENTS True)
set(QT_USE_QTXML TRUE)
set(QT_USE_QTXMLPATTERNS TRUE)
set(QT_USE_QTSQL TRUE)
set(QT_USE_QTSCRIPT TRUE)

include(UseUniqueApplication)
include(UseXinx)

add_definitions(-D_LIB_BUILD_)

add_xinx_librairie(framework "" 0.9.1.0)
