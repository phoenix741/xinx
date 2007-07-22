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

#ifndef __FILECONTENTITEMMODEL_H__
#define __FILECONTENTITEMMODEL_H__

#include <QAbstractItemModel>

/*!
 * Exception throw when the model can't be updated.
 */
class FileContentException {
public:
	/*!
	 * Create the exception with a message and a line.
	 * \param message Error of the exception.
	 * \param line Line where the error is.
	 */
	FileContentException( const QString & message, int line );
	/*!
	 * Return the message.
	 * \return The message of the error.
	 */
	const QString & getMessage() const;
	/*!
	 * Return the line where the error is.
	 * \return The line of the error.
	 */
	int getLine() const;
private:
	int m_line;
	QString m_message;
};

/*!
 * Model of data used in the content tree view. Represent the content of a file
 * For each element of the tree, the model return a link with the line and the
 * filename.
 */
class FileContentItemModel : public QAbstractItemModel {
	Q_OBJECT
public:
	/*!
	 * Construct a file content model.
	 * \param parent Parent of the file content model.
	 */
	FileContentItemModel( QObject *parent = 0 );
	/*!
	 * Destroy the model.
	 */
	virtual ~FileContentItemModel();
	
	/*!
	 * This structure give the line and the filename where we can find data that
	 * model represents.
	 * If the filename is empty, the model refere to the current file.
	 */
	struct struct_file_content {
		/*!
		 * The line of the element.
		 */
		int line;
		/*!
		 * The file name of the element. If empty the file is the current file.
		 */
		QString filename;
	};
};

Q_DECLARE_METATYPE ( FileContentItemModel::struct_file_content )

#endif // __FILECONTENTITEMMODEL_H__
