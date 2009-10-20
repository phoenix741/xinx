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
#include "editorfactory.h"
#include "core/xinxcore.h"
#include "plugins/xinxpluginsloader.h"
#include "editors/textfileeditor.h"
#include "editors/xinxcodeedit.h"

/* Static member */

EditorFactory * EditorFactory::s_self = 0;


/* EditorFactory */

EditorFactory::EditorFactory() {
}

EditorFactory::~EditorFactory() {
	if( s_self == this )
		s_self = NULL;
}

EditorFactory * EditorFactory::self() {
	if( s_self == 0 ) {
		s_self = new EditorFactory();
		XINXStaticDeleter::self()->addObject( s_self );
	}
	return s_self;
}

AbstractEditor * EditorFactory::createEditor( IFileTypePlugin * interface ) {
	AbstractEditor * editor = interface ? interface->createEditor() : new TextFileEditor( new XinxCodeEdit() );

	editor->initLayout();

	Q_ASSERT_X( editor, "EditorFactory::createEditor", "The interface can't create editor" );

	return editor;
}

AbstractEditor * EditorFactory::createEditor( const QString & filename ) {
	Q_ASSERT( ! filename.isEmpty() );

	AbstractEditor * editor = 0;
	IFileTypePlugin * interface = XinxPluginsLoader::self()->matchedFileType( filename );
	if( interface ) {
		editor = interface->createEditor( filename );
	} else {
		editor = new TextFileEditor( new XinxCodeEdit() );
		editor->loadFromFile( filename );
	}

	Q_ASSERT_X( editor, "EditorFactory::createEditor", "The factory can't create editor" );

	editor->initLayout();

	return editor;
}

AbstractEditor * EditorFactory::createEditor( XinxProjectSessionEditor * session ) {
	Q_ASSERT( session );

	AbstractEditor * editor = AbstractEditor::deserializeEditor( session );

	Q_ASSERT_X( editor, "EditorFactory::createEditor", "The factory can't deserialize the editor" );

	editor->initLayout();

	return editor;
}
