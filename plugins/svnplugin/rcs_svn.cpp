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

// Xinx header
#include "rcs_svn.h"
#include <core/xinxconfig.h>
#include <plugins/xinxpluginsloader.h>

// Qt header
#include <QTextStream>
#include <QRegExp>
#include <QDir>
#include <QApplication>

// Svn
#include <svncpp/status.hpp>
#include <apr_time.h>

/* RCS_SVN */

RCS_SVN::RCS_SVN(const QString & basePath) : RCS(basePath)
{
	m_context = new svn::Context(basePath.toStdString());
	m_client  = new svn::Client(m_context);
}

RCS_SVN::~RCS_SVN()
{
	delete m_context;
	delete m_client;
}

RCS::struct_rcs_infos RCS_SVN::info(const QString & path)
{
	QList<struct_rcs_infos> list = infos(path);
	if (list.size())
		return list.at(0);
	else
	{
		RCS::struct_rcs_infos rcsInfos = { QDir::fromNativeSeparators(path), RCS::Unknown, "0", QDateTime() };
		rcsInfos.state = RCS::Unknown;
		rcsInfos.version = "0";
		rcsInfos.rcsDate = QDateTime();
		return rcsInfos;
	}
}

QList<RCS::struct_rcs_infos> RCS_SVN::infos(const QString & path)
{
	QList<RCS::struct_rcs_infos> result;
	svn::StatusEntries entries;
	try
	{
		try
		{
			entries = m_client->status(qPrintable(path), true, true, true, false, false);
		}
		catch(svn::ClientException e)
		{
			entries = m_client->status(qPrintable(path), true, true, false, false, false);
		}
	
		for(int i = 0; i < entries.size(); i++)
		{
			svn::Status status = entries.at(i);
	
			RCS::struct_rcs_infos rcsInfos = { QDir::fromNativeSeparators(status.path()), RCS::Unknown, "0", QDateTime() };
			if (status.isVersioned())
			{
				rcsInfos.version = QString("%1").arg (status.entry().revision());
				switch (status.textStatus())
				{
				case svn_wc_status_none:
					rcsInfos.state = RCS::Unknown;
					break;
				case svn_wc_status_unversioned:
					rcsInfos.state = RCS::Unknown;
					break;
				case svn_wc_status_normal:
					rcsInfos.state = RCS::Updated;
					break;
				case svn_wc_status_added:
					rcsInfos.state = RCS::LocallyAdded;
					break;
				case svn_wc_status_missing:
					rcsInfos.state = RCS::NeedsCheckout;
					break;
				case svn_wc_status_deleted:
					rcsInfos.state = RCS::LocallyRemoved;
					break;
				case svn_wc_status_replaced:
					rcsInfos.state = RCS::LocallyModified;
					break;
				case svn_wc_status_modified:
					rcsInfos.state = RCS::LocallyModified;
					break;
				case svn_wc_status_merged:
					rcsInfos.state = RCS::LocallyModified;
					break;
				case svn_wc_status_conflicted:
					rcsInfos.state = RCS::FileHadConflictsOnMerge;
					break;
				case svn_wc_status_ignored:
					rcsInfos.state = RCS::UnresolvedConflict;
					break;
				case svn_wc_status_obstructed:
					rcsInfos.state = RCS::Unknown;
					break;
				case svn_wc_status_external:
					rcsInfos.state = RCS::Updated;
					break;
				case svn_wc_status_incomplete:
					rcsInfos.state = RCS::NeedsCheckout;
					break;
				}
	
				uint cmtDate  = (quint64)status.entry().cmtDate() / 1000000;
				rcsInfos.rcsDate = QDateTime::fromTime_t(cmtDate);
			}
	
			result << rcsInfos;
		}
	}
	catch(svn::ClientException e)
	{
		emit log(RCS::LogError, e.message());
	}

	return result;
}

RCS::FilesOperation RCS_SVN::operations(const QStringList & paths)
{

}

void RCS_SVN::update(const QStringList & path)
{

}

void RCS_SVN::commit(const QStringList & path, const QString & message)
{

}

void RCS_SVN::add(const QStringList & path)
{

}

void RCS_SVN::remove(const QStringList & path)
{

}

void RCS_SVN::updateToRevision(const QString & path, const QString & revision, QByteArray * content)
{

}

void RCS_SVN::abort()
{

}
