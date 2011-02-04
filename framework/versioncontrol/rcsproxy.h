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

#ifndef RCSMANAGER_H
#define RCSMANAGER_H

// Xinx header
#include <core/lib-config.h>
#include <versioncontrol/rcs.h>

// Qt header
#include <QObject>
#include <QMap>
#include <QQueue>
#include <QPair>
#include <QStringList>
#include <QAction>
#include <versioncontrol/versioncontrolmanager.h>

namespace XinxProject
{
	class Project;
}

namespace VersionControl
{

class LIBEXPORT RCSProxy : public QObject
{
	Q_OBJECT
public:
	enum rcsAddRemoveOperation
	{
		RCS_ADD,
		RCS_REMOVE
	};

	explicit RCSProxy(XinxProject::Project * project);
	~RCSProxy();

	bool setCurrentRCS(const QString & rcs);
	QString currentRCS() const;
	QString description() const;
	RCS * currentRCSInterface() const;

	void setCurrentRootPath(const QString & rootPath);
	const QString & currentRootPath() const;

	void addFileOperation(rcsAddRemoveOperation op, const QStringList & filename, QWidget * parent = 0, bool confirm = true);
public slots:
	void validFileOperations();
	void rollbackFileOperations();

	void validWorkingCopy(QStringList files = QStringList(), QWidget * parent = 0);
	void updateWorkingCopy(QStringList list = QStringList());

	void updateToRevision(const QString & path, const QString & revision, QByteArray * content = 0);

	void abort();
signals:
	void stateChange(const QString & filename, RCS::struct_rcs_infos informations);
private:
	RCS * m_rcs;

	QString m_rcsName, m_rootPath;
	QQueue< QPair<rcsAddRemoveOperation,QStringList> > m_operations;

	XinxProject::Project* m_project;
};

}

Q_DECLARE_METATYPE(VersionControl::RCSProxy*);

#endif // RCSMANAGER_H
