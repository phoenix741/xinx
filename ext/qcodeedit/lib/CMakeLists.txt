find_package(Qt4 REQUIRED)

set(QT_USE_QTXML TRUE)

include(UseXinx)

add_definitions(-D_QCODE_EDIT_BUILD_)
add_definitions(-DQNFA_BUILD)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/document ${CMAKE_CURRENT_SOURCE_DIR}/language ${CMAKE_CURRENT_SOURCE_DIR}/qnfa ${CMAKE_CURRENT_SOURCE_DIR}/widgets)

include(${QT_USE_FILE})

add_definitions(${QT_DEFINITIONS})
set(headers 
	qce-config.h 
	qeditor.h
	qeditorfactory.h 
	qcodeedit.h 
	qpanellayout.h 
	qformatfactory.h 
	qformatscheme.h 
	qlinemarksinfocenter.h 
	qreliablefilewatch.h 
	document/qdocument.h 
	document/qdocument_p.h 
	document/qdocumentcommand.h 
	document/qdocumentcursor.h 
	document/qdocumentline.h 
	document/qdocumentsearch.h 
	qcodecompletionengine.h 
	qlanguagedefinition.h 
	qlanguagefactory.h 
	widgets/qpanel.h 
	widgets/qlinenumberpanel.h 
	widgets/qlinemarkpanel.h
	widgets/qlinechangepanel.h 
	widgets/qfoldpanel.h 
	widgets/qstatuspanel.h 
	widgets/qsearchreplacepanel.h 
	widgets/qgotolinedialog.h 
	widgets/qeditconfig.h 
	widgets/qformatconfig.h 
	widgets/qsimplecolorpicker.h 
	widgets/qcalltip.h
	qnfa/qnfa.h 
	qnfa/qnfadefinition.h 
	qnfa/light_vector.h)
set(sources 
	qeditor.cpp 
	qeditorfactory.cpp 
	qcodeedit.cpp 
	qpanellayout.cpp 
	qformatscheme.cpp 
	qlinemarksinfocenter.cpp 
	qreliablefilewatch.cpp 
	document/qdocument.cpp 
	document/qdocumentcommand.cpp 
	document/qdocumentcursor.cpp 
	document/qdocumentline.cpp 
	document/qdocumentsearch.cpp 
	qcodecompletionengine.cpp 
	qlanguagedefinition.cpp 
	qlanguagefactory.cpp 
	widgets/qpanel.cpp 
	widgets/qlinenumberpanel.cpp 
	widgets/qlinemarkpanel.cpp
	widgets/qlinechangepanel.cpp 
	widgets/qfoldpanel.cpp 
	widgets/qstatuspanel.cpp 
	widgets/qsearchreplacepanel.cpp 
	widgets/qgotolinedialog.cpp 
	widgets/qeditconfig.cpp 
	widgets/qformatconfig.cpp 
	widgets/qsimplecolorpicker.cpp 
	widgets/qcalltip.cpp
	qnfa/qnfa.cpp 
	qnfa/qnfadefinition.cpp 
	qnfa/xml2qnfa.cpp)
set(forms
	widgets/searchreplace.ui 
	widgets/gotoline.ui 
	widgets/editconfig.ui 
	widgets/formatconfig.ui)

qt4_wrap_ui(generated_forms ${forms})
xinx_automoc(moc_headers ${headers})

add_library(qcodeedit SHARED ${moc_headers} ${sources} ${generated_forms})
target_link_libraries(qcodeedit ${QT_LIBRARIES})
set_target_properties(qcodeedit PROPERTIES VERSION 2.2.0.0)
if(UNIX)
	set_target_properties(qcodeedit PROPERTIES LINK_FLAGS "-rdynamic")
endif(UNIX)

install(TARGETS qcodeedit DESTINATION bin)

