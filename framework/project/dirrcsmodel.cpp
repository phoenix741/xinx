/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2007-2010 by Ulrich Van Den Hekke                         *
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

// Xinx header
#include "project/dirrcsmodel.h"
#include "project/xinxproject.h"
#include "plugins/xinxpluginsloader.h"
#include "rcs/rcsmanager.h"

// Qt header
#include <QBrush>
#include <QDateTime>

/* RCSCachedElement */

class RCSCachedElement
{
public:
	QString fileName, version;
	RCS::rcsState state;
	QDateTime filedate, rcsdate;

	QString stringState() const;
};

QString RCSCachedElement::stringState() const
{
	if (state == RCS::Unknown)
		return DirRCSModel::tr("Unknown");
	if (state == RCS::LocallyModified)
		return DirRCSModel::tr("Locally modified");
	if (state == RCS::LocallyAdded)
		return DirRCSModel::tr("Locally added");
	if ((state == RCS::UnresolvedConflict) || (state == RCS::FileHadConflictsOnMerge))
		return DirRCSModel::tr("Has conflict");
	if (state == RCS::NeedsCheckout)
		return DirRCSModel::tr("No modified") + ", " + DirRCSModel::tr("Need checkout");
	return DirRCSModel::tr("No modified");
}

/* DirRCSModel */

DirRCSModel::DirRCSModel(const QStringList & nameFilters, QDir::Filters filters, QDir::SortFlags sort, QObject * parent) : QDirModel(nameFilters, filters, sort, parent), m_cache(500)
{
	connect(RCSManager::self(), SIGNAL(stateChange(QString,RCS::struct_rcs_infos)), this, SLOT(refresh(QString)));
}

DirRCSModel::DirRCSModel(QObject *parent) : QDirModel(parent)
{

}

DirRCSModel::~DirRCSModel()
{
}

RCSCachedElement DirRCSModel::cachedValue(const QString & key) const
{
	QString path = QFileInfo(key).absoluteFilePath();
	RCSCachedElement * value = m_cache.object(path);
	if (value)
	{
		if (value->filedate == QFileInfo(path).lastModified())
			return *value;
		else
			m_cache.remove(path);
	}

	value = new RCSCachedElement;
	RCS::struct_rcs_infos infos = RCSManager::self()->currentRCSInterface()->info(path);

	value->fileName = path;
	value->state    = infos.state;
	value->filedate = QFileInfo(path).lastModified();
	value->rcsdate  = infos.rcsDate.toLocalTime();
	value->version  = infos.version;
	m_cache.insert(path, value);
	return *value;
}

QVariant DirRCSModel::data(const QModelIndex &index, int role) const
{
	if (! index.isValid()) return QVariant();

	QString path = filePath(index);
	if (RCSManager::self()->currentRCSInterface())
	{
		if (role == Qt::BackgroundRole && index.column() == 0)
		{
			RCSCachedElement element = cachedValue(path);
			RCS::rcsState state = element.state;
			if (state == RCS::Unknown)
				return QBrush(Qt::gray);
			if (state == RCS::NeedsCheckout)
				return QBrush(Qt::cyan);
			if (state == RCS::LocallyModified)
				return QBrush(Qt::yellow);
			if (state == RCS::LocallyAdded)
				return QBrush(Qt::green);
			if ((state == RCS::UnresolvedConflict) || (state == RCS::FileHadConflictsOnMerge))
				return QBrush(Qt::red);

			return QDirModel::data(index, role);
		}
		else if (role == Qt::ToolTipRole && index.column() == 0)
		{
			RCSCachedElement element = cachedValue(path);
			QString filedate = element.filedate.toString(),
			                   date     = element.rcsdate.toString(),
			                              version  = element.version,
			                                         status   = element.stringState();

			QString tips = tr("Filename : %1\n"
			                  "Status : %2\n"
			                  "Date of file : %3\n"
			                  "Date in CVS : %4\n"
			                  "Version : %5\n\n"
			                  "Cache size : %6/%7").arg(QFileInfo(path).fileName()).arg(status).arg(filedate).arg(date).arg(version).arg(m_cache.totalCost()).arg(m_cache.maxCost());
			return tips;
		}
	}
	return QDirModel::data(index, role);
}

void DirRCSModel::refresh(const QString & path)
{
	m_cache.remove(QFileInfo(path).absoluteFilePath());
	QModelIndex idx = index(path);
	if (idx.isValid())
		QDirModel::refresh(idx);
}
