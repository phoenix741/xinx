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

// Xinx header
#include "editorfactory.h"
#include "core/xinxcore.h"
#include "plugins/xinxpluginsloader.h"
#include "editors/textfileeditor.h"
#include "editors/xinxcodeedit.h"
#include "editors/editorchoicedlg.h"
#include "project/xinxprojectmanager.h"

// Qt header
#include <QApplication>

/* EditorFactory */

EditorFactory::EditorFactory()
{

}

EditorFactory::~EditorFactory()
{

}

IFileTypePlugin * EditorFactory::interfaceOfName(const QString & name)
{
	QList<IFileTypePlugin*> interfaces = XinxPluginsLoader::self()->fileTypes();
	foreach(IFileTypePlugin* interface, interfaces)
	{
		if (interface->name () == name)
		{
			return interface;
		}
	}
	return 0;
}

AbstractEditor * EditorFactory::createEditor(IFileTypePlugin * interface)
{
	Q_ASSERT_X(interface, "EditorFactory::createEditor", "Interface not defined");

	AbstractEditor * editor = interface ? interface->createEditor() : new TextFileEditor(new XinxCodeEdit());

	Q_ASSERT_X(editor, "EditorFactory::createEditor", "The factory can't create editor");

	editor->setFileTypePluginInterface(interface);

	editor->initLayout();

	return editor;
}

AbstractEditor * EditorFactory::createEditor(const QString& filename, IFileTypePlugin* interface, XinxProject::ProjectPtr project)
{
	Q_ASSERT(! filename.isEmpty());

	/* If no interface defined, we must ask for one */
	if (!interface)
	{
		EditorChoiceDlg dlg(qApp->activeWindow());
		dlg.setFileName(filename);
		if ((dlg.count () > 1) && (dlg.exec() != QDialog::Accepted))
		{
			/* The user cancel the operation */
			return NULL;
		}

		interface = dlg.selectedType();
		Q_ASSERT_X(interface, "EditorFactory::createEditor", "No editor selected");
	}

	/* If no project is defined we must find one */
	if (project.isNull())
	{
		project = XinxProject::Manager::self ()->projectOfFile(filename);
	}

	AbstractEditor * editor = createEditor(interface);
	editor->setProject (project);
	editor->loadFromFile (filename);

	return editor;
}

AbstractEditor * EditorFactory::createEditor(XinxSession::SessionEditor * session)
{
	Q_ASSERT(session);

	AbstractEditor * editor = AbstractEditor::deserializeEditor(session);

	Q_ASSERT_X(editor, "EditorFactory::createEditor", "The factory can't deserialize the editor");

	editor->initLayout();

	return editor;
}

