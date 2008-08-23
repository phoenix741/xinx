/***************************************************************************
 *   Copyright (C) 2008 by Ulrich Van Den Hekke                            *
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

#ifndef _WEBSERVICESFILETYPE_H_
#define _WEBSERVICESFILETYPE_H_

// Xinx header
#include "webserviceseditor.h"

/* WebServicesFileType */

class WebServicesFileType : public QObject, public IFileTypePlugin {
	Q_OBJECT
public:
	virtual QString description() {	return tr( "Web Services Stream" ); };
	virtual QString match() { return "*.fws"; };
	virtual QIcon icon() { return QIcon( ":/services/images/typefws.png" ); };

	virtual struct_properties properties() {
		struct_properties p;
		p.canBeCommitToRCS = true;
		p.canBeFindInConfiguration = false;
		p.canBeSaveAsSpecifique = false;
		p.specifiqueSubDirectory = QString();
		return p;
	};

	virtual AbstractEditor * createEditor( const QString & filename ) {
		WebServicesEditor * editor = new WebServicesEditor();

		if( ! filename.isEmpty() )
			editor->loadFromFile( filename );

		return editor;
	}
	virtual FileContentElement * createElement( FileContentElement * parent, int line, const QString & filename ) {
		Q_UNUSED( parent );
		Q_UNUSED( line );
		Q_UNUSED( filename );
		return 0;
	}
};

#endif //  _WEBSERVICESFILETYPE_H_