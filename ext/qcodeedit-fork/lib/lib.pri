DEFINES += _QCODE_EDIT_BUILD_

CONFIG *= qnfa

QT *= xml

# Input

HEADERS += qce-config.h \
	qeditor.h \
	qeditorfactory.h \
	qcodeedit.h \
	qpanellayout.h \
	qformatfactory.h \
	qlinemarksinfocenter.h \
	qreliablefilewatch.h \
	document/qdocument.h \
	document/qdocument_p.h \
	document/qdocumentcommand.h \
	document/qdocumentcursor.h \
	document/qdocumentline.h \
	document/qdocumentsearch.h \
	language/qcodecompletionengine.h \
	language/qhighlighter.h \
	language/qhighlighterinterface.h \
	language/qindenter.h \
	language/qindenterinterface.h \
	language/qlanguagedefinition.h \
	language/qlanguagefactory.h \
	language/qmarker.h \
	language/qmarkerinterface.h \
	language/qmatcher.h \
	language/qmatcherinterface.h \
	widgets/qpanel.h \
	widgets/qlinenumberpanel.h \
	widgets/qlinemarkpanel.h \
	widgets/qlinechangepanel.h \
	widgets/qfoldpanel.h \
	widgets/qstatuspanel.h \
	widgets/qsearchreplacepanel.h \
	widgets/qgotolinedialog.h \
	widgets/qeditconfig.h \
	widgets/qcalltip.h

SOURCES += qeditor.cpp \
	qeditorfactory.cpp \
	qcodeedit.cpp \
	qpanellayout.cpp \
	qformatfactory.cpp \
	qlinemarksinfocenter.cpp \
	qreliablefilewatch.cpp \
	document/qdocument.cpp \
	document/qdocumentcommand.cpp \
	document/qdocumentcursor.cpp \
	document/qdocumentline.cpp \
	document/qdocumentsearch.cpp \
	language/qcodecompletionengine.cpp \
	language/qhighlighter.cpp \
	language/qindenter.cpp \
	language/qlanguagedefinition.cpp \
	language/qlanguagefactory.cpp \
	language/qmarker.cpp \
	language/qmatcher.cpp \
	widgets/qpanel.cpp \
	widgets/qlinenumberpanel.cpp \
	widgets/qlinemarkpanel.cpp \
	widgets/qlinechangepanel.cpp \
	widgets/qfoldpanel.cpp \
	widgets/qstatuspanel.cpp \
	widgets/qsearchreplacepanel.cpp \
	widgets/qgotolinedialog.cpp \
	widgets/qeditconfig.cpp \
	widgets/qcalltip.cpp

FORMS += widgets/searchreplace.ui \
	widgets/gotoline.ui \
	widgets/editconfig.ui

qnfa {
	DEFINES += QNFA_BUILD

	HEADERS += qnfa/qnfa.h \
		qnfa/qnfadefinition.h \
		qnfa/light_vector.h

	SOURCES += qnfa/qnfa.cpp \
		qnfa/qnfadefinition.cpp \
		qnfa/xml2qnfa.cpp
}

