/***************************************************************************
 *   Copyright (C) 2006 by Ulrich Van Den Hekke                            *
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

#ifndef __NUMBERBAR_H__
#define __NUMBERBAR_H__

#include <QWidget>

class QTextEdit;
class QPaintEvent;

/*!
 * Represent the bar used in a text editor and contains line number. The bar must be associate with
 * the QTextEdit to be used.
 * When we refresh the QTextEdit (i.e. scroll) the number bar must be repain. The number bar use the
 * block of the QTextEdit. He considere that one block is equal to one line. It's rigth in plain text
 * document, but not in rich edit document.
 */
class NumberBar : public QWidget {
	Q_OBJECT
public:
	/*!
	 * Create a bar, with a parent widget and a direction. the direction is Vertical
	 * by default, and is the only current value.
	 */
	NumberBar( QWidget *parent );
	/*!
	 * Destroy the number bar.
	 */
	virtual ~NumberBar();
 
 	/*!
 	 * Define the text edit used with the number bar. The Text Edit will be used to count the 
 	 * number block and known where ths start of the document is show.
 	 * \param edit the text edit associate with the number bar.
 	 */
	void setTextEdit( QTextEdit *edit );
	/*!
	 * Redfine the paint event of the number bar. The paint event write the line number in 
	 * the number bar.
	 */
	virtual void paintEvent( QPaintEvent *ev );
	/*!
	 * Redefine the mouse event of the number bar. The mouse event show a popup to toggled a 
	 * Bookmark.
	 */
	virtual void mousePressEvent ( QMouseEvent * event );

	/*!
	 * Show icon bookmark at line \e line.
	 */
	void setBookmark( int line, bool enabled );
	
	/*!
	 * List of bookmark of the editor.
	 */
	QList<int> & listOfBookmark();
	
	/*!
	 * Set the list of error
	 */
	void setErrors( QList<int> errors );
signals:
	/*!
	 * Signal is emitted when a bookmark is toogled.
	 * \param line Line of the bookmark
	 * \param enabled If enabled, the bookmark is added.
	 */
	void bookmarkToggled( int line, bool enabled );
private:
	QTextEdit *m_edit;
	QList<int> m_lineBookmark, m_errors;
};

#endif // __NUMBERBAR_H__
