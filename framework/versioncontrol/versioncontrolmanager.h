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

#ifndef VERSIONCONTROL_PLUGINMANAGER_H
#define VERSIONCONTROL_PLUGINMANAGER_H

// Xinx header
#include <core/lib-config.h>
#include <core/xinxcore.h>
#include <core/xinxsingleton.h>
#include <versioncontrol/rcs.h>
#include <project/xinxprojectproject.h>

// Qt header
#include <QMap>
#include <QFutureWatcher>

class QAction;

namespace XinxProject
{
class Project;
typedef QSharedPointer<Project> ProjectPtr;
typedef QWeakPointer<Project> ProjectPtrWeak;
}

namespace VersionControl
{

class LIBEXPORT IManagerUI
{
public:
	virtual ~IManagerUI() {}
	virtual void startVersionControlOperations() = 0;
	virtual void logVersionControlOperation(RCS::rcsLog niveau, const QString & info) = 0;
	virtual void stopVersionControlOperations() = 0;
};

class LIBEXPORT Manager : public XinxLibSingleton<Manager>
{
	Q_OBJECT
public:
	RCS * createRevisionControl(QString revision, QString basePath) const;
	QMap<QString,QString> revisionControlIds() const;

	QAction * updateAllAction() const;
	QAction * commitAllAction() const;
	QAction * abortAction() const;

	void setUserInterface(IManagerUI * interface);
	IManagerUI * userInterface() const;

	void addRemoveFiles(RCS * rcs, QStringList toAdd, QStringList toRemove);
	void validWorkingCopy(RCS * rcs, RCS::FilesOperation operations, QString messages);
	void updateWorkingCopy(RCS * rcs, QStringList list);
	void updateRevision(RCS * rcs, QString path, QString revision, QByteArray * content);

	void waitForFinished();
	bool isExecuting() const;
signals:
	void started();
	void finished();

public slots:
	void abort();
	void logOperation(RCS::rcsLog niveau, const QString& info);
protected:
	virtual void initialisation();
private slots:
	void startOperations();
	void stopOperations();

	void changed(XinxProject::ProjectPtr project);

	void updateWorkingCopy();
	void validWorkingCopy();

	void updateActions();
private:
	Manager();

	void callRCSFileOperations(RCS * rcs, QStringList toAdd, QStringList toRemove);
	void callRCSValideWorkingCopy(RCS * rcs, RCS::FilesOperation operations, QString messages);
	void callRCSUpdateWorkingCopy(RCS * rcs, QStringList list);
	void callRCSUpdateRevision(RCS * rcs, QString path, QString revision, QByteArray * content);

	QAction * m_updateAll, * m_commitAll, * m_abort;
	IManagerUI * _interface;

	QFutureWatcher<void> m_rcsWatcher;
	RCS * _current_rcs;

	friend class XinxLibSingleton<Manager>;
};

}

#endif // VERSIONCONTROL_PLUGINMANAGER_H
