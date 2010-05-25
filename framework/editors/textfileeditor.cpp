/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2007-2010 by Ulrich Van Den Hekke                         *
 * ulrich.vdh@shadoware.org                                                *
 *                                                                         *
 * This program is free software: you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation, either version 3 of the License, or       *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This program is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
 * *********************************************************************** */

// xinx header
#include "editors/textfileeditor.h"
#include "editors/xinxcodeedit.h"
#include "editors/bookmarktexteditorinterface.h"
#include "actions/actioninterface.h"
#include "plugins/xinxpluginsloader.h"
#include "project/xinxproject.h"
#include "core/xinxconfig.h"
#include "borderlayout.h"
#include "contentview2/contentview2completionmodel.h"
#include "contentview2/contentview2manager.h"
#include "contentview2/contentview2cache.h"
#include "contentview2/contentview2node.h"
#include "contentview2/contentview2parser.h"
#include "contentview2/contentview2treemodel.h"

// Qt header
#include <QScrollBar>
#include <QPainter>
#include <QHBoxLayout>
#include <QFileInfo>
#include <QDir>
#include <QMessageBox>
#include <QTextStream>
#include <QApplication>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QAction>
#include <QContextMenuEvent>
#include <QMenu>
#include <QCompleter>
#include <QAbstractItemView>
#include <QTextCodec>
#include <QUuid>
#include <QBuffer>

/* TextFileEditor */

#ifdef Q_WS_X11
#   define DEFAULT_EOL TextFileEditor::UnixEndOfLine
#else
#ifdef Q_WS_MAC
#   define DEFAULT_EOL TextFileEditor::MacEndOfLine
#else
#   define DEFAULT_EOL TextFileEditor::WindowsEndOfLine
#endif
#endif

TextFileEditor::TextFileEditor(XinxCodeEdit * editor, QWidget *parent) : AbstractEditor(parent), m_view(editor), m_eol(DEFAULT_EOL), m_completionModel(0)
{
	initObjects();
}

TextFileEditor::~TextFileEditor()
{
	ContentView2::Manager::self()->cache()->unregisterPath(m_uuid);
	ErrorManager::self()->clearMessages(m_uuid);
	ErrorManager::self()->removeContextTranslation(m_uuid);
	delete m_model;
}

void TextFileEditor::initObjects()
{
	m_buffer = new QBuffer(this);
	m_uuid   = QUuid::createUuid().toString();
	m_file   = ContentView2::FileContainer(XINXProjectManager::self()->project(), m_uuid, false);
	ContentView2::Manager::self()->cache()->registerPath(m_uuid);

	m_keyTimer = new QTimer();
	m_keyTimer->setSingleShot(true);
	m_keyTimer->setInterval(XINXConfig::self()->config().editor.automaticModelRefreshTimeout);
	connect(m_keyTimer, SIGNAL(timeout()), this, SLOT(updateModel()));

	// Set filter event layout.
	if (! m_view)
		m_view = new XinxCodeEdit(this);
	else
		m_view->setParent(this);

	setContextMenuPolicy(Qt::DefaultContextMenu);
	m_view->setContextMenuPolicy(Qt::NoContextMenu);
	m_view->editor()->setContextMenuPolicy(Qt::NoContextMenu);
	//installEventFilter( this );
	//m_view->installEventFilter( this );

	m_bookmarkInterface = new BookmarkTextEditorInterface(this);
	m_bookmarkInterface->setTextEditor(m_view);

	connect(m_view, SIGNAL(copyAvailable(bool)), this, SIGNAL(copyAvailable(bool)));
	connect(m_view, SIGNAL(copyAvailable(bool)), this, SIGNAL(selectionAvailable(bool)));
	connect(m_view, SIGNAL(undoAvailable(bool)), this, SIGNAL(undoAvailable(bool)));
	connect(m_view, SIGNAL(redoAvailable(bool)), this, SIGNAL(redoAvailable(bool)));

	connect(m_view->editor(), SIGNAL(contentModified(bool)), this, SLOT(setModified(bool)));
	connect(textEdit()->document(), SIGNAL(contentsChanged()), this, SLOT(textChanged()));

	connect(m_view, SIGNAL(searchWord(QString)), this, SLOT(searchWord(QString)));

	m_model = new ContentView2::TreeModel(ContentView2::Manager::self()->database(), m_file, this);

	connect(ContentView2::Manager::self()->cache(), SIGNAL(cacheLoaded(ContentView2::File)), this, SLOT(updateImports(ContentView2::File)), Qt::BlockingQueuedConnection);
	connect(ErrorManager::self(), SIGNAL(changed()), this, SLOT(errorChanged()));

	connect(textEdit()->editor(), SIGNAL(textEdited(QKeyEvent*)), this, SLOT(updateCodec()));
}

ContentView2::CompletionModel * TextFileEditor::createModel(QSqlDatabase db, QObject * parent)
{
	return new ContentView2::CompletionModel(db, m_file, parent);
}

void TextFileEditor::initCompleter()
{
	m_completionModel = createModel(ContentView2::Manager::self()->database(), this);
	connect(ContentView2::Manager::self()->cache(), SIGNAL(cacheLoaded()), m_completionModel, SLOT(select()), Qt::BlockingQueuedConnection);

	QCompleter * completer = new QCompleter(textEdit());
	completer->setModel(m_completionModel);
	textEdit()->setCompleter(completer);
}

void TextFileEditor::initLayout()
{
	QHBoxLayout * hbox = new QHBoxLayout;
	hbox->addWidget(m_view);

	hbox->setMargin(0);

	setLayout(hbox);
}

BookmarkEditorInterface * TextFileEditor::bookmarkInterface()
{
	return m_bookmarkInterface;
}

BookmarkTextEditorInterface * TextFileEditor::bookmarkTextInterface()
{
	return m_bookmarkInterface;
}

XinxCodeEdit * TextFileEditor::textEdit() const
{
	return m_view;
}

QString TextFileEditor::defaultFileName() const
{
	return tr("noname.txt");
}

void TextFileEditor::selectAll()
{
	textEdit()->editor()->selectAll();
}

void TextFileEditor::loadFromFile(const QString & fileName)
{
	ErrorManager::self()->addContextTranslation(m_uuid, fileName);
	AbstractEditor::loadFromFile(fileName);
	textEdit()->setFilename(fileName);
}

void TextFileEditor::saveToFile(const QString & fileName)
{
	ErrorManager::self()->addContextTranslation(m_uuid, fileName);
	AbstractEditor::saveToFile(fileName);
	textEdit()->setFilename(fileName);
}

void TextFileEditor::detectEOL(QIODevice & d)
{
	// Get the EOL of the file.
	char c;
	while (d.getChar(&c))
	{
		if (c == '\r')
		{
			m_eol = MacEndOfLine;
			if (d.getChar(&c) && (c == '\n'))
			{
				m_eol = WindowsEndOfLine;
			}
			break;
		}
		else if (c == '\n')
		{
			m_eol = UnixEndOfLine;
			break;
		}
	}

	d.reset();
}

void TextFileEditor::detectCodec(QIODevice & d)
{
	Q_UNUSED(d);

	setCodec(XINXConfig::self()->config().editor.defaultTextCodec);
}

void TextFileEditor::loadFromDevice(QIODevice & d)
{
	m_keyTimer->stop();

	detectEOL(d);
	detectCodec(d);

	QTextStream text(&d);
	text.setCodec(codec());

	QString textBuffer = text.readAll();
	if (m_eol == MacEndOfLine)
	{
		textBuffer.replace("\r", "\n");
	}
	m_view->setPlainText(textBuffer);
	m_keyTimer->stop();

	updateModel();
}

void TextFileEditor::saveToDevice(QIODevice & d)
{
	if (XINXConfig::self()->config().editor.autoindentOnSaving) autoIndent();
	m_eol = DEFAULT_EOL; // Don't conserve the EOL
	d.setTextModeEnabled(true);
	QTextStream text(&d);
	text.setCodec(codec());   // Use the real codec on save
	text << m_view->toPlainText();
}

void TextFileEditor::setModified(bool isModified)
{
	m_view->setModified(isModified);
	AbstractEditor::setModified(isModified);
}

QAbstractItemModel * TextFileEditor::model()  const
{
	return contentViewModel();
}

ContentView2::TreeModel * TextFileEditor::contentViewModel() const
{
	return m_model;
}

void TextFileEditor::initSearch(SearchOptions & options)
{
	m_cursorStart = textEdit()->textCursor();
	m_cursorEnd   = QDocumentCursor();

	QDocumentCursor selectionStart = m_cursorStart.selectionStart();
	QDocumentCursor selectionEnd   = m_cursorStart.selectionEnd();

	if (options.testFlag(SEARCH_FROM_START))
	{
		if (! options.testFlag(BACKWARD))
			m_cursorStart.movePosition(1, QDocumentCursor::Start, QDocumentCursor::MoveAnchor);
		else
			m_cursorStart.movePosition(1, QDocumentCursor::End, QDocumentCursor::MoveAnchor);
		options &= ~ SearchOptions(SEARCH_FROM_START);
	}
	else if (options.testFlag(ONLY_SELECTION) && ! options.testFlag(BACKWARD))
	{
		m_cursorStart.moveTo(selectionStart);
		m_cursorEnd   = m_cursorStart;
		m_cursorEnd.movePosition(selectionEnd.position() - selectionStart.position(), QDocumentCursor::Right, QDocumentCursor::MoveAnchor);
	}
	else if (options.testFlag(ONLY_SELECTION) && options.testFlag(BACKWARD))
	{
		m_cursorStart.moveTo(selectionEnd);
		m_cursorEnd   = m_cursorStart;
		m_cursorEnd.movePosition(selectionEnd.position() - selectionStart.position(), QDocumentCursor::Left, QDocumentCursor::MoveAnchor);
	}
	else if (options.testFlag(BACKWARD))
	{
		m_cursorStart.moveTo(selectionStart);
	}

	textEdit()->setTextCursor(m_cursorStart);
}

bool TextFileEditor::find(const QString & text, SearchOptions options)
{
	QDocumentCursor finded, tc = m_cursorStart;

	if (options.testFlag(BACKWARD))
		tc.moveTo(tc.selectionStart());
	else
		tc.moveTo(tc.selectionEnd());

	XinxCodeEdit::FindFlags flags;
	if (options.testFlag(BACKWARD)) flags ^= XinxCodeEdit::FindBackward;
	if (options.testFlag(MATCH_CASE)) flags ^= XinxCodeEdit::FindCaseSensitively;
	if (options.testFlag(WHOLE_WORDS)) flags ^= XinxCodeEdit::FindWholeWords;

	if (options.testFlag(REGULAR_EXPRESSION))
	{
		finded = textEdit()->find(QRegExp(text), tc, flags);
	}
	else
	{
		finded = textEdit()->find(text, tc, flags);
	}

	if (! finded.isNull())
	{
		if (options.testFlag(ONLY_SELECTION))
		{
			if ((! options.testFlag(BACKWARD)) && (m_cursorEnd < finded))
			{
				return false;
			}
			else if (options.testFlag(BACKWARD) && (finded < m_cursorEnd))
			{
				return false;
			}
		}

		m_cursorStart = finded;
		textEdit()->setTextCursor(finded);
	}

	return !finded.isNull();
}

void TextFileEditor::replace(const QString & from, const QString & to, SearchOptions options)
{
	Q_ASSERT(! m_cursorStart.isNull());

	if (! options.testFlag(REGULAR_EXPRESSION))
	{
		m_cursorStart.removeSelectedText();
		m_cursorStart.insertText(to);
		return;
	}

	QRegExp expression(from);
	expression.indexIn(m_cursorStart.selectedText());
	QStringList list = expression.capturedTexts();
	QString result(to);
	int index = 1;

	QStringList::iterator it = list.begin();
	while (it != list.end())
	{
		result = result.replace(QString("\\%1").arg(index), *it);
		it++;
		index++;
	}

	m_cursorStart.removeSelectedText();
	m_cursorStart.insertText(result);

}

ContentView2::Parser * TextFileEditor::createParser()
{
	return 0;
}

void TextFileEditor::updateModel()
{
	ContentView2::Parser * parser = createParser();
	if (! parser) {
		emit contentChanged();
		return ;
	}

	m_buffer->close();
	if (codec())
	{
		m_buffer->setData(codec()->fromUnicode(textEdit()->toPlainText()));
	}
	else
	{
		m_buffer->setData(qPrintable(textEdit()->toPlainText()));
	}
	m_buffer->open(QBuffer::ReadOnly);

	parser->setInputDevice(m_buffer);

	ContentView2::Manager::self()->cache()->addToCache(XINXProjectManager::self()->project(), m_uuid, QString(), "M", parser);

	emit contentChanged();
}

void TextFileEditor::textChanged()
{
	m_keyTimer->start();
}

void TextFileEditor::errorChanged()
{
	QList<int> lines;
	const QString ctxt = lastFileName().isEmpty() ? m_uuid : lastFileName();
	foreach(ErrorManager::Error err, ErrorManager::self()->errors().value(ctxt))
	{
		if (! lines.contains(err.line))
			lines << err.line - 1;
	}
	textEdit()->setErrors(lines);
}

void TextFileEditor::updateImports(const ContentView2::File & file)
{
	if ((file.path() == m_uuid) && (!file.isCached()))
	{
		m_file.reload(ContentView2::Manager::self()->database());
		m_model->select();

		QCompleter * c = textEdit()->completer();

		if (c && c->completionModel()->rowCount())
		{
			c->popup()->setCurrentIndex(c->completionModel()->index(0, 0));
		}
	}
}

ContentView2::FileContainer TextFileEditor::fileContainer() const
{
	return m_file;
}

void TextFileEditor::setCodec(const QString & text)
{
	if (m_codec != text)
	{
		m_codec = text;
		emit codecChanged();
	}
}

QTextCodec * TextFileEditor::codec()
{
	QTextCodec * codec = QTextCodec::codecForName(m_codec.toLatin1());
	if( ! codec )
	{
		codec = QTextCodec::codecForLocale();
	}

	return codec;
}

void TextFileEditor::updateCodec()
{

}

TextFileEditor::EndOfLineType TextFileEditor::eol() const
{
	return m_eol;
}

bool TextFileEditor::autoIndent()
{
	ErrorManager::self()->addMessage(lastFileName(), -1, ErrorManager::MessageWarning, tr("Can't indent this type of document"), QStringList());
	return false;
}

ContentView2::Node TextFileEditor::rootNode() const
{
	if (m_file.isValid(ContentView2::Manager::self()->database()))
	{
		int rootId = m_file.file(ContentView2::Manager::self()->database()).rootId();
		return ContentView2::Node(ContentView2::Manager::self()->database(), rootId);
	}
	else
	{
		return ContentView2::Node();
	}
}

void TextFileEditor::complete()
{
	QDocumentCursor cursor = textEdit()->textCursor();

	QString completionPrefix = textEdit()->textUnderCursor(cursor);
	QCompleter * c = textEdit()->completer();

	if (c)
	{
		if (completionPrefix != c->completionPrefix())
		{
			m_completionModel->setPrefix(completionPrefix);
			c->setCompletionPrefix(completionPrefix);
			c->popup()->setCurrentIndex(c->completionModel()->index(0, 0));
		}

		int x, y, h, w;
		QPoint pos = textEdit()->editor()->mapFromContents(textEdit()->textCursor().documentPosition());
		textEdit()->editor()->getPanelMargins(&x, &y, &h, &w);
		QRect cr(pos.x() + x, pos.y() + textEdit()->document()->fontMetrics().height(), 1, 1);

		cr.setWidth(c->popup()->sizeHintForColumn(0) + c->popup()->verticalScrollBar()->sizeHint().width());
		c->complete(cr);
	}
}

void TextFileEditor::contextMenuEvent(QContextMenuEvent * contextMenuEvent)
{
	QMenu * menu = new QMenu(m_view);

	foreach(XinxPluginElement * e, XinxPluginsLoader::self()->plugins())
	{

		// Si le plugin est activé
		if (e->isActivated() && qobject_cast<IXinxPlugin*>(e->plugin()))
		{
			XinxAction::MenuList menuList = qobject_cast<IXinxPlugin*>(e->plugin())->actions();

			// Pour chaque menu
			foreach(const XinxAction::ActionList & aMenu, menuList)
			{

				// Pour chaque élemént du menu
				foreach(XinxAction::MenuItem * item, aMenu)
				{
					XinxAction::Action* xinxAction = dynamic_cast<XinxAction::Action*>(item);
					if (xinxAction && xinxAction->isInPopupMenu())
					{
						xinxAction->updateActionState();

						menu->addAction(xinxAction->action());
					}
				}

				menu->addSeparator();
			}
		}
	}

	menu->addAction(undoAction());
	menu->addAction(redoAction());
	menu->addSeparator();
	menu->addAction(cutAction());
	menu->addAction(copyAction());
	menu->addAction(pasteAction());

	menu->exec(contextMenuEvent->globalPos());
	delete menu;
}

void TextFileEditor::searchWord(const QString & word)
{
	textEdit()->completer();
	ContentView2::Node n = m_completionModel->nodeOfWord(word);
	if (n.isValid())
	{
		ContentView2::File file = n.file(ContentView2::Manager::self()->database());
		if (file.isCached())
			emit open(file.path(), n.line());
		else
			emit open(QString(), n.line());
		return;
	}
	QMessageBox::information(this, tr("Search Word"), tr("Word %1 not found").arg(word));
}

void TextFileEditor::serialize(XinxProjectSessionEditor * data, bool content)
{
	AbstractEditor::serialize(data, content);

	data->writeProperty("Position", QVariant(m_view->textCursor().position()));

	if (content && isModified())
	{
		data->writeProperty("Content", QVariant(m_view->toPlainText()));
	}

	int i = 0;
	foreach(int line, m_view->listOfBookmark())
	{
		data->writeProperty(QString("Bookmark_%1").arg(i++), QVariant(line));
	}
	data->writeProperty("BookmarkCount", QVariant(i));
}

void TextFileEditor::deserialize(XinxProjectSessionEditor * data)
{
	int position = 0;
	QString text;

	position  = data->readProperty("Position") .toInt();
	text = data->readProperty("Content").toString();

	if (! text.isEmpty())
	{
		m_view->setPlainText(text);

		AbstractEditor::deserialize(data);
	}
	else
	{
		AbstractEditor::deserialize(data);

		if (! lastFileName().isEmpty())
			loadFromFile(lastFileName());
	}
	m_view->setFilename(lastFileName());

	int bc = data->readProperty("BookmarkCount").toInt();
	for (int i = 0 ; i < bc; i++)
	{
		m_bookmarkInterface->setBookmark(data->readProperty(QString("Bookmark_%1").arg(i)).toInt(), true);
	}

	QDocumentCursor tc(textEdit()->editor()->document());
	tc.movePosition(position, QDocumentCursor::Right);
	m_view->setTextCursor(tc);
}

bool TextFileEditor::canCopy()
{
	return textEdit()->textCursor().hasSelection();
}

bool TextFileEditor::canPaste()
{
	return textEdit()->canPaste();
}

bool TextFileEditor::canUndo()
{
	return textEdit()->editor()->canUndo();
}

bool TextFileEditor::canRedo()
{
	return textEdit()->editor()->canRedo();
}

void TextFileEditor::undo()
{
	textEdit()->editor()->undo();
}

void TextFileEditor::redo()
{
	textEdit()->editor()->redo();
}

void TextFileEditor::cut()
{
	textEdit()->editor()->cut();
}

void TextFileEditor::copy()
{
	textEdit()->editor()->copy();
}

void TextFileEditor::paste()
{
	textEdit()->editor()->paste();
}
