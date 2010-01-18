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

#ifndef SNIPETCOMPLETIONNODEMODEL_H
#define SNIPETCOMPLETIONNODEMODEL_H

// Xinx header
#include <core/lib-config.h>
#include <contentview/completionnodemodel.h>

/* SnipetCompletionNodeModel */

class LIBEXPORT SnipetCompletionNodeModel : public CompletionNodeModel {
	Q_OBJECT
public:
	SnipetCompletionNodeModel( ContentViewNode * root, QObject *parent = 0 );
	virtual ~SnipetCompletionNodeModel();

protected:
	virtual void timerEvent( QTimerEvent * event );
};

#endif // SNIPETCOMPLETIONNODEMODEL_H
