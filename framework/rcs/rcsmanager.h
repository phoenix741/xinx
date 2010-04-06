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

#ifndef RCSMANAGER_H
#define RCSMANAGER_H

// Xinx header
#include <core/lib-config.h>
#include <rcs/rcs.h>

// Qt header
#include <QObject>
#include <QMap>
#include <QQueue>
#include <QPair>
#include <QStringList>
#include <QFutureWatcher>
#include <QAction>

class LIBEXPORT RCSManager : public QObject
{
	Q_OBJECT
public:
	enum rcsAddRemoveOperation
	{
		RCS_ADD,
		RCS_REMOVE
	};

	~RCSManager();

	QMap<QString,QString> revisionControlIds() const;

	bool setCurrentRCS(const QString & rcs);
	QString currentRCS() const;
	QString description() const;
	RCS * currentRCSInterface() const;

	void setCurrentRootPath(const QString & rootPath);
	const QString & currentRootPath() const;

	void addFileOperation(rcsAddRemoveOperation op, const QStringList & filename, QWidget * parent = 0, bool confirm = true);

	static RCSManager * self();

	bool isExecuting() const;

	QAction * updateAllAction() const;
	QAction * commitAllAction() const;
	QAction * abortAction() const;
public slots:
	void validFileOperations();
	void rollbackFileOperations();

	void validWorkingCopy(QStringList files = QStringList(), QWidget * parent = 0);
	void updateWorkingCopy(QStringList list = QStringList());

	void updateToRevision(const QString & path, const QString & revision, QByteArray * content = 0);

	void abort();
signals:
	void stateChange(const QString & filename, RCS::struct_rcs_infos informations);
	void log(RCS::rcsLog niveau, const QString & info);
	void operationStarted();
	void operationTerminated();
private slots:
	void updateActions();
private:
	RCSManager();

	void callRCSFileOperations(RCS * rcs, QStringList toAdd, QStringList toRemove);
	void callRCSValideWorkingCopy(RCS * rcs, RCS::FilesOperation operations, QString messages);
	void callRCSUpdateWorkingCopy(RCS * rcs, QStringList list);
	void callRCSUpdateRevision(RCS * rcs, QString path, QString revision, QByteArray * content);

	RCS * createRevisionControl(QString revision, QString basePath) const;

	QAction * m_updateAll, * m_commitAll, * m_abort;

	QFutureWatcher<void> m_rcsWatcher;
	QString m_rcsName, m_rootPath;
	RCS * m_rcs;
	QQueue< QPair<rcsAddRemoveOperation,QStringList> > m_operations;
	static RCSManager * s_self;
};

#endif // RCSMANAGER_H
