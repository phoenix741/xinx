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

#ifndef _CONTENTVIEWTEXTEDITOR_H_
#define _CONTENTVIEWTEXTEDITOR_H_

// Xinx header
#include <editors/textfileeditor.h>

// Qt header
#include <QPointer>

class ContentViewNode;
class ContentViewParser;
class ContentViewModel;
class QModelIndex;

/*!
 * An \e ContentViewTextEditor load and unload the content view model editor when
 * reading the device. The content model is based on a ContentViewParser and
 * some ContentViewNode.
 */
class ContentViewTextEditor : public TextFileEditor {
	Q_OBJECT
public:
	/*!
	 * Create the \e ContentViewTextEditor. This editor add the notion of model.
	 * \param element The parser to use to create tree.
	 * \param editor The editor (center widget) of the document.
	 * \param parent The parent object
	 */
	ContentViewTextEditor( ContentViewParser * parser, XinxCodeEdit * editor = 0, QWidget *parent = 0 );
	//! Destroy the object
	virtual ~ContentViewTextEditor();

	virtual void loadFromFile( const QString & fileName = QString() );
	virtual void saveToFile( const QString & fileName = QString() );
	virtual void loadFromDevice( QIODevice & d );

	//! The model editor is really created (based on the parser) at the first call.
	virtual QAbstractItemModel * model() const;

signals:
	void positionInEditorChanged( const QModelIndex & index );

public slots:
	virtual void updateModel();

protected:
	//! Change the parser of the model file editor.
	void setParser( ContentViewParser * parser );

	//! Return the current parser
	ContentViewParser * parser() const;

	//! Return the current root node
	ContentViewNode * rootNode() const;
private slots:
	virtual void textChanged();

private:
	mutable QPointer<ContentViewModel> m_model;
	ContentViewNode * m_rootNode;
	ContentViewParser * m_parser;

	QTimer * m_keyTimer;
};

#endif // _CONTENTVIEWTEXTEDITOR_H_
