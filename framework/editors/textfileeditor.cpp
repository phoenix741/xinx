/*
 XINX
 Copyright (C) 2007-2011 by Ulrich Van Den Hekke
 xinx@shadoware.org

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful.
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// xinx header
#include "editors/textfileeditor.h"
#include "editors/xinxcodeedit.h"
#include "editors/bookmarktexteditorinterface.h"
#include "actions/actioninterface.h"
#include "plugins/xinxpluginsloader.h"
#include "project/xinxprojectproject.h"
#include "session/sessioneditor.h"
#include "core/xinxconfig.h"
#include "borderlayout.h"
#include <plugins/xinxpluginelement.h>
#include <actions/actionmanager.h>
#include <plugins/interfaces/codecompletion.h>

#include <contentview3/parserfactory.h>
#include <contentview3/parser.h>
#include <contentview3/treemodel.h>
#include <contentview3/filenode.h>
#include <codecompletion/completer.h>
#include <codecompletion/model.h>
#include <codecompletion/pool.h>

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
#include <QBuffer>
#include <QClipboard>

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

/*!
 * \ingroup XinxEditors
 * \class TextFileEditor
 * Class used to represent a file editor. A file editor have a XinxCodeEditor class, who show the line in the document,
 * folding, ....
 * The File Editor is a subclass of Editor and implements all its pure virtual method.
 *
 * The File Editor propose a method to read the FileName : getFileName(), and the default suffix (for save as) to use (getSuffix).
 * If a file is open, getTitle() return the file name otherwise it return noname.
 *
 * FileEditor has also two methode for load and save file.
 *
 * An \e TextFileEditor load and unload the content view model editor when
 * reading the device. The content model is based on a ContentViewParser and
 * some ContentViewNode.
 */

/*!
 * \enum TextFileEditor::EndOfLineType
 * \brief Type of End Of Line that the editor can understand.
 */

/*!
 * \var TextFileEditor::WindowsEndOfLine
 * \brief The end of line is terminated by \\r\\n
 */

/*!
 * \var TextFileEditor::UnixEndOfLine
 * \brief The end of line is terminated by \\n
 */

/*!
 * \var TextFileEditor::MacEndOfLine
 * \brief The end of line is terminated by \\r\\n
 */


/*!
 * Construct a TextFileEditor with the help of a XinxCodeEdit and a parent.
 * \param editor XinxCodeEdit to use to print file to screen (center widget)
 * \param parent Parent and containers of the editor.
 */
TextFileEditor::TextFileEditor(XinxCodeEdit* editor, QWidget* parent): AbstractEditor(parent), _modification_timer(0), _move_timer(0), m_codec(XINXConfig::self()->config().editor.defaultTextCodec), m_view(editor), m_eol(DEFAULT_EOL), m_bookmarkInterface(0), _model(0), _completion_model(0), _completer(0)
{
	initObjects();
}

/*! Destructor of the FileEditor.*/
TextFileEditor::~TextFileEditor()
{
	ErrorManager::self()->clearMessages(lastFileName());
}

/*!
 * \fn void TextFileEditor::selectionAvailable(bool yes);
 * \brief Signal emitted when a text is selected or deselected.
 * \param yes The value is true if a part of the text is selected, else the value is false.
 */

/*!
 * \fn void TextFileEditor::codecChanged();
 * \brief This signal is emited when the codec is modified.
 */


void TextFileEditor::initObjects()
{
	_modification_timer.reset(new QTimer);
	_modification_timer->setSingleShot(true);
	_modification_timer->setInterval(XINXConfig::self()->config().editor.automaticModelRefreshTimeout);
	connect(_modification_timer.data(), SIGNAL(timeout()), this, SLOT(updateModel()));

	_move_timer.reset(new QTimer);
	_move_timer->setSingleShot(true);
	_move_timer->setInterval(100);
	connect(_move_timer.data(), SIGNAL(timeout()), this, SLOT(updateContext()));

	// Set the internal editor used by textfileeditor
	if (! m_view)
		m_view = new XinxCodeEdit(this);
	else
		m_view->setParent(this);

	m_view->setTextFileEditor(this);
	setContextMenuPolicy(Qt::DefaultContextMenu);
	m_view->setContextMenuPolicy(Qt::NoContextMenu);
	m_view->editor()->setContextMenuPolicy(Qt::NoContextMenu);

	m_bookmarkInterface = new BookmarkTextEditorInterface(this);
	m_bookmarkInterface->setTextEditor(m_view);

	connect(QApplication::clipboard(), SIGNAL(dataChanged()), this, SLOT(clipboardChanged()));
	connect(m_view, SIGNAL(copyAvailable(bool)), this, SIGNAL(copyAvailable(bool)));
	connect(m_view, SIGNAL(copyAvailable(bool)), this, SIGNAL(selectionAvailable(bool)));
	connect(m_view, SIGNAL(undoAvailable(bool)), this, SIGNAL(undoAvailable(bool)));
	connect(m_view, SIGNAL(redoAvailable(bool)), this, SIGNAL(redoAvailable(bool)));

	connect(m_view->editor(), SIGNAL(contentModified(bool)), this, SLOT(setModified(bool)));
	connect(m_view->document(), SIGNAL(contentsChanged()), this, SLOT(textChanged()));
	connect(m_view->editor(), SIGNAL(cursorPositionChanged()), this, SLOT(cursorPositionChanged()));

	connect(m_view, SIGNAL(searchWord(QString)), this, SLOT(searchWord(QString)));

	connect(ErrorManager::self(), SIGNAL(changed()), this, SLOT(errorChanged()));

	connect(m_view->editor(), SIGNAL(textEdited(QKeyEvent*)), this, SLOT(updateCodec()));

	/* Completer */
	_completer			= new CodeCompletion::Completer(this);
	_completion_model	= new CodeCompletion::Model(_completer);

	_completer->setModel(_completion_model);
	_completer->setContext(_context);
	_completer->setEditor(this);

	_model = new ContentView3::TreeModel(this);

	setFile(ContentView3::File::create());
}

void TextFileEditor::setProject(XinxProject::ProjectPtr project)
{
	if (AbstractEditor::project().data() != project.data())
	{
		AbstractEditor::setProject(project);
		_file->setProject(project);
	}
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

/*!
 * Return the text editor corresponding on the file editor. This editor can't be null.
 * \return The text editor widget used by the file editor.
 */
XinxCodeEdit * TextFileEditor::textEdit() const
{
	return m_view;
}

/*! Method used to select all the text in the editor. The call is sent to the TextEditor. */
void TextFileEditor::selectAll()
{
	m_view->editor()->selectAll();
}

void TextFileEditor::loadFromFile(const QString & fileName)
{
	_file->setFilename(fileName);
	AbstractEditor::loadFromFile(fileName);
	m_view->setFilename(fileName);
}

void TextFileEditor::saveToFile(const QString & fileName)
{
	_file->setFilename(fileName);
	AbstractEditor::saveToFile(fileName);
	m_view->setFilename(fileName);
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

	setCodecName(XINXConfig::self()->config().editor.defaultTextCodec);
}

void TextFileEditor::loadFromDevice(QIODevice & d)
{
	_modification_timer->stop();

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
	_modification_timer->stop();

	updateModel();
}

void TextFileEditor::saveToDevice(QIODevice & d)
{
	if (XINXConfig::self()->config().editor.autoindentOnSaving == "true") autoIndent();
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

CodeCompletion::Context TextFileEditor::context()
{
	return _context;
}

/*!
 * The default implementation return null
 * \sa contentViewModel()
 */
QAbstractItemModel * TextFileEditor::model()  const
{
	return contentViewModel();
}

/*!
 * Return the model used in the edi	tor as model() but with the type ContentViewModel
 * \sa model()
 */
ContentView3::TreeModel * TextFileEditor::contentViewModel() const
{
	return _model;
}

void TextFileEditor::initSearch(SearchOptions & options)
{
	m_cursorStart = m_view->textCursor();
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

	m_view->setTextCursor(m_cursorStart);
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
		finded = m_view->find(QRegExp(text), tc, flags);
	}
	else
	{
		finded = m_view->find(text, tc, flags);
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
		m_view->setTextCursor(finded);
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

//! Create a parser for the text editor (based on the paser type of the interface)
ContentView3::Parser * TextFileEditor::createParser()
{
	Q_ASSERT_X(fileTypePluginInterface(), "TextFileEditor::createParser", "An interface must be defined on an editor");

	if (fileTypePluginInterface())
	{
		const QString parserName = fileTypePluginInterface()->parserType();
		if (parserName.isEmpty()) return NULL;

		return ContentView3::ParserFactory::getParserByType(parserName);
	}

	return NULL;
}

/*!
 * \brief Update content view and completion with the content of the text editor.
 *
 * The default implementation create the parser by using \e createParser() with the content
 * of the editor, and run it.
 */
void TextFileEditor::updateModel()
{
	updateCodec();

	ContentView3::Parser * parser = createParser();
	if (parser)
	{
		connect(parser, SIGNAL(jobEnding()), this, SLOT(fileParsed()));

		QBuffer * buffer = new QBuffer;
		if (codec())
		{
			buffer->setData(codec()->fromUnicode(m_view->toPlainText()));
		}
		else
		{
			buffer->setData(qPrintable(m_view->toPlainText()));
		}

		parser->setFile(_file);
		if (project())
		{
			parser->setWorkingPath(project()->projectPath());
		}

		parser->setDevice(buffer);
		project()->cache()->addFileToCache(parser, true, ContentView3::Cache::NONE);
	}

	emit contentChanged();
}

void TextFileEditor::fileParsed()
{
	_model->reload();
	//_completion_model->updateModel ();
	qDebug() << "File " << _file->filename() << " reloaded.";
}

void TextFileEditor::textChanged()
{
	_modification_timer->start();
}

void TextFileEditor::errorChanged()
{
	QList<int> lines;
	foreach(ErrorManager::Error err, ErrorManager::self()->errors().value(lastFileName()))
	{
		if (! lines.contains(err.line))
			lines << err.line - 1;
	}
	m_view->setErrors(lines);
}

void TextFileEditor::clipboardChanged()
{
	emit pasteAvailable(canPaste());
}

void TextFileEditor::setFile(ContentView3::FilePtr file)
{
	if (_file != file)
	{
		_file = file;
		_file->setProject(project());

		_context.setFile(_file.toWeakRef());
		_model->setFile(_file);
		m_view->setFile(_file.toWeakRef());
	}
}

ContentView3::FilePtr TextFileEditor::file() const
{
	return _file;
}

//! Set the codec to \e text
void TextFileEditor::setCodecName(const QString & text)
{
	if (m_codec != text)
	{
		m_codec = text;
		emit codecChanged();
	}
}

//! Get the codec name
const QString & TextFileEditor::codecName() const
{
	return m_codec;
}

//! The codec used to read and write the file. By Default, the codec is defined in options.
QTextCodec * TextFileEditor::codec() const
{
	QTextCodec * codec = QTextCodec::codecForName(m_codec.toAscii());
	if (! codec)
	{
		codec = QTextCodec::codecForLocale();
	}

	return codec;
}

void TextFileEditor::updateCodec()
{

}

void TextFileEditor::cursorPositionChanged()
{
	const int line = m_view->currentRow() + 1;
	const int column = m_view->currentColumn() + 1;
	const QString text = QString("   %1 x %2   ").arg(line, 3, 10, QLatin1Char('0')).arg(column, 3, 10, QLatin1Char('0'));
	emit positionChanged(text);

	_move_timer->start();
}

void TextFileEditor::updateContext()
{
	_move_timer->stop();
	_context.setFilename(lastFileName());
	XinxPluginsLoader::self()->codeCompletionPool()->updateContext(this, _context);
}

/*!
 * Return the EndOfLine of the document. This can't be modified.
 * A newly created editor is in platform end of line type. Saving a file converte the
 * EndOfLine to the platform behavour.
 */
TextFileEditor::EndOfLineType TextFileEditor::eol() const
{
	return m_eol;
}

/*! Auto indent all the document (named Pretty Print). */
bool TextFileEditor::autoIndent()
{
	ErrorManager::self()->addMessage(lastFileName(), -1, QtWarningMsg, tr("Can't indent this type of document"), QStringList());
	return false;
}

/*! Call the completer of the text on the current position of the cursor, if possible. */
void TextFileEditor::complete()
{
	if (_completer)
	{
		_completer->complete(false);
	}
}

void TextFileEditor::contextMenuEvent(QContextMenuEvent * contextMenuEvent)
{
	QScopedPointer<QMenu> menu(new QMenu(m_view));

	foreach(XinxAction::MenuItem * item, XinxAction::ActionManager::self()->popup())
	{
		item->updateActionState();
		menu->addAction(item->action());
	}

	menu->exec(contextMenuEvent->globalPos());
}

ContentView3::NodePtr TextFileEditor::localNodeOfWord(const ContentView3::NodePtr & node, const QString & word)
{
	foreach(ContentView3::NodePtr child, node->childs())
	{
		if (child->name() == word)
		{
			return child;
		}
		else
		{
			ContentView3::NodePtr recursiveNode = localNodeOfWord(child, word);
			if (recursiveNode)
				return recursiveNode;
		}
	}

	return ContentView3::NodePtr();
}

ContentView3::NodePtr TextFileEditor::globalNodeOfWord(const QString & word)
{
	ContentView3::NodePtr node = localNodeOfWord(_file->rootNode().dynamicCast<ContentView3::Node> (), word);
	if (node.isNull() && project())
	{
		foreach(const ContentView3::FilePtr & import, project()->cache()->importOf(_file))
		{
			node = localNodeOfWord(import->rootNode().dynamicCast<ContentView3::Node> (), word);
			if (node)
			{
				break;
			}
		}
	}
	return node;
}

void TextFileEditor::searchWord(const QString & word)
{
	ContentView3::NodePtr n = globalNodeOfWord(word);
	if (n)
	{
		const QString filename = n->filename();
		emit open(filename, n->line());

		return; /* Founded */
	}
	QMessageBox::information(this, tr("Search Word"), tr("Word %1 not found").arg(word));
}

/*!
 * \brief Indicate if the snipet must be show or not in the completion list
 * \since 0.9.1.0
 *
 * Used by CodeCompletion::SnipetContextParser to know if snipet must be added
 * This methode must subclassed, if the return value must be different.
 *
 * By default this method return always true.
 */
bool TextFileEditor::isSnipetMustBeShow() const
{
	return true;
}

void TextFileEditor::serialize(XinxSession::SessionEditor * data, bool content)
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

void TextFileEditor::deserialize(XinxSession::SessionEditor * data)
{
	AbstractEditor::deserialize(data);

	int position = 0;
	QString text;

	position  = data->readProperty("Position") .toInt();
	text = data->readProperty("Content").toString();

	if (! text.isEmpty())
	{
		m_view->setPlainText(text);
	}
	else
	{
		if (! lastFileName().isEmpty())
			loadFromFile(lastFileName());
	}
	m_view->setFilename(lastFileName());

	int bc = data->readProperty("BookmarkCount").toInt();
	for (int i = 0 ; i < bc; i++)
	{
		m_bookmarkInterface->setBookmark(data->readProperty(QString("Bookmark_%1").arg(i)).toInt(), true);
	}

	QDocumentCursor tc(m_view->editor()->document());
	tc.movePosition(position, QDocumentCursor::Right);
	m_view->setTextCursor(tc);
}

bool TextFileEditor::canCopy()
{
	return m_view->textCursor().hasSelection();
}

bool TextFileEditor::canPaste()
{
	return m_view->canPaste();
}

bool TextFileEditor::canUndo()
{
	return m_view->editor()->canUndo();
}

bool TextFileEditor::canRedo()
{
	return m_view->editor()->canRedo();
}

void TextFileEditor::undo()
{
	m_view->editor()->undo();
}

void TextFileEditor::redo()
{
	m_view->editor()->redo();
}

void TextFileEditor::cut()
{
	m_view->editor()->cut();
}

void TextFileEditor::copy()
{
	m_view->editor()->copy();
}

void TextFileEditor::paste()
{
	m_view->editor()->paste();
}
