/*
 XINX
 Copyright (C) 2007-2010 by Ulrich Van Den Hekke
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

#include "dictionarymodel.h"
#include <core/errormanager.h>
#include <editors/editormanager.h>
#include <editors/abstracteditor.h>
#include <configuration/configurationmanager.h>
#include <contentview3/filenode.h>
#include "dictionary_labelnode.h"
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QFile>

/* DictionaryModel */


DictionaryModel::DictionaryModel(QObject* parent): QStandardItemModel(parent)
{
	connect(EditorManager::self(), SIGNAL(currentChanged(int)), this, SLOT(textEditorChanged(int)));
}

DictionaryModel::~DictionaryModel()
{

}

XinxProject::ProjectPtr DictionaryModel::getProject() const
{
	return _project.toStrongRef();
}

void DictionaryModel::textEditorChanged(int index)
{
	XinxProject::ProjectPtr project;
	if ((index >= 0) && (index < EditorManager::self()->editorsCount()))
	{
		project = EditorManager::self()->editor(index)->project();
	}

	if (project.data() != _project.data())
	{
		_project = project.toWeakRef();
		setConfigurationManager(0);
		reload();
	}
}

void DictionaryModel::setConfigurationManager(ConfigurationManager* m)
{
	if (_currentConfigurationManager.data() != m)
	{
		if (! _currentConfigurationManager.isNull())
		{
			disconnect(_currentConfigurationManager, SIGNAL(dictionaryChanged()), this, SLOT(reload()));
		}

		_currentConfigurationManager = m;

		if (! _currentConfigurationManager.isNull())
		{
			connect(_currentConfigurationManager, SIGNAL(dictionaryChanged()), this, SLOT(reload()));
		}
	}
}


void DictionaryModel::reload()
{
	loadDictionaries(_prefix);
}


const QStringList & DictionaryModel::dictionaries() const
{
	return _dictionaries;
}

/**
 * Add the given label at the end of the file. For this the file is opened as a DomDocument and next appended, and finally saved.
 */
bool DictionaryModel::changeLabel(const QString & dictionary, const QString & label, const QString & lang, const QString & context, const QString & content)
{
	/* Open the file */
	QFile dictionaryFile(dictionary);
	if (! dictionaryFile.open(QIODevice::ReadWrite | QIODevice::Text))
	{
		ErrorManager::self()->addMessage(dictionary, -1, QtCriticalMsg, tr("Can't open the dictionary for write : %1").arg(dictionaryFile.errorString()));
		return false;
	}

	QByteArray dictionaryByteArray;
	QXmlStreamReader reader(&dictionaryFile);
	QXmlStreamWriter writer(&dictionaryByteArray);
	writer.setAutoFormatting(true);
	writer.setAutoFormattingIndent(-1); /* One tab */

	bool updated = false;
	QString currentLabelsCode;
	while(! reader.atEnd())
	{
		QXmlStreamReader::TokenType token = reader.readNext();
		switch(token)
		{
		case QXmlStreamReader::Characters:
			/* Ignore characters, Writer made indentation */
			break;
		case QXmlStreamReader::StartElement:
			if (!updated)
			{
				const QStringRef baliseName = reader.qualifiedName();
				if (baliseName == "labels") /* On recherche un labels */
				{
					currentLabelsCode = reader.attributes().value("code").toString();
					writer.writeCurrentToken(reader);
				}
				else if ((baliseName == "label") && (currentLabelsCode == label)) /* On recherche un label */
				{
					const QStringRef currentContext = reader.attributes().value("ctx");
					const QStringRef currentLang = reader.attributes().value("lang");

					if ((currentContext == context) && (currentLang == lang))
					{
						writer.writeStartElement("label");
						writer.writeAttribute("lang", lang);
						writer.writeAttribute("ctx", context);
						writer.writeAttribute("value", content);
						updated = true;
					}
					else
					{
						writer.writeCurrentToken(reader);
					}
				}
				else
				{
					writer.writeCurrentToken(reader);
				}
			}
			else
			{
				writer.writeCurrentToken(reader);
			}
			break;
		case QXmlStreamReader::EndElement:
			if (! updated)
			{
				const QStringRef baliseName = reader.qualifiedName();
				if ((baliseName == "labels") && (currentLabelsCode == label))
				{
					writer.writeStartElement("label");
					writer.writeAttribute("lang", lang);
					writer.writeAttribute("ctx", context);
					writer.writeAttribute("value", content);
					writer.writeEndElement();
					updated = true;
				}
				else if (baliseName == "root")
				{
					writer.writeStartElement("labels");
					writer.writeAttribute("code", label);
					writer.writeStartElement("label");
					writer.writeAttribute("lang", lang);
					writer.writeAttribute("ctx", context);
					writer.writeAttribute("value", content);
					writer.writeEndElement();
					writer.writeEndElement();
				}
			}
			writer.writeCurrentToken(reader);
			break;
		case QXmlStreamReader::StartDocument:
			writer.setCodec(reader.documentEncoding().toLocal8Bit());
			writer.writeDefaultNamespace("http://www.generix.fr/technicalframework/trad");
			writer.writeCurrentToken(reader);
			break;
		default:
			writer.writeCurrentToken(reader);
		}
	}
	if (reader.hasError())
	{
		ErrorManager::self()->addMessage(dictionary, -1, QtCriticalMsg, tr("Can't parse the dictionary for write : %1").arg(reader.errorString()));
		return false;
	}

	dictionaryFile.resize(dictionaryByteArray.size());
	dictionaryFile.seek(0);
	dictionaryFile.write(dictionaryByteArray);

	return true;
}

void DictionaryModel::loadDictionaries(const QString & prefix)
{
	_prefix = prefix;

	XinxProject::ProjectPtr project = _project.toStrongRef();

	clear();
	_dictionaries.clear();

	emit changed();

	if (! project) return;

	setConfigurationManager(ConfigurationManager::manager(project));
	if (_currentConfigurationManager.isNull()) return;
	if (! _currentConfigurationManager->getInterface()) return;

	/* We load dictionary even if we don't show labels */
	_dictionaries = _currentConfigurationManager->getInterface()->dictionnaries();

	/* If there is no prefix defined, we don't show any labels (for performance reason */
	if (prefix.isEmpty())
	{
		appendRow(new QStandardItem(tr("Please type a prefix to list labels.")));

		emit changed();
		return;
	}

	QRegExp regExpPrefix(prefix);
	regExpPrefix.setCaseSensitivity(Qt::CaseInsensitive);

	QHash<QString,QStandardItem*> _items;
	foreach(const QString & dictionary, _dictionaries)
	{
		ContentView3::NodePtr nodePtr = project->cache()->cachedFile(dictionary)->rootNode();

		foreach(ContentView3::NodePtr childNodePtr, nodePtr->childs())
		{
			QStandardItem * label;
			bool forceChild = false;
			if (_items.contains(childNodePtr->name()))
			{
				label = _items.value(childNodePtr->name());
				forceChild = true;
			}
			else
			{
				if (! childNodePtr->name().contains(regExpPrefix))
				{
					bool skipLabel = true;
					foreach(ContentView3::NodePtr childChildPtr, childNodePtr->childs())
					{
						QSharedPointer<Generix::Dictionary::LabelNode> labelPtr = childChildPtr.dynamicCast<Generix::Dictionary::LabelNode>();
						if (labelPtr->value().contains(regExpPrefix))
						{
							skipLabel = false;
							break;
						}
					}
					if (skipLabel)
					{
						continue;
					}
				}

				forceChild = true;

				label = new QStandardItem;
				label->setText(childNodePtr->name());
				label->setIcon(QIcon(childNodePtr->icon()));

				appendRow(label);
				_items.insert(childNodePtr->name(), label);
			}

			QStandardItem * langue = 0;
			QStandardItem * context = 0;

			foreach(ContentView3::NodePtr childChildPtr, childNodePtr->childs())
			{
				QSharedPointer<Generix::Dictionary::LabelNode> labelPtr = childChildPtr.dynamicCast<Generix::Dictionary::LabelNode>();

				if (!labelPtr->value().contains(regExpPrefix) && !forceChild)
				{
					continue;
				}

				if ((langue == 0) || (langue->text() != labelPtr->lang()))
				{
					if (labelPtr->lang().isEmpty())
					{
						langue = label;
					}
					else
					{
						langue = new QStandardItem;
						langue->setText(labelPtr->lang());
						if (labelPtr->lang() == "FRA")
						{
							langue->setIcon(QIcon(":/generix/images/france.png"));
						}
						else if (labelPtr->lang() == "ENG")
						{
							langue->setIcon(QIcon(":/generix/images/usa.png"));
						}
						else if (labelPtr->lang() == "ESP")
						{
							langue->setIcon(QIcon(":/generix/images/spain.png"));
						}
						else if (labelPtr->lang() == "POR")
						{
							langue->setIcon(QIcon(":/generix/images/portugal.png"));
						}
						else if (labelPtr->lang() == "DEU")
						{
							langue->setIcon(QIcon(":/generix/images/germany.png"));
						}
						else if (labelPtr->lang() == "ITA")
						{
							langue->setIcon(QIcon(":/generix/images/italy.png"));
						}
						label->appendRow(langue);
					}
				}

				if ((context == 0) || (context->text() != labelPtr->context()))
				{
					if (labelPtr->context().isEmpty())
					{
						context = langue;
					}
					else
					{
						context = new QStandardItem;
						context->setText(labelPtr->context());
						context->setIcon(QIcon(":/generix/images/unknown.png"));
						langue->appendRow(context);
					}
				}

				QStandardItem * item = new QStandardItem;
				item->setText(labelPtr->name());
				item->setIcon(QIcon(labelPtr->icon()));
				item->setData(labelPtr->code(), ITEM_CODE);
				item->setData(labelPtr->lang(), ITEM_LANG);
				item->setData(labelPtr->context(), ITEM_CTX);
				item->setData(dictionary, ITEM_DICO);
				context->appendRow(item);
			}
		}
	}
	emit changed();
}
