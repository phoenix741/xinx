/*
 XINX
 Copyright (C) 2007-2011 by Ulrich Van Den Hekke
 xinx@shadoware.org

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful.
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef FINDEDMODEL_H
#define FINDEDMODEL_H

#include <QAbstractItemModel>
#include <QMap>

class FindedLine;
class FindedFile;

class FindedModel : public QAbstractItemModel
{
	Q_OBJECT
public:
	enum
	{
		FilenameRole = Qt::UserRole,
		LineRole     = Qt::UserRole + 1,
		ContentRole  = Qt::UserRole + 2,
		PosStartRole = Qt::UserRole + 3,
		PosEndRole   = Qt::UserRole + 4
	};

	/**
	 * Create a find model containing the list of finded file and finded line.
	 * @param parent The parent object of the model
	 */
    FindedModel(QObject* parent = 0);
	//! Destroy the finded model.
    virtual ~FindedModel();

	/*!
	 * Add a new finded line in the model.
	 * @param filename Name of the file where the text is finded.
	 * @param line Line number of the finded text.
	 * @param content Content of the line where we can find text.
	 */
	void append(const QString & filename, int line, const QString & content, int posStart, int posEnd);
	/*!
	 * Add a new line for the text file. This incremente all line after the line number for the
	 * given file.
	 * @param filename Name of the file
	 * @param line Line number of the finded text.
	 */
	void addLine(const QString & filename, int line);
	/*!
	 * Remove a line of the text file. This decremente all line after of the given line number.
	 * @param filename Name of the file
	 * @param line Line number of the finded text
	 */
	void removeLine(const QString & filename, int line);
	/*!
	 * Remove all files, and line finded.
	 */
	void clear();

	bool isIndexFile(const QModelIndex & index) const;
	bool isIndexLine(const QModelIndex & index) const;

	QModelIndex index(FindedFile* file) const;
	QModelIndex index(FindedFile* file, FindedLine * line) const;
    virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex& child) const;

	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
private:
	FindedFile * filesValue(const QString & filename) const;

	QList<FindedFile*> _files;
	QMap<QString,int> _filesIndex;
};

#endif // FINDEDMODEL_H
