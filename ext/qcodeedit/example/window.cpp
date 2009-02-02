
#include "window.h"

#include "qpanel.h"
#include "qeditor.h"
#include "qcodeedit.h"
#include "qeditconfig.h"

#include "qdocument.h"
#include "qdocumentline.h"
#include "qdocumentcursor.h"

#include "qformatscheme.h"
#include "qlanguagefactory.h"

#include "qlinemarksinfocenter.h"

#include <QDir>
#include <QTime>
#include <QFile>
#include <QSettings>
#include <QFileInfo>
#include <QCloseEvent>
#include <QMessageBox>
#include <QFileDialog>

static QMap<QString, QVariant> readSettingsMap(const QSettings& s)
{
	QMap<QString, QVariant> m;
	QStringList c = s.childKeys();
	
	foreach ( QString k, c )
		m[k] = s.value(k);
	
	return m;
}

static void writeSettingsMap(QSettings& s, const QMap<QString, QVariant>& m, const QString& g = QString())
{
	QMap<QString, QVariant>::const_iterator i = m.constBegin();
	
	if ( g.count() )
		s.beginGroup(g);
	
	while ( i != m.constEnd() )
	{
		s.setValue(i.key(), *i);
		
		++i;
	}
	
	if ( g.count() )
		s.endGroup();
	
}

Window::Window(QWidget *p)
{
	setupUi(this);
	
	// QCE setup
	m_formats = new QFormatScheme("qxs/formats.qxf", this);
	QDocument::setDefaultFormatScheme(m_formats);
	
	QLineMarksInfoCenter::instance()->loadMarkTypes("qxs/marks.qxm");
	
	m_languages = new QLanguageFactory(m_formats, this);
	m_languages->addDefinitionPath("qxs");
	
	m_editControl = new QCodeEdit(this);
	
	m_editControl
		->addPanel("Line Mark Panel", QCodeEdit::West, true)
		->setShortcut(QKeySequence("F6"));
	
	m_editControl
		->addPanel("Line Number Panel", QCodeEdit::West, true)
		->setShortcut(QKeySequence("F11"));
	
	m_editControl
		->addPanel("Fold Panel", QCodeEdit::West, true)
		->setShortcut(QKeySequence("F9"));
	
	m_editControl
		->addPanel("Line Change Panel", QCodeEdit::West, true)
		; //->setShortcut(QKeySequence("F11"));
	
	m_editControl
		->addPanel("Status Panel", QCodeEdit::South, true);
	
	m_editControl
		->addPanel("Search Replace Panel", QCodeEdit::South);
	
	connect(m_editControl->editor()	, SIGNAL( contentModified(bool) ),
			this					, SLOT  ( setWindowModified(bool) ) );
	
	m_stack->insertWidget(1, m_editControl->editor());
	
	// create toolbars
	QToolBar *edit = new QToolBar(tr("Edit"), this);
	edit->setIconSize(QSize(24, 24));
	edit->addAction(m_editControl->editor()->action("undo"));
	edit->addAction(m_editControl->editor()->action("redo"));
	edit->addSeparator();
	edit->addAction(m_editControl->editor()->action("cut"));
	edit->addAction(m_editControl->editor()->action("copy"));
	edit->addAction(m_editControl->editor()->action("paste"));
	edit->addSeparator();
	edit->addAction(m_editControl->editor()->action("indent"));
	edit->addAction(m_editControl->editor()->action("unindent"));
	//edit->addAction(m_editControl->editor()->action("comment"));
	//edit->addAction(m_editControl->editor()->action("uncomment"));
	addToolBar(edit);
	
	QToolBar *find = new QToolBar(tr("Find"), this);
	find->setIconSize(QSize(24, 24));
	find->addAction(m_editControl->editor()->action("find"));
	//find->addAction(m_editControl->editor()->action("findNext"));
	find->addAction(m_editControl->editor()->action("replace"));
	find->addAction(m_editControl->editor()->action("goto"));
	addToolBar(find);
	
	// settings restore
	QSettings settings;
	
	// general settings page
	QStringList panels = QStringList()
		<< "Line numbers"
		<< "Line marks"
		<< "Line changes"
		<< "Fold indicators"
		<< "Status"
		;
	
	settings.beginGroup("display");
	
	settings.beginGroup("panels");
	
	foreach ( QString p, panels )
	{
		// show all but line marks by default
		bool show = settings.value(p.toLower().replace(' ', '_'), p != "Line marks").toBool();
		
		if ( !show )
			m_editControl->sendPanelCommand(p, "hide");
	}
	
	settings.endGroup();
	
	bool wrap = settings.value("dynamic_word_wrap", false).toBool();
	chkWrap->setChecked(wrap);
	//m_editControl->editor()->setFlag(QEditor::LineWrap, wrap);
	
	bool cmwwb = settings.value("cursor_movement_within_wrapped_blocks", true).toBool();
	chkMoveInWrap->setChecked(cmwwb);
	//m_editControl->editor()->setFlag(QEditor::CursorJumpPastWrap, cmwwb);
	
	settings.endGroup();
	
	int flags = QEditor::defaultFlags();
	
	if ( wrap )
		flags |= QEditor::LineWrap;
	else
		flags &= ~QEditor::LineWrap;
	
	if ( cmwwb )
		flags |= QEditor::CursorJumpPastWrap;
	else
		flags &= ~QEditor::CursorJumpPastWrap;
	
	QEditor::setDefaultFlags(flags);
	
	spnRecent->setValue(settings.value("files/max_recent", 10).toInt());
	updateRecentFiles();
	
	// editor settings page
	m_config = new QEditConfig(settingsStack);
	settings.beginGroup("edit");
	m_config->loadKeys(readSettingsMap(settings));
	settings.endGroup();
	m_config->apply();
	settingsStack->addWidget(m_config);
	
	// syntax settings page
	settingsStack->addWidget(new QWidget(settingsStack));
	
	// restore GUI state
	settings.beginGroup("gui");
	
	int winwidth = settings.value("width", 0).toInt();
	int winheight = settings.value("height", 0).toInt();
	
	if ( winwidth > 0 && winheight > 0 )
	{
		resize(winwidth, winheight);
		
		QPoint winpos = settings.value("position").toPoint();
		
		if ( !winpos.isNull() )
			move(winpos);
		
		int winstate = settings.value("winstate").toInt();
		
		if ( !(winstate & Qt::WindowMinimized) )
			setWindowState(Qt::WindowStates(winstate));
		
	} else {
		setWindowState(Qt::WindowMaximized);
	}
	
	settings.endGroup();
	
	setWindowTitle("QCodeEdit::Demo [untitled[*]]");
}

void Window::closeEvent(QCloseEvent *e)
{
	if ( maybeSave() )
	{
		e->ignore();
		return;
	}
	
	e->accept();
	
	QSettings settings;
	settings.beginGroup("gui");
	settings.setValue("winstate", (int)windowState());
	settings.setValue("title", windowTitle());
	settings.setValue("width", width());
	settings.setValue("height", height());
	settings.setValue("position", pos());
	settings.endGroup();
}

bool Window::maybeSave()
{
	if ( m_editControl->editor()->isContentModified() )
	{
		int ret = QMessageBox::warning(
							this,
							tr("About to quit"),
							tr(
								"The open document contains unsaved modifications.\n"
								"Save it as %1 ?"
							).arg(m_editControl->editor()->fileName()),
							QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
							QMessageBox::Yes
						);
		
		if ( ret == QMessageBox::Cancel )
		{
			return true;
		} else if ( ret == QMessageBox::Yes ) {
			m_editControl->editor()->save();
		}
	}
	
	return false;
}

void Window::load(const QString& file)
{
	QTime t;
	t.start();
	
	QString filename = file.count() ? QFileInfo(file).absoluteFilePath() : file;
	
	if ( filename.count() && QFile::exists(filename) )
	{
		//qDebug("load : %s", qPrintable(filename));
		m_languages->setLanguage(m_editControl->editor(), filename);
		m_editControl->editor()->load(filename);
		
		updateRecentFiles(filename);
		
		setWindowTitle(QString("QCodeEdit::Demo [%1[*]]").arg(filename));
	} else {
		//qDebug("fallback...");
		m_languages->setLanguage(m_editControl->editor(), ""); //loading_failed.cpp");
		m_editControl->editor()->setFileName(""); // Loading failed\n// fallback to C++\n");
		m_editControl->editor()->setText(""); // Loading failed\n// fallback to C++\n");
		
		setWindowTitle("QCodeEdit::Demo [untitled[*]]");
	}
	
	m_stack->setCurrentIndex(1);
	//qDebug("loading took %i ms", t.elapsed());
}

void Window::on_action_New_triggered()
{
	if ( maybeSave() )
		return;
	
	load(QString());
}

void Window::on_action_Open_triggered()
{
	if ( maybeSave() )
		return;
	
	QString fn = QFileDialog::getOpenFileName(
									this,
									"Open file...",
									QFileInfo(m_editControl->editor()->fileName()).path(), //QDir::currentPath(),
									m_languages->fileFilters().join(";;"),
									0,
									0
								);
	
	if ( fn.count() )
		load(fn);
}

void Window::on_action_Save_triggered()
{
	QString old = m_editControl->editor()->fileName();
	
	m_editControl->editor()->save();
	
	QString file = m_editControl->editor()->fileName();
	
	if ( file != old )
	{
		//qDebug("fn changed from %s to %s", qPrintable(old), qPrintable(file));
		m_languages->setLanguage(m_editControl->editor(), file);
		//m_editControl->editor()->highlight();
		setWindowTitle(QString("QCodeEdit::Demo [%1[*]]").arg(file));
	}
}

void Window::on_action_SaveAs_triggered()
{
	QString fn = QFileDialog::getSaveFileName(
									this,
									tr("Save file as..."),
									m_editControl->editor()->fileName(), //QDir::currentPath(),
									m_languages->fileFilters().join(";;")
								);
	
	if ( fn.isEmpty() )
		return;
	
	m_editControl->editor()->save(fn);
	m_languages->setLanguage(m_editControl->editor(), fn);
	//m_editControl->editor()->highlight();
	setWindowTitle(QString("QCodeEdit::Demo [%1[*]]").arg(fn));
}

void Window::on_action_Print_triggered()
{
	m_editControl->editor()->print();
}

void Window::on_clbEdit_clicked()
{
	m_stack->setCurrentIndex(1);
}

void Window::on_clbMore_clicked()
{
	welcomeStack->setCurrentIndex(1);
}

void Window::on_clbLess_clicked()
{
	welcomeStack->setCurrentIndex(0);
}

void Window::on_bbSettings_clicked(QAbstractButton *b)
{
	QDialogButtonBox::ButtonRole r = bbSettings->buttonRole(b);
	
	QSettings settings;
	
	if ( r == QDialogButtonBox::AcceptRole )
	{
		// General section
		settings.beginGroup("display");
		settings.setValue("line_numbers", chkLineNumbers->isChecked());
		settings.setValue("fold_indicators", chkFoldPanel->isChecked());
		settings.setValue("line_marks", chkLineMarks->isChecked());
		settings.setValue("line_changes", chkLineChange->isChecked());
		settings.setValue("status", chkStatusPanel->isChecked());
		
		settings.setValue("dynamic_word_wrap", chkWrap->isChecked());
		settings.setValue("cursor_move_within_wrapped_blocks", chkMoveInWrap->isChecked());
		
		m_editControl->sendPanelCommand("Line numbers", chkLineNumbers->isChecked() ? "show" : "hide");
		m_editControl->sendPanelCommand("Fold indicators", chkFoldPanel->isChecked() ? "show" : "hide");
		m_editControl->sendPanelCommand("Line marks", chkLineMarks->isChecked() ? "show" : "hide");
		m_editControl->sendPanelCommand("Line changes", chkLineChange->isChecked() ? "show" : "hide");
		m_editControl->sendPanelCommand("Status", chkStatusPanel->isChecked() ? "show" : "hide");
		
		settings.beginGroup("panels");
		
		QList<QPanel*> panels = m_editControl->panels();
		
		foreach ( QPanel *p, panels )
		{
			QString s = p->type();
			settings.setValue(s.toLower().replace(' ', '_'), p->isVisibleTo(m_editControl->editor()));
		}
		
		settings.endGroup();
		
		settings.endGroup();
		
		int flags = QEditor::defaultFlags();
		
		if ( chkWrap->isChecked() )
			flags |= QEditor::LineWrap;
		else
			flags &= ~QEditor::LineWrap;
		
		if ( chkMoveInWrap->isChecked() )
			flags |= QEditor::CursorJumpPastWrap;
		else
			flags &= ~QEditor::CursorJumpPastWrap;
		
		QEditor::setDefaultFlags(flags);
		
		//m_editControl->editor()->setLineWrapping(chkWrap->isChecked());
		//m_editControl->editor()->setFlag(QEditor::CursorJumpPastWrap, chkWrap->isChecked() && chkMoveInWrap->isChecked());
		
		settings.beginGroup("files");
		settings.setValue("max_recent", spnRecent->value());
		settings.endGroup();
		
		updateRecentFiles();
		
		// Editor section
		m_config->apply();
		writeSettingsMap(settings, m_config->dumpKeys(), "edit");
		
		// Syntax section
		
		m_stack->setCurrentIndex(1);
	} else if ( r == QDialogButtonBox::RejectRole ) {
		// General section
		
		//chkWrap->setChecked(m_editControl->editor()->flag(QEditor::LineWrap));
		//chkMoveInWrap->setChecked(m_editControl->editor()->flag(QEditor::CursorJumpPastWrap));
		
		spnRecent->setValue(settings.value("files/max_recent", 10).toInt());
		
		// Editor section
		m_config->cancel();
		
		// Syntax section
		
		m_stack->setCurrentIndex(1);
	} else if ( r == QDialogButtonBox::ResetRole ) {
		// General section
		chkLineNumbers->setChecked(true);
		m_editControl->sendPanelCommand("Line numbers", "show");
		chkFoldPanel->setChecked(true);
		m_editControl->sendPanelCommand("Fold indicators", "show");
		chkLineMarks->setChecked(false);
		m_editControl->sendPanelCommand("Line marks", "hide");
		chkLineChange->setChecked(true);
		m_editControl->sendPanelCommand("Line changes", "show");
		chkStatusPanel->setChecked(true);
		m_editControl->sendPanelCommand("Status", "show");
		
		chkWrap->setChecked(false);
		chkMoveInWrap->setChecked(true);
		
		spnRecent->setValue(10);
		
		// Editor section
		m_config->restore();
		
		// Syntax section
		
	}
}

void Window::on_action_Settings_triggered()
{
	QEditor *e = m_editControl->editor();
	QList<QPanel*> panels = m_editControl->panels();
	
	foreach ( QPanel *p, panels )
	{
		if ( p->type() == "Line numbers" )
			chkLineNumbers->setChecked(p->isVisibleTo(e));
		
		if ( p->type() == "Fold indicators" )
			chkFoldPanel->setChecked(p->isVisibleTo(e));
		
		if ( p->type() == "Line marks" )
			chkLineMarks->setChecked(p->isVisibleTo(e));
		
		if ( p->type() == "Line changes" )
			chkLineChange->setChecked(p->isVisibleTo(e));
		
		if ( p->type() == "Status" )
			chkStatusPanel->setChecked(p->isVisibleTo(e));
		
	}
	
	int flags = QEditor::defaultFlags();
	chkWrap->setChecked(flags & QEditor::LineWrap);
	chkMoveInWrap->setChecked(flags & QEditor::CursorJumpPastWrap);
	
	m_stack->setCurrentIndex(m_stack->count() - 1);
}

void Window::on_action_Reload_syntax_files_triggered()
{
	
}

void Window::on_action_About_triggered()
{
	m_stack->setCurrentIndex(0);
}

void Window::on_action_About_Qt_triggered()
{
	QMessageBox::aboutQt(this, "About Qt 4");
}

void Window::on_menu_Recent_triggered(QAction *a)
{
	if ( !a )
		return;
	
	if ( a == action_Clear )
	{
		QSettings settings;
		settings.beginGroup("files");
		settings.setValue("recent_list", QStringList());
		settings.endGroup();
		updateRecentFiles();
		
		return;
	}
	
	if ( maybeSave() )
		return;
	
	load(a->objectName());
}

void Window::updateRecentFiles(const QString& filename)
{
	QSettings settings;
	settings.beginGroup("files");
	
	int max = settings.value("max_recent", 10).toInt();
	QStringList l = settings.value("recent_list").toStringList();
	
	menu_Recent->removeAction(action_Clear);
	menu_Recent->clear();
	menu_Recent->addAction(action_Clear);
	menu_Recent->addSeparator();
	
	if ( filename.count() )
	{
		l.removeAll(filename);
		l.prepend(filename);
	}
	
	while ( l.count() > max )
	{
		QString fn = l.takeLast();
	}
	
	int count = 0;
	
	foreach ( QString fn, l )
	{
		QString label = QString("%1 [%2]").arg(QFileInfo(fn).fileName()).arg(fn);
		
		if ( count < 10 )
		{
			label.prepend(' ');
			label.prepend('0' + count);
			label.prepend('&');
		}
		
		QAction *a = new QAction(label, menu_Recent);
		a->setObjectName(fn);
		
		menu_Recent->addAction(a);
		++count;
	}
	
	settings.setValue("recent_list", l);
	settings.endGroup();
}

