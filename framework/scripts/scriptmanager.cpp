/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2009 by Ulrich Van Den Hekke                              *
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

// Xinx header
#include "scriptmanager.h"
#include "core/xinxcore.h"
#include "project/xinxproject.h"
#include "editors/abstracteditor.h"
#include "editors/textfileeditor.h"
#include "editors/xinxcodeedit.h"
#include "scripts/documentsearch.h"

// Qt header
#include <QApplication>
#include <QMutex>
#include <QMutexLocker>
#include <QDir>
#include <QFileInfo>
#include <QTextStream>
#include <QMessageBox>
#include <QInputDialog>
#include <QTextEdit>

/* Constante */

#if QT_VERSION <= 0x040500
#   define UndefinedValue QScriptValue()
#else
#   define UndefinedValue QScriptValue::UndefinedValue
#endif


/* Static member */

ScriptManager * ScriptManager::s_self = 0;

/* Global script function */

static QScriptValue alert(QScriptContext * context, QScriptEngine *)
{
	if (context->argumentCount() == 0)
		context->throwError("Not enough parameter");

	for (int i = 0; i < context->argumentCount(); i ++)
	{
		QScriptValue arg = context->argument(i);
		QMessageBox::information(0, "Script message", arg.toString());
	}
	return QScriptValue();
}

static QScriptValue confirm(QScriptContext * context, QScriptEngine * interpreter)
{
	if (context->argumentCount() == 0)
		context->throwError("Not enough parameter");
	QString message = context->argument(0).toString();
	int r = QMessageBox::question(0, "Script question", message, QMessageBox::Yes | QMessageBox::No);
	return QScriptValue(interpreter, r == QMessageBox::Yes);
}

static QScriptValue input(QScriptContext * context, QScriptEngine * interpreter)
{
	if (context->argumentCount() == 0)
		context->throwError("Not enough parameter");

	QString message = context->argument(0).toString();
	if ((context->argumentCount() > 1) && (context->argument(1).isNumber()))
	{
		int defaultValue = 0;
		if (context->argument(1).isNumber()) defaultValue = (int)context->argument(1).toNumber();
		int result = QInputDialog::getInteger(0, "Script input", message, defaultValue);
		return QScriptValue(interpreter, result);
	}
	else
	{
		QString defaultValue = context->argument(1).isString() ? context->argument(1).toString() : "";
		QString result = QInputDialog::getText(0, "Script input", message, QLineEdit::Normal, defaultValue);
		return QScriptValue(interpreter, result);
	}
}

static QScriptValue documentSearch_constructor(QScriptContext * context, QScriptEngine * interpreter)
{
	XinxCodeEdit * textEdit = qobject_cast<XinxCodeEdit*>(context->argument(0).toQObject());
	if (textEdit)
		return interpreter->newQObject(new DocumentSearch(textEdit), QScriptEngine::ScriptOwnership);
	else
		return interpreter->nullValue();
}

/* Global conversion fonction */

Q_DECLARE_METATYPE(DocumentSearchOption*);

static QScriptValue documentSearchOptionToScriptValue(QScriptEngine *engine, DocumentSearchOption * const &in)
{
	return engine->newQObject(in);
}

static void documentSearchOptionFromScriptValue(const QScriptValue &object, DocumentSearchOption* &out)
{
	out = qobject_cast<DocumentSearchOption*>(object.toQObject());
}

/* ScriptValue */

ScriptValue::ScriptValue()
{
}


ScriptValue::ScriptValue(QScriptValue value) : m_value(value)
{

}

const QScriptValue & ScriptValue::value() const
{
	return m_value;
}

QString ScriptValue::text() const
{
	return m_value.property("text").toString();
}

bool ScriptValue::isCallBeforeSave() const
{
	return m_value.property("beforeSave").isFunction();
}

void ScriptValue::callScriptBeforeSave()
{
	QScriptValue result = m_value.property("beforeSave").call(m_value);
	if (result.isError())
	{
		qWarning(qPrintable(qApp->translate("ScriptValue", "An error occure while run the script : %1").arg(result.toString())));
	}
}

bool ScriptValue::isCallBeforeLoad() const
{
	return m_value.property("beforeLoad").isFunction();
}

void ScriptValue::callScriptBeforeLoad()
{
	QScriptValue result = m_value.property("beforeLoad").call(m_value);
	if (result.isError())
	{
		qWarning(qPrintable(qApp->translate("ScriptValue", "An error occure while run the script : %1").arg(result.toString())));
	}
}

bool ScriptValue::isCallAfterSave() const
{
	return m_value.property("afterSave").isFunction();
}

void ScriptValue::callScriptAfterSave()
{
	QScriptValue result = m_value.property("afterSave").call(m_value);
	if (result.isError())
	{
		qWarning(qPrintable(qApp->translate("ScriptValue", "An error occure while run the script : %1").arg(result.toString())));
	}
}

bool ScriptValue::isCallAfterLoad() const
{
	return m_value.property("afterLoad").isFunction();
}

void ScriptValue::callScriptAfterLoad()
{
	QScriptValue result = m_value.property("afterLoad").call(m_value);
	if (result.isError())
	{
		qWarning(qPrintable(qApp->translate("ScriptValue", "An error occure while run the script : %1").arg(result.toString())));
	}
}

void ScriptValue::callScript()
{
	QScriptValue result = m_value.property("run").call(m_value);
	if (result.isError())
	{
		qWarning(qPrintable(qApp->translate("ScriptValue", "An error occure while run the script : %1").arg(result.toString())));
	}
}

/* ScriptManager */

ScriptManager::ScriptManager()
{
	qScriptRegisterMetaType(&m_engine, &documentSearchOptionToScriptValue, &documentSearchOptionFromScriptValue);

	QScriptValue qsAlert = m_engine.newFunction(alert);
	m_engine.globalObject().setProperty("alert", qsAlert);

	QScriptValue qsConfirm = m_engine.newFunction(confirm);
	m_engine.globalObject().setProperty("confirm", qsConfirm);

	QScriptValue qsInput = m_engine.newFunction(input);
	m_engine.globalObject().setProperty("input", qsInput);

	QScriptValue qsDocumentSearch = m_engine.newFunction(documentSearch_constructor);
	QScriptValue qsMetaObjectDocumentSearch = m_engine.newQMetaObject(&DocumentSearch::staticMetaObject, qsDocumentSearch);
	m_engine.globalObject().setProperty("DocumentSearch", qsDocumentSearch);

	loadScripts();
	projectChange();
	setCurrentEditeur(0);

	connect(XINXProjectManager::self(), SIGNAL(changed()), this, SLOT(projectChange()));
}

ScriptManager::~ScriptManager()
{

}

void ScriptManager::loadScripts()
{
	QDir scriptDir("scripts:");

	m_objects.clear();
	QStringList filenames = scriptDir.entryList(QStringList("*.js"), QDir::Files);

	foreach(const QString & filename, filenames)
	{
		loadScript(scriptDir.absoluteFilePath(filename));
	}

	emit changed();
}

void ScriptManager::loadScript(const QString & filename)
{
	QFile qsFile(filename);
	if (! qsFile.open(QIODevice::ReadOnly))
	{
		qWarning(qPrintable(tr("Cannot read file : %1: %2").arg(QFileInfo(filename).fileName()).arg(qsFile.errorString())));
		return;
	}

	QTextStream in(&qsFile);
	in.setCodec("UTF-8");

	QString script = in.readAll();
	qsFile.close();

	QScriptValue qsScript = m_engine.evaluate(script);
	if (m_engine.hasUncaughtException())
	{
		qWarning(qPrintable(tr("An error occurred while executing the script %1 : %2 at line %3").arg(QFileInfo(filename).fileName()).arg(m_engine.uncaughtException().toString()).arg(m_engine.uncaughtExceptionLineNumber())));
		return;
	}

	ScriptValue sValue(qsScript);
	m_objects.append(sValue);

	m_filenames << filename;
}

void ScriptManager::callScriptsBeforeSave()
{
	foreach(ScriptValue v, m_objects)
	if (v.isCallBeforeSave())
		v.callScriptBeforeSave();
}

void ScriptManager::callScriptsAfterSave()
{
	foreach(ScriptValue v, m_objects)
	if (v.isCallAfterSave())
		v.callScriptAfterSave();
}

void ScriptManager::callScriptsBeforeLoad()
{
	foreach(ScriptValue v, m_objects)
	if (v.isCallBeforeLoad())
		v.callScriptBeforeLoad();
}

void ScriptManager::callScriptsAfterLoad()
{
	foreach(ScriptValue v, m_objects)
	if (v.isCallAfterLoad())
		v.callScriptAfterLoad();
}

const QList<ScriptValue> & ScriptManager::objects() const
{
	return m_objects;
}

QScriptEngine & ScriptManager::engine()
{
	return m_engine;
}

void ScriptManager::projectChange()
{
	if (m_project != XINXProjectManager::self()->project())
	{
		m_project = XINXProjectManager::self()->project();

		if (m_project)
		{
			QScriptValue qsProject = m_engine.newQObject(m_project);

			m_engine.globalObject().setProperty("project", qsProject);
		}
		else
		{
			m_engine.globalObject().setProperty("project", UndefinedValue);
		}
	}
}

void ScriptManager::setCurrentEditeur(AbstractEditor * editor)
{
	if (editor)
	{
		QScriptValue qsEditor = ScriptManager::self()->engine().newQObject(editor);
		m_engine.globalObject().setProperty("editor", qsEditor);

		TextFileEditor * textFileEditor = qobject_cast<TextFileEditor*>(editor);
		if (textFileEditor)
		{
			XinxCodeEdit * textEdit = textFileEditor->textEdit();
			QScriptValue qsTextEdit = m_engine.newQObject(textEdit);
			QScriptValue qsDocument = m_engine.newQObject(textEdit->document());
			qsTextEdit.setProperty("document", qsDocument);
			m_engine.globalObject().setProperty("textEdit", qsTextEdit);
		}
		else
		{
			m_engine.globalObject().setProperty("textEdit", UndefinedValue);
		}
	}
	else
	{
		m_engine.globalObject().setProperty("textEdit", UndefinedValue);
		m_engine.globalObject().setProperty("editor", UndefinedValue);
	}
}

ScriptManager * ScriptManager::self()
{
	static QMutex selfMutex;

	QMutexLocker locker(&selfMutex);
	if (s_self == 0)
	{
		s_self = new ScriptManager();
		XINXStaticDeleter::self()->addObject(s_self);
	}
	return s_self;
}


