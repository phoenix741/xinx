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

#ifndef __FILETYPEINTERFACE_H__
#define __FILETYPEINTERFACE_H__

// Qt header
#include <QObject>

class TextEditor;
class PrivateFileTypeInterface;
class QAbstractItemModel;

class FileTypeInterface : public QObject {
	Q_OBJECT
public:
	FileTypeInterface( TextEditor * parent );
	virtual ~FileTypeInterface();
	
	virtual void commentSelectedText( bool uncomment ) = 0;
	
	virtual void updateModel() = 0;
	virtual QAbstractItemModel * model() = 0;
	
	virtual void complete() = 0;
signals:
	void modelUpdated( QAbstractItemModel * model );
protected:
	TextEditor * textEdit();
private:
	PrivateFileTypeInterface * d;
	friend class PrivateFileTypeInterface;
};

#endif // __FILETYPEINTERFACE_H__
