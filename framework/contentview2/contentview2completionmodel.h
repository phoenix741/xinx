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

#ifndef CONTENTVIEW2COMPLETIONMODEL_H
#define CONTENTVIEW2COMPLETIONMODEL_H

// Xinx header
#include <contentview2/contentview2node.h>
#include <contentview2/contentview2file.h>

// Qt header
#include <QSqlQueryModel>

namespace ContentView2
{

class LIBEXPORT CompletionModel : public QSqlQueryModel
{
	Q_OBJECT
public:
	CompletionModel(QSqlDatabase db, FileContainer file, QObject * parent = 0);
	virtual ~CompletionModel();

	virtual QVariant data(const QModelIndex &index, int role) const;
	virtual bool setHeaderData(int section, Qt::Orientation orientation, const QVariant & value, int role = Qt::EditRole);
	virtual void setPrefix(const QString & prefix);

	QSqlDatabase database() const;
	QSqlDatabase database();
public slots:
	//! Launch the selection in the base
	virtual void select();

protected:
	virtual QString whereClause() const;
private:
	QSqlDatabase m_db;
	QString m_prefix;
	FileContainer m_file;
};

} // namespace ContentView2

#endif // CONTENTVIEW2COMPLETIONMODEL_H
