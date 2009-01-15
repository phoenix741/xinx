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

#ifndef _ITEMMODELFILEEDITOR_H_
#define _ITEMMODELFILEEDITOR_H_

// Xinx header
#include "textfileeditor.h"

// Qt header
#include <QPointer>

/*!
 * An \e ItemModelFileEditor load and unload the content model editor when
 * reading the device. The content model is based on a FileContentParser and
 * some FileContentElement.
 */
class ItemModelFileEditor : public TextFileEditor {
	Q_OBJECT
public:
	/*!
	 * Create the \e ItemModelFileEditor. This editor add the notion of model.
	 * \param element The parser to use to show the content of the document.
	 * \param editor The editor (center widget) of the document.
	 * \param parent The parent object
	 */
	ItemModelFileEditor( FileContentParser * element, XinxCodeEdit * editor = 0, QWidget *parent = 0 );
	//! Destroy the object
	virtual ~ItemModelFileEditor();

	virtual void loadFromFile( const QString & fileName = QString() );
	virtual void saveToFile( const QString & fileName = QString() );
	virtual void loadFromDevice( QIODevice & d );

	//! The model editor is really created (based on the parser) at the first call.
	virtual QAbstractItemModel * model() const;
public slots:
	virtual void updateModel();

protected:
	//! Change the parser of the model file editor.
	void setParser( FileContentParser * parser );

private slots:
	virtual void textChanged();

private:
	mutable QPointer<FileContentItemModel> m_model;
	FileContentParser * m_parser;

	QTimer * m_keyTimer;
};

#endif // _ITEMMODELFILEEDITOR_H_
