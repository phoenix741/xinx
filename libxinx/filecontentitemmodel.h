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

#ifndef __FILECONTENTITEMMODEL_H__
#define __FILECONTENTITEMMODEL_H__

// Xinx header
#include <filecontentstructure.h>

// Qt header
#include <QAbstractItemModel>

class PrivateFileContentModel;

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
	FileContentItemModel( FileContentElement * root, QObject *parent = 0 );
	/*! Destroy the model. */
	virtual ~FileContentItemModel();

	/*!
	 * This structure give the line and the filename where we can find data that
	 * model represents.
	 * If the filename is empty, the model refere to the current file.
	 */
	struct struct_file_content {
		int line; /*!< The line of the element. */
		QString filename; /*!< The file name of the element. If empty the file is the current file. */
	};

	virtual QVariant data( const QModelIndex &index, int role ) const;
	virtual Qt::ItemFlags flags( const QModelIndex &index ) const;
	virtual QModelIndex index( int row, int column, const QModelIndex &parent = QModelIndex() ) const;
	QModelIndex index( FileContentElement * element ) const;
	virtual QModelIndex parent( const QModelIndex &index ) const;
	virtual bool hasChildren( const QModelIndex & parent = QModelIndex() ) const;
	virtual int rowCount( const QModelIndex &parent = QModelIndex() ) const;
	virtual QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
	virtual int columnCount( const QModelIndex &parent = QModelIndex() ) const;
public slots:
	void update( FileContentElement * element );
	void beginInsertRow( FileContentElement * element, int row );
	void endInsertRow();
	void beginRemoveRow( FileContentElement * element );
	void endRemoveRow();
private:
	PrivateFileContentModel * d;
	friend class PrivateFileContentModel;
};

Q_DECLARE_METATYPE( FileContentItemModel::struct_file_content )

#endif // __FILECONTENTITEMMODEL_H__
