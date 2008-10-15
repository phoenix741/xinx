
#include "window.h"

#include "qeditor.h"
#include "qcodeedit.h"

#include "qfoldpanel.h"
#include "qlinemarkpanel.h"
#include "qlinechangepanel.h"
#include "qlinenumberpanel.h"

#include "qdocument.h"
#include "qdocumentline.h"
#include "qdocumentcursor.h"

#include "qformatfactory.h"
#include "qlanguagefactory.h"

#include "qlinemarksinfocenter.h"

#include <QDir>
#include <QTime>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QFileDialog>

Window::Window(QWidget *p)
{
	setupUi(this);

	m_formats = new QFormatFactory("qxs/formats.qxf", this);
	QDocument::setFormatFactory(m_formats);

	QDocument::setShowSpaces(QDocument::ShowTrailing | QDocument::ShowLeading | QDocument::ShowTabs);

	QLineMarksInfoCenter::instance()->loadMarkTypes("qxs/marks.qxm");

	m_languages = new QLanguageFactory(m_formats, this);
	m_languages->addDefinitionPath("qxs");

	m_editControl = new QCodeEdit(this);

	m_editControl
		->addPanel(new QLineMarkPanel, QCodeEdit::West, true)
		->setShortcut(QKeySequence("F6"));

	m_editControl
		->addPanel(new QLineNumberPanel, QCodeEdit::West, true)
		->setShortcut(QKeySequence("F11"));

	m_editControl
		->addPanel(new QFoldPanel, QCodeEdit::West, true)
		->setShortcut(QKeySequence("F9"));

	m_editControl
		->addPanel(new QLineChangePanel, QCodeEdit::West, true)
		; //->setShortcut(QKeySequence("F11"));

	setCentralWidget(m_editControl->editor());

	setWindowTitle("QCodeEdit example");
}

void Window::load(const QString& file)
{/*
	QTime t;
	t.start();
	if ( QFile::exists(file) )
	{
		m_languages->setLanguage(m_editControl->editor(), file);
		m_editControl->editor()->load(file);

		setWindowTitle(QString("QCodeEdit example [%1[*]]").arg(file));
	} else {
		m_languages->setLanguage(m_editControl->editor(), "loading_failed.cpp");
		m_editControl->editor()->setText("// Loading failed\n// fallback to C++\n");

		setWindowTitle("QCodeEdit example");
	}
	qDebug("loading took %i ms", t.elapsed());*/
}

void Window::on_action_Open_triggered()
{
	QString fn = QFileDialog::getOpenFileName(
									this,
									"Open file...",
									QDir::currentPath(),
									m_languages->fileFilters().join(";;"),
									0,
									0
								);

	load(fn);
}

void Window::on_action_Settings_triggered()
{

}

void Window::on_action_Reload_syntax_files_triggered()
{

}

void Window::on_action_About_triggered()
{
	QMessageBox::about(this, "About : QCodeEdit example",
						"QCodeEdit2 example v1.0\n"
						"Copyright &copy; 2006-2007 fullmetalcoder\n\n"
						"This small demo gives a fair idea\n"
						"of what QCodeEdit 2 can achieve.\n\n"
						"Hoping you'll like it :)\n");
}

void Window::on_action_About_Qt_triggered()
{
	QMessageBox::aboutQt(this, "About Qt 4");
}
