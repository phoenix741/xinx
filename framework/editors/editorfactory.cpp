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
#include "editorfactory.h"
#include "core/xinxcore.h"
#include "plugins/xinxpluginsloader.h"
#include "editors/textfileeditor.h"
#include "editors/xinxcodeedit.h"
#include "editors/editorchoicedlg.h"

// Qt header
#include <QApplication>

/* Static member */

EditorFactory * EditorFactory::s_self = 0;

/* EditorFactory */

EditorFactory::EditorFactory()
{

}

EditorFactory::~EditorFactory()
{
	if (s_self == this)
		s_self = NULL;
}

EditorFactory * EditorFactory::self()
{
	if (s_self == 0)
	{
		s_self = new EditorFactory();
		XINXStaticDeleter::self()->addObject(s_self);
	}
	return s_self;
}

AbstractEditor * EditorFactory::createEditor(IFileTypePlugin * interface)
{
	AbstractEditor * editor = interface ? interface->createEditor() : new TextFileEditor(new XinxCodeEdit());

	editor->initLayout();
	if (qobject_cast<TextFileEditor*>(editor))
		qobject_cast<TextFileEditor*>(editor)->initCompleter();

	Q_ASSERT_X(editor, "EditorFactory::createEditor", "The interface can't create editor");

	return editor;
}

AbstractEditor * EditorFactory::createEditor(const QString & filename)
{
	Q_ASSERT(! filename.isEmpty());

	AbstractEditor * editor = 0;
	QList<IFileTypePlugin *> interfaces = XinxPluginsLoader::self()->matchedFileType(filename);
	switch(interfaces.size())
	{
	case 0:
		{
			EditorChoiceDlg dlg(qApp->activeWindow());
			dlg.setFileName(filename);
			dlg.setFileTypes(XinxPluginsLoader::self()->fileTypes());
			if (dlg.exec() == QDialog::Accepted)
			{
				Q_ASSERT_X(dlg.selectedType(), "EditorFactory::createEditor", "No editor selected");
				editor = dlg.selectedType()->createEditor(filename);
			}
		}
		break;
	case 1:
		editor = interfaces.at(0)->createEditor(filename);
		break;
	default:
		{
			EditorChoiceDlg dlg(qApp->activeWindow());
			dlg.setFileName(filename);
			dlg.setFileTypes(interfaces);
			if (dlg.exec() == QDialog::Accepted)
			{
				Q_ASSERT_X(dlg.selectedType(), "EditorFactory::createEditor", "No editor selected");
				editor = dlg.selectedType()->createEditor(filename);
			}
		}
	}

	Q_ASSERT_X(editor, "EditorFactory::createEditor", "The factory can't create editor");

	editor->initLayout();
	if (qobject_cast<TextFileEditor*>(editor))
		qobject_cast<TextFileEditor*>(editor)->initCompleter();

	return editor;
}

AbstractEditor * EditorFactory::createEditor(XinxProjectSessionEditor * session)
{
	Q_ASSERT(session);

	AbstractEditor * editor = AbstractEditor::deserializeEditor(session);

	Q_ASSERT_X(editor, "EditorFactory::createEditor", "The factory can't deserialize the editor");

	editor->initLayout();
	if (qobject_cast<TextFileEditor*>(editor))
		qobject_cast<TextFileEditor*>(editor)->initCompleter();

	return editor;
}

