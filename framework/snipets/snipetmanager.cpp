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

// Xinx header
#include "core/xinxcore.h"
#include "snipets/snipetmanager.h"
#include "snipets/callsnipetdlg.h"
#include "snipets/snipetpropertydlgimpl.h"
#include "snipets/categorypropertydlgimpl.h"
#include "scripts/scriptmanager.h"
#include "snipets/snipetitemmodel.h"
#include "snipets/snipetdockitemmodel.h"
#include "snipets/categoryitemmodel.h"
#include "snipets/snipetmenu.h"
#include "snipets/snipet.h"

// Qt header
#include <QSqlError>
#include <QSqlQuery>
#include <QFileInfo>
#include <QScriptEngine>
#include <QInputDialog>
#include <QMessageBox>
#include <QDebug>
#include <QDir>

/* Static member */

SnipetManager * SnipetManager::s_self = 0;

/* SnipetManager */

SnipetManager::SnipetManager()
{

}

SnipetManager::~SnipetManager()
{
	m_handler.clear();
	closeDatabase();
	if (s_self == this)
		s_self = NULL;
}

SnipetManager * SnipetManager::self()
{
	if (s_self == 0)
	{
		s_self = new SnipetManager();
		XINXStaticDeleter::self()->addObject(s_self);
	}
	return s_self;
}

QSqlDatabase SnipetManager::database() const
{
	if (!QSqlDatabase::contains("SNIPETBASE"))
	{
		openDatabase();
	}
	return QSqlDatabase::database("SNIPETBASE", false);
}

SnipetItemModel * SnipetManager::createSnipetItemModel(QObject * parent)
{
	SnipetItemModel * model = new SnipetItemModel(database(), parent);
	m_handler.add(model);
	return model;
}

SnipetDockItemModel * SnipetManager::createSnipetDockItemModel(QObject * parent)
{
	SnipetDockItemModel * model = new SnipetDockItemModel(database(), parent);
	m_handler.add(model);
	return model;
}

CategoryItemModel * SnipetManager::createCategoryItemModel(QObject * parent)
{
	CategoryItemModel * model = new CategoryItemModel(database(), parent);
	m_handler.add(model);
	return model;
}

SnipetMenu * SnipetManager::createSnipetMenu(const QString & title, QWidget * parent)
{
	SnipetMenu * menu = new SnipetMenu(parent);
	menu->setTitle(title);

	SnipetItemModel * snipetModel = new SnipetMenuModel(database(), menu);
	m_handler.add(snipetModel);
	menu->setModel(snipetModel);

	return menu;
}

int SnipetManager::getCategoryId(const QStringList & category)
{
	int parentCategory = 1;
	QSqlQuery selectQuery("SELECT id FROM categories WHERE parent_id=:parentCategory AND LOWER(name) like LOWER(:name)", database());
	QSqlQuery insertQuery("INSERT INTO categories(parent_id, name) VALUES(:parentCategory, :name)", database());
	for (int index = 0; index < category.count(); index++)
	{
		const QString & categoryLevelName = category.at(index);

		selectQuery.bindValue(":parentCategory", parentCategory);
		selectQuery.bindValue(":name", categoryLevelName);

		bool result = selectQuery.exec();
		Q_ASSERT(result);

		if (selectQuery.next())
		{
			parentCategory = selectQuery.value(0).toInt();
		}
		else
		{
			insertQuery.bindValue(":parentCategory", parentCategory);
			insertQuery.bindValue(":name", categoryLevelName);
			result = insertQuery.exec();

			Q_ASSERT(result);

			// Afin de relancer la recherche courante.
			index--;
		}
	}
	return parentCategory;
}

QStringList SnipetManager::getCategoryName(int id)
{
	if (id == 0) return QStringList();
	QSqlQuery selectQuery("SELECT parent_id, name FROM categories WHERE id=:id", database());
	QStringList categories;
	int parent_id = id;
	while (parent_id > 1)
	{
		selectQuery.bindValue(":id", parent_id);
		bool result = selectQuery.exec();
		Q_ASSERT(result);

		if (! selectQuery.next())
		{
			break;
		}

		categories.insert(0, selectQuery.value(1).toString());
		parent_id = selectQuery.value(0).toInt();
	}
	return categories;
}

void SnipetManager::addCategory(int parentId, bool categoryAccess, QWidget * parent)
{
	CategoryPropertyDlgImpl dlg(database(), parent);
	dlg.setParentId(parentId);
	dlg.setCategoryAccess(categoryAccess);
	dlg.exec();
}

void SnipetManager::removeCategory(int id, QWidget * parent)
{
	Q_ASSERT(id != 0);

	QSqlQuery countQuery("select (select count(1) from categories where parent_id=:snipet_id1) + (select count(1) from snipets where category_id=:snipet_id2) as count", database());
	countQuery.bindValue(":snipet_id1", id);
	countQuery.bindValue(":snipet_id2", id);
	bool result = countQuery.exec();

	Q_ASSERT(result);

	countQuery.next();

	int count = countQuery.value(0).toInt();
	if (count > 0)
	{
		QMessageBox::critical(parent, tr("Remove a category"), tr("This category contains snipet or other category, you can't remove this category."));
		return;
	}

	QSqlQuery titleQuery("select name from categories where id=:id", database());
	titleQuery.bindValue(":id", id);

	result = titleQuery.exec();
	Q_ASSERT(result);

	result = titleQuery.next();
	Q_ASSERT(result);

	int r = QMessageBox::question(parent, tr("Remove a category"), tr("Are you sure to delete the category \"%1\"").arg(titleQuery.value(0).toString()), QMessageBox::Yes | QMessageBox::No);

	if (r == QMessageBox::Yes)
	{
		QSqlQuery removeQuery("DELETE FROM categories WHERE id=:id", database());
		removeQuery.bindValue(":id", id);
		result = removeQuery.exec();

		Q_ASSERT(result);
	}
}

void SnipetManager::modifyCategory(int id, QWidget * parent)
{
	CategoryPropertyDlgImpl dlg(id, database(), parent);
	dlg.exec();
}

void SnipetManager::addSnipet(int parentId, QWidget * parent)
{
	SnipetPropertyDlgImpl dlg(database(), parent);
	dlg.setParentId(parentId);
	dlg.exec();
}

void SnipetManager::modifySnipet(int id, QWidget * parent)
{
	SnipetPropertyDlgImpl dlg(id, database(), parent);
	dlg.exec();
}

void SnipetManager::duplicateSnipet(int id, QWidget * parent)
{
	SnipetPropertyDlgImpl dlg(database(), parent);
	dlg.duplicate(id);
	dlg.exec();
}

bool SnipetManager::removeSnipet(int id, QWidget * parent)
{
	QSqlQuery removeSnipetQuery(
	    "DELETE FROM snipets "
	    "WHERE id=:id", database());
	removeSnipetQuery.bindValue(":id", id);
	QSqlQuery removeSnipetParamsQuery(
	    "DELETE FROM snipets_params "
	    "WHERE snipet_id=:id", database());
	removeSnipetParamsQuery.bindValue(":id", id);
	QSqlQuery removeSnipetExtQuery(
	    "DELETE FROM snipets_extentions "
	    "WHERE snipet_id=:id", database());
	removeSnipetExtQuery.bindValue(":id", id);

	if (! removeSnipetQuery.exec())
	{
		qWarning(qPrintable(removeSnipetQuery.lastError().text()));
		return false;
	}
	if (! removeSnipetParamsQuery.exec())
	{
		qWarning(qPrintable(removeSnipetQuery.lastError().text()));
		return false;
	}
	if (! removeSnipetExtQuery.exec())
	{
		qWarning(qPrintable(removeSnipetQuery.lastError().text()));
		return false;
	}
	return true;
}

QList<int> SnipetManager::snipets() const
{
	QList<int> ids;
	QSqlQuery selectQuery("SELECT id FROM snipets", database());

	bool result = selectQuery.exec();
	Q_ASSERT(result);
	if (! result)
	{
		qWarning(qPrintable(selectQuery.lastError().text()));
		return ids;
	}

	while (selectQuery.next())
	{
		ids += selectQuery.value(0).toInt();
	}
	return ids;
}

//! TODO
QList<int> SnipetManager::snipets(int categoryId) const
{
	return QList<int>();
}

bool SnipetManager::exportSnipetList(const QList<int> & list, SnipetList * snipets, QWidget * parent)
{
	QSqlQuery selectQuery("SELECT name, description, shortcut, icon, auto, show_dialog, text, available_script, category_id FROM snipets WHERE id=:id", database());
	QSqlQuery paramsQuery("SELECT name, default_value FROM snipets_params WHERE snipet_id=:id", database());
	QSqlQuery extentionsQuery("SELECT extention FROM snipets_extentions WHERE snipet_id=:id", database());
	foreach(int id, list)
	{
		Snipet snipet;
		selectQuery.bindValue(":id", id);
		paramsQuery.bindValue(":id", id);
		extentionsQuery.bindValue(":id", id);

		bool result = selectQuery.exec();
		Q_ASSERT(result);
		if (! result)
		{
			qWarning(qPrintable(selectQuery.lastError().text()));
			return false;
		}

		result = selectQuery.next();
		Q_ASSERT(result);

		snipet.setName(selectQuery.value(0).toString());
		snipet.setDescription(selectQuery.value(1).toString());
		snipet.setKey(selectQuery.value(2).toString());
		snipet.setIcon(selectQuery.value(3).toString());
		snipet.setCallIsAutomatic(selectQuery.value(4).toBool());
		snipet.setShowDialog(selectQuery.value(5).toBool());
		snipet.setText(selectQuery.value(6).toString());
		snipet.setAvailableScript(selectQuery.value(7).toString());

		int categoryId =           selectQuery.value(8).toInt();
		snipet.categories()    =   getCategoryName(categoryId);

		result = extentionsQuery.exec();
		Q_ASSERT(result);
		if (! result)
		{
			qWarning(qPrintable(extentionsQuery.lastError().text()));
			return false;
		}

		while (extentionsQuery.next())
		{
			snipet.extentions() += extentionsQuery.value(0).toString();
		}

		result = paramsQuery.exec();
		Q_ASSERT(result);
		if (! result)
		{
			qWarning(qPrintable(paramsQuery.lastError().text()));
			return false;
		}

		while (paramsQuery.next())
		{
			Snipet::Parameter p;
			p.name = paramsQuery.value(0).toString();
			p.defaultValue = paramsQuery.value(1).toString();
			snipet.params() += p;
		}

		*snipets << snipet;
	}
	return true;
}

bool SnipetManager::importSnipetList(const SnipetList & list, bool imported, QWidget * parent)
{
	QSqlQuery insertSnipetQuery("INSERT INTO snipets(name, description, shortcut, icon, auto, show_dialog, text, available_script, category_id) "
	                            "VALUES(:name, :description, :shortcut, :icon, :auto, :dialog, :text, :available_script, :category_id)", database());
	QSqlQuery insertSnipetExtentionsQuery("INSERT INTO snipets_extentions(snipet_id, extention) VALUES (:snipet_id, :extention)", database());
	QSqlQuery insertSnipetParamsQuery("INSERT INTO snipets_params(snipet_id, name, default_value) VALUES (:snipet_id, :name, :default_value)", database());

	foreach(const Snipet & s, list)
	{
		QStringList categories;
		if (imported)
			categories << tr("Imported Snipets");
		categories << s.categories();
		int categoryId = getCategoryId(categories);

		insertSnipetQuery.bindValue(":name", s.name());
		insertSnipetQuery.bindValue(":description", s.description());
		insertSnipetQuery.bindValue(":shortcut", s.key());
		insertSnipetQuery.bindValue(":icon", s.icon());
		insertSnipetQuery.bindValue(":auto", s.callIsAutomatic());
		insertSnipetQuery.bindValue(":dialog", s.showDialog());
		insertSnipetQuery.bindValue(":text", s.text());
		insertSnipetQuery.bindValue(":available_script", s.availableScript());
		insertSnipetQuery.bindValue(":category_id", categoryId);

		if (! insertSnipetQuery.exec())
		{
			qWarning(qPrintable(insertSnipetQuery.lastError().text()));
			return false;
		}

		int snipetId = insertSnipetQuery.lastInsertId().toInt();

		insertSnipetExtentionsQuery.bindValue(":snipet_id", snipetId);
		insertSnipetParamsQuery.bindValue(":snipet_id", snipetId);

		foreach(const QString & ext, s.extentions())
		{
			insertSnipetExtentionsQuery.bindValue(":extention", ext);
			if (! insertSnipetExtentionsQuery.exec())
			{
				qWarning(qPrintable(insertSnipetExtentionsQuery.lastError().text()));
				return false;
			}
		}

		foreach(const Snipet::Parameter & param, s.params())
		{
			insertSnipetParamsQuery.bindValue(":name", param.name);
			insertSnipetParamsQuery.bindValue(":default_value", param.defaultValue);
			if (! insertSnipetParamsQuery.exec())
			{
				qWarning(qPrintable(insertSnipetParamsQuery.lastError().text()));
				return false;
			}
		}
	}

	return true;
}

bool SnipetManager::callSnipet(int id, QString * result, QWidget * parent)
{
	Q_ASSERT(result);

	CallSnipetDialogImpl dlg(database(), id, parent);
	if (dlg.exec() != QDialog::Accepted) return false;
	if (! executeSnipetScript(dlg.snipetText(), dlg.values(), result)) return false;
	return true;
}

bool SnipetManager::callSnipet(QString key, QString * result, const QString & filename, QWidget * parent)
{
	Q_ASSERT(result);

	QSqlQuery searchId("select id, available_script from snipets where lower(shortcut) = lower(:shortcut)", database());
	searchId.bindValue(":shortcut", key);

	if (! searchId.exec())
	{
		qWarning(qPrintable(tr("Can't search id for key '%1' : %2").arg(key).arg(searchId.lastError().text())));
		return false;
	}

	while (searchId.next())
	{
		int snipet_id  = searchId.value(0).toInt();
		QString script = searchId.value(1).toString();
		if (isAvailable(script, "SNIPET", snipet_id) && (filename.isEmpty() || isSnipetMatch(filename, snipet_id)))
		{
			return callSnipet(snipet_id, result, parent);
		}
	}

	return false;
}

bool SnipetManager::callAutomaticSnipet(QString key, QString * result, const QString & filename, QWidget * parent)
{
	QSqlQuery searchId("select id, available_script from snipets where lower(shortcut) = lower(:shortcut) and auto>=:auto", database());
	searchId.bindValue(":key", key);
	searchId.bindValue(":auto", true);

	if (! searchId.exec())
	{
		qWarning(qPrintable(tr("Can't search id for key '%1' : %2").arg(key).arg(searchId.lastError().text())));
		return false;
	}

	while (searchId.next())
	{
		int snipet_id  = searchId.value(0).toInt();
		QString script = searchId.value(1).toString();
		if (isAvailable(script, "SNIPET", snipet_id) && (filename.isEmpty() || isSnipetMatch(filename, snipet_id)))
		{
			return callSnipet(snipet_id, result, parent);
		}
	}

	return false;
}


bool SnipetManager::executeSnipetScript(const QString & script, const QStringList & values, QString * result) const
{
	/* Process arguments */
	QString text = script;
	for (int i = 0 ; i < values.size() ; i++)
	{
		text = text.arg(values.at(i));
	}

	/* Process script */
	QScriptEngine & engine = ScriptManager::self()->engine();

	engine.pushContext();
	QRegExp jsString("<\\?[^x][^m][^l].*\\?>");
	jsString.setMinimal(true);
	int from = 0;
	QString processedString;
	while (jsString.indexIn(text, from) >= 0)
	{
		processedString += text.mid(from, jsString.pos() - from);

		QString js = text.mid(jsString.pos() + 2, jsString.matchedLength() - 4);
		if (js.at(0) == '=')
		{
			QScriptValue result = engine.evaluate(js.mid(1));
			if (! result.isError())
				processedString += result.toString();
			else
			{
				qCritical(qPrintable(result.toString()));
				engine.popContext();
				return false;
			}
		}
		else
		{
			engine.evaluate(js);   // Only for define variable or make pre-process.
			if (engine.hasUncaughtException())
			{
				qCritical(qPrintable(engine.uncaughtException().toString()));
				engine.popContext();
				return false;
			}
		}
		from = jsString.pos() + jsString.matchedLength();
	}
	processedString += text.mid(from);

	engine.popContext();
	*result = processedString;
	return true;
}

bool SnipetManager::isAvailable(const QString & script, const QString & type, int id) const
{
	if (! script.isEmpty())
	{
		QScriptEngine & engine = ScriptManager::self()->engine();

		engine.pushContext();
		QScriptValue result = engine.evaluate(script);
		if (result.isError())
		{
			qWarning() << tr("Error when calling script for record %1 : %2").arg(id).arg(result.toString());
		}
		else
		{
#if QT_VERSION > 0x040500
			if (! result.isBool())
			{
				qWarning() << tr("The script %1 %2 return neither true or false.\n%3").arg(type).arg(id).arg(result.toString());
			}
			else
			{
				if (! result.toBool())
				{
					engine.popContext();
					return false;
				}
			}
#else
			if (! result.isBoolean())
			{
				qWarning() << tr("The script %1 %2 return neither true or false.\n%3").arg(type).arg(id).arg(result.toString());
			}
			else
			{
				if (! result.toBoolean())
				{
					engine.popContext();
					return false;
				}
			}
#endif
		}
		engine.popContext();
	}
	return true;
}

bool SnipetManager::isSnipetMatch(const QString & filename, int snipetId) const
{
	// Check extentions to know if the snipet can be used with this editor
	QSqlQuery extentionsQuery("SELECT DISTINCT extention FROM snipets_extentions WHERE snipet_id=:id", database());
	extentionsQuery.bindValue(":id", snipetId);
	if (! extentionsQuery.exec())
	{
		qWarning() << "Can't lookup extentions for snipet " << snipetId;
		return true;
	}

	// If no extention selected, all can be used
	if (! extentionsQuery.next())
	{
		return true;
	}

	do
	{
		QRegExp regExp(extentionsQuery.value(0).toString(), Qt::CaseInsensitive, QRegExp::Wildcard);
		if (regExp.exactMatch(filename))
			return true;
	}
	while (extentionsQuery.next());

	return false;
}

bool SnipetManager::openDatabase() const
{
	// Create the db object
	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "SNIPETBASE");
	QFileInfo fileInfo("datas:snipets.db");
	QString databaseFileName = fileInfo.absoluteFilePath();
	if (! fileInfo.exists())
	{
		databaseFileName = QDir(QDir::searchPaths("datas").at(0)).absoluteFilePath("snipets.db");
	}


	db.setDatabaseName(databaseFileName);
	if (! db.open())
	{
		qWarning(qPrintable(tr("Can't load snipet database : %1").arg(db.lastError().text())));
		return false;
	}

	// Check the content
	QStringList tables = db.tables();
	if (! tables.contains("snipets"))
		return createDatabase(db);

	return true;
}

bool SnipetManager::createDatabase(QSqlDatabase db) const
{
	QSqlQuery createQuery(db);
	/* Create tables */
	if (!
	        createQuery.exec("CREATE TABLE snipet_config ("
	                         "    name TEXT PRIMARY KEY,"
	                         "    value TEXT)"))
	{
		qWarning(qPrintable(createQuery.lastError().text()));
		return false;
	}
	if (!
	        createQuery.exec("CREATE TABLE categories ("
	                         "    id INTEGER PRIMARY KEY,"
	                         "    parent_id INTEGER NOT NULL DEFAULT(0),"
	                         "    name TEXT NOT NULL,"
	                         "    description TEXT,"
	                         "    available_script TEXT)"))
	{
		qWarning(qPrintable(createQuery.lastError().text()));
		return false;
	}
	if (!
	        createQuery.exec("CREATE TABLE snipets ("
	                         "    id INTEGER PRIMARY KEY,"
	                         "    name TEXT,"
	                         "    description TEXT,"
	                         "    shortcut TEXT,"
	                         "    icon TEXT,"
	                         "    auto INTEGER,"
	                         "    show_dialog INTEGER,"
	                         "    text TEXT,"
	                         "    available_script TEXT,"
	                         "    category_id INTEGER  NOT NULL DEFAULT (0),"
	                         "    FOREIGN KEY(category_id) REFERENCES category)"))
	{
		qWarning(qPrintable(createQuery.lastError().text()));
		return false;
	}
	if (!
	        createQuery.exec("CREATE TABLE snipets_extentions ("
	                         "    id INTEGER PRIMARY KEY,"
	                         "    snipet_id INTEGER NOT NULL,"
	                         "    extention TEXT,"
	                         "    FOREIGN KEY(snipet_id) REFERENCES snipet)"))
	{
		qWarning(qPrintable(createQuery.lastError().text()));
		return false;
	}
	if (!
	        createQuery.exec("CREATE TABLE snipets_params ("
	                         "    id INTEGER PRIMARY KEY,"
	                         "    snipet_id INTEGER NOT NULL,"
	                         "    name TEXT,"
	                         "    default_value TEXT,"
	                         "    FOREIGN KEY(snipet_id) REFERENCES snipet)"))
	{
		qWarning(qPrintable(createQuery.lastError().text()));
		return false;
	}
	/* Create Index */
	if (!
	        createQuery.exec("CREATE INDEX categories_idx1 on categories (id ASC)"))
	{
		qWarning(qPrintable(createQuery.lastError().text()));
		return false;
	}
	if (!
	        createQuery.exec("CREATE INDEX categories_idx2 on categories (parent_id ASC, name ASC)"))
	{
		qWarning(qPrintable(createQuery.lastError().text()));
		return false;
	}
	if (!
	        createQuery.exec("CREATE INDEX snipets_idx1 on snipets (id ASC)"))
	{
		qWarning(qPrintable(createQuery.lastError().text()));
		return false;
	}
	if (!
	        createQuery.exec("CREATE INDEX snipets_idx2 on snipets (name ASC, description ASC, shortcut ASC)"))
	{
		qWarning(qPrintable(createQuery.lastError().text()));
		return false;
	}
	if (!
	        createQuery.exec("CREATE INDEX snipets_idx3 on snipets (name ASC, category_id ASC)"))
	{
		qWarning(qPrintable(createQuery.lastError().text()));
		return false;
	}
	if (!
	        createQuery.exec("CREATE INDEX snipets_idx5 on snipets (shortcut ASC)"))
	{
		qWarning(qPrintable(createQuery.lastError().text()));
		return false;
	}
	if (!
	        createQuery.exec("CREATE INDEX snipets_idx6 on snipets (shortcut ASC, auto ASC)"))
	{
		qWarning(qPrintable(createQuery.lastError().text()));
		return false;
	}
	if (!
	        createQuery.exec("CREATE INDEX snipets_extentions_idx1 on snipets_extentions (snipet_id ASC)"))
	{
		qWarning(qPrintable(createQuery.lastError().text()));
		return false;
	}
	if (!
	        createQuery.exec("CREATE INDEX snipets_params_idx1 on snipets_params (snipet_id ASC)"))
	{
		qWarning(qPrintable(createQuery.lastError().text()));
		return false;
	}
	/* DATAS */
	if (!
	        createQuery.exec("INSERT INTO snipet_config(name, value) VALUES( 'version', '1.0' )"))
	{
		qWarning(qPrintable(createQuery.lastError().text()));
		return false;
	}
	if (!
	        createQuery.exec(QString("INSERT INTO categories(parent_id, name, description) VALUES( 0, '%1', '%2' )").arg(tr("Categories")).arg(tr("List of all categories"))))
	{
		qWarning(qPrintable(createQuery.lastError().text()));
		return false;
	}
	return true;
}

void SnipetManager::closeDatabase() const
{
	database().close();
	QSqlDatabase::removeDatabase("SNIPETBASE");
}

