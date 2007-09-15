/***************************************************************************
 *   Copyright (C) 2007 by Ulrich Van Den Hekke                            *
 *   ulrich.vdh@free.fr                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef _P_FILEEDITOR_H_
#define _P_FILEEDITOR_H_

// Xinx header
#include "../fileeditor.h"
#include "../filewatcher.h"
#include "../filetypeinterface.h"
#include "../filetypejs.h"
#include "../filetypexml.h"
#include "../filetypexsl.h"

// Qt header
#include <QObject>
#include <QAction>
#include <QSyntaxHighlighter>

class PrivateFileEditor : public QObject {
	Q_OBJECT
public:
	PrivateFileEditor( FileEditor * parent );
	~PrivateFileEditor();
	
	QSyntaxHighlighter * m_syntaxhighlighter;
	FileEditor::enumHighlighter m_highlighterType;
	
	FileTypeInterface * m_interface;
	FileEditor::enumFileType m_fileType;

	QAction * m_commentAction;
	QAction * m_uncommentAction;
	FileWatcher * m_watcher;
	QString m_path;

	bool m_isSaving;

	bool hasWatcher(); 
	void setWatcher( const QString & path );
	void activateWatcher();
	void desactivateWatcher();
public slots:
	void fileChanged ();

	void comment();
	void uncomment();
private:
	FileEditor * m_parent;
};

#endif /* _P_FILEEDITOR_H_ */
