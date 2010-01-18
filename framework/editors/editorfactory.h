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

#ifndef EDITORFACTORY_H
#define EDITORFACTORY_H

// Xinx header
#include <core/lib-config.h>

// Qt header
#include <QObject>

class AbstractEditor;
class IFileTypePlugin;
class XinxProjectSessionEditor;

class LIBEXPORT EditorFactory : public QObject {
	Q_OBJECT
public:
	virtual ~EditorFactory();

	AbstractEditor * createEditor( IFileTypePlugin * interface = 0 );
	AbstractEditor * createEditor( const QString & filename );
	AbstractEditor * createEditor( XinxProjectSessionEditor * session = 0 );

	static EditorFactory * self();
private:
	EditorFactory();

	static EditorFactory * s_self;
};

#endif // EDITORFACTORY_H
