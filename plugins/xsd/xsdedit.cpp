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
#include "xsdedit.h"
#include "xsdeditor.h"

// Qt header
#include <QString>
#include <QVariant>
#include <QApplication>

/* XSLStyleSheetFileType */

class XSDFileType : public QObject, public IFileTypePlugin
{
public:
	virtual QString description()
	{
		return tr("XML Schema Description");
	};
	virtual QString match()
	{
		return "*.xsd";
	};
	virtual QIcon icon()
	{
		return QIcon(":/xsd/images/typexsd.png");
	};

	virtual AppSettings::struct_extentions properties()
	{
		AppSettings::struct_extentions p;
		p.canBeCommitToRcs = true;
		p.canBeFindInConfiguration = false;
		p.canBeSaveAsSpecifique = false;
		p.specifiqueSubDirectory = "";
		return p;
	};

	virtual AbstractEditor * createEditor(const QString & filename)
	{
		XsdEditor * editor = new XsdEditor();
		if (! filename.isEmpty())
			editor->loadFromFile(filename);
		return editor;
	}
	virtual FileContentElement * createElement(FileContentElement * parent, int line, const QString & filename)
	{
		return NULL;
	}

	virtual ContentViewParser * createParser()
	{
		return 0;
	}
};

/* XsdEditPlugin */

XsdEditPlugin::XsdEditPlugin()
{
	Q_INIT_RESOURCE(xsdedit);

	m_fileTypes << new XSDFileType;
}

XsdEditPlugin::~XsdEditPlugin()
{
}

bool XsdEditPlugin::initializePlugin(const QString & lang)
{
	return true;
}

QVariant XsdEditPlugin::getPluginAttribute(const enum IXinxPlugin::PluginAttribute & attr)
{
	switch (attr)
	{
	case PLG_NAME:
		return tr("User Interface for editing XML Schema");
	case PLG_DESCRIPTION:
		return tr("EXPERIMENTAL / Editor for visualise and edit XML Schema");
	case PLG_AUTHOR:
		return "Ulrich Van Den Hekke";
	case PLG_ICON:
		return QPixmap(":/xsd/images/xsdedit.png");
	case PLG_EMAIL:
		return "ulrich.vdh@shadoware.org";
	case PLG_WEBSITE:
		return "http://www.shadoware.org";
	case PLG_VERSION:
		return "0.1";
	case PLG_LICENCE:
		return "GPL v2.0 or later";
	}
	return QVariant();
}

QList<IFileTypePlugin*> XsdEditPlugin::fileTypes()
{
	return m_fileTypes;
}

Q_EXPORT_PLUGIN2(xslgui, XsdEditPlugin)
