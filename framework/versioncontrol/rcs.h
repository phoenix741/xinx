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

#pragma once
#ifndef __RCS_H__
#define __RCS_H__

// Xinx header
#include <core/lib-config.h>
#include <core/xinxcore.h>
#include <versioncontrol/logentry.h>

// Qt header
#include <QMetaType>
#include <QObject>
#include <QPair>
#include <QList>
#include <QDateTime>
#include <QDir>

class LIBEXPORT RCS : public QObject
{
	Q_OBJECT
public:
	explicit RCS(const QString & workingDirectory = QString());
	virtual ~RCS();


	enum rcsLog
	{
		LogError,
		LogNormal,
		LogConflict,
		LogLocallyModified,
		LogRemotlyModified,
		LogNotManaged,
		LogApplication
	};

	enum rcsState
	{
		Updated,
		LocallyModified,
		LocallyAdded,
		LocallyRemoved,
		NeedsCheckout,
		NeedPatch,
		UnresolvedConflict,
		FileHadConflictsOnMerge,
		Unknown,
		NotManaged
	};

	struct struct_rcs_infos
	{
		QString filename;
		rcsState state;
		QString version;
		QDateTime rcsDate;
	};

	enum rcsOperation
	{
		RemoveAndCommit,
		AddAndCommit,
		Commit,
		Nothing
	};

	struct LIBEXPORT FileOperation
	{
		FileOperation(const QString & f, rcsOperation o);
		FileOperation();

		QString filename;
		rcsOperation operation;
	};

	typedef QList<FileOperation> FilesOperation;

	enum rcsFeature
	{
		RcsFeatureUpdateAndCommit	= 0x01,
		RcsFeatureAdd				= 0x02,
		RcsFeatureRemove			= 0x04,
		RcsFeatureRevert			= 0x10,
		RcsFeatureLog				= 0x20,
		RcsFeatureBlame				= 0x40
	};

	Q_DECLARE_FLAGS(rcsFeatures, rcsFeature);

	/* Method on file of the repository */

	virtual void update(const QStringList & paths) {}
	virtual void updateToRevision(const QString & path, const QString & revision, QByteArray * content = 0) {}
	virtual void commit(const QStringList & paths, const QString & message) {}
	virtual LogEntries log(const QString & path, const QString & revisionStart = QString(), const QString & revisionEnd = QString()) { return LogEntries(); }
	virtual void blame(const QString & path) {}

	/* Operation on file of the working copy */

	virtual void add(const QStringList & paths) {}
	virtual void remove(const QStringList & paths) {}
	virtual void revert(const QStringList & paths) {}

	virtual FilesOperation operations(const QStringList & path) { return FilesOperation(); }
	virtual struct_rcs_infos info(const QString & path) = 0;
	virtual QList<struct_rcs_infos> infos(const QString & path) = 0;

	virtual rcsFeatures features() const = 0;

	/* Working Directory */

	void setWorkingDirectory(const QString & value);
	const QString & getWorkingDirectory() const;
public slots:
	virtual void abort() = 0;
signals:
	void stateChanged(const QString & fileName, RCS::struct_rcs_infos informations);
	void alert(RCS::rcsLog niveau, const QString & info);
private:
	QString m_workingDirectory;
};

Q_DECLARE_METATYPE(RCS::rcsLog)
Q_DECLARE_OPERATORS_FOR_FLAGS(RCS::rcsFeatures)

#endif // __RCS_H__
