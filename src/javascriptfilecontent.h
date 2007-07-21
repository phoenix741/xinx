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

#ifndef __JAVASCRIPTFILECONTENT_H__
#define __JAVASCRIPTFILECONTENT_H__

#include "filecontentitemmodel.h"

class PrivateJavascriptFileContent;
class JavaScriptParser;

/*!
 * Model to show the file content of file of type JavaScript. The file is showed in a tree.
 * The globals variables and function in the root, and parameter and variables of functions in
 * the child.
 *
 * There is nothing to show other Javascript (like the XML Tree View).
 */
class JavascriptFileContent : public FileContentItemModel {
	Q_OBJECT
public:
	/*!
	 * Construct the file content model with a simple javascript parser.
	 * \param parser The simple javascript parser.
	 * \param parent The parent of the object parent.
	 */
	JavascriptFileContent( JavaScriptParser * parser, QObject *parent = 0 );
	/*!
	 * Destroy the file content object
	 */
	virtual ~JavascriptFileContent();
	
	QVariant data(const QModelIndex &index, int role) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex &index) const;
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;	
private:
	PrivateJavascriptFileContent * d;
	friend class PrivateJavascriptFileContent;
};

#endif // __JAVASCRIPTFILECONTENT_H__
