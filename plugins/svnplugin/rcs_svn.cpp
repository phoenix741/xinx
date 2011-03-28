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
#include "authentificationformimpl.h"

// Qt header
#include <QTextStream>
#include <QRegExp>
#include <QDir>
#include <QApplication>
#include <QDesktopServices>
#include <QDebug>
#include <QFileDialog>
#include <QInputDialog>

// Svn
#include <svncpp/status.hpp>
#include <apr_time.h>
#include <svncpp/targets.hpp>

/* SubVersionContextListener */

SubVersionContextListener::SubVersionContextListener(RCS_SVN * parent) : _cancel(false), _parent(parent)
{
	connect(this, SIGNAL(signal_login()), this, SLOT(slot_login()), Qt::BlockingQueuedConnection);
	connect(this, SIGNAL(signal_cert_prompt()), this, SLOT(slot_cert_prompt()), Qt::BlockingQueuedConnection);
	connect(this, SIGNAL(signal_cert_password_prompt()), this, SLOT(slot_cert_password_prompt()), Qt::BlockingQueuedConnection);
}

SubVersionContextListener::~SubVersionContextListener()
{

}

void SubVersionContextListener::slot_login()
{
	AuthentificationFormImpl dlg(qApp->activeWindow());
	dlg.setRealm(_login.realm);
	dlg.setUsername(_login.username);
	dlg.setPassword(_login.password);
	dlg.setIsPasswordSaved(_login.may_be_save);

	if (dlg.exec())
	{
		_login.username = dlg.username();
		_login.password = dlg.password();
		_login.may_be_save = dlg.isPasswordSaved();

		_login.result = true;
		return;
	}

	_login.result = false;
}

bool SubVersionContextListener::contextGetLogin(const std::string& realm, std::string& username, std::string& password, bool& maySave)
{
	_login.realm       = QString::fromStdString(realm);
	_login.username    = QString::fromStdString(username);
	_login.password    = QString::fromStdString(password);
	_login.may_be_save = maySave;

	emit signal_login();

	username = _login.username.toStdString();
	password = _login.password.toStdString();
	maySave  = _login.may_be_save;

	return _login.result;
}

bool SubVersionContextListener::contextCancel()
{
	// Variable isCancel
	return _cancel;
}

void SubVersionContextListener::contextNotify(const char* path, svn_wc_notify_action_t action, svn_node_kind_t kind, const char* mime_type, svn_wc_notify_state_t content_state, svn_wc_notify_state_t prop_state, svn_revnum_t revision)
{
	RCS::struct_rcs_infos informations;

	RCS::rcsLog niveau = RCS::LogNormal;
	QString actionStr;
	switch (action)
	{
	case svn_wc_notify_add:
		informations = _parent->info(QString::fromAscii(path));
		actionStr = tr("Add");
		niveau = RCS::LogLocallyModified;
		emit _parent->stateChanged(QString::fromAscii(path), informations);
		break;
	case svn_wc_notify_copy:
		informations = _parent->info(QString::fromAscii(path));
		actionStr = tr("Copy");
		niveau = RCS::LogLocallyModified;
		emit _parent->stateChanged(QString::fromAscii(path), informations);
		break;
	case svn_wc_notify_delete:
		informations = _parent->info(QString::fromAscii(path));
		actionStr = tr("Delete");
		niveau = RCS::LogLocallyModified;
		emit _parent->stateChanged(QString::fromAscii(path), informations);
		break;
	case svn_wc_notify_restore:
		informations = _parent->info(QString::fromAscii(path));
		actionStr = tr("Restore");
		niveau = RCS::LogLocallyModified;
		emit _parent->stateChanged(QString::fromAscii(path), informations);
		break;
	case svn_wc_notify_revert:
		informations = _parent->info(QString::fromAscii(path));
		actionStr = tr("Revert");
		niveau = RCS::LogLocallyModified;
		emit _parent->stateChanged(QString::fromAscii(path), informations);
		break;
	case svn_wc_notify_failed_revert:
		actionStr = tr("Failed revert");
		niveau = RCS::LogError;
		break;
	case svn_wc_notify_resolved:
		actionStr = tr("Resolved");
		break;
	case svn_wc_notify_skip:
		actionStr = tr("Skip");
		niveau = RCS::LogError;
		break;
	case svn_wc_notify_update_delete:
		informations = _parent->info(QString::fromAscii(path));
		actionStr = tr("Update delete");
		niveau = RCS::LogRemotlyModified;
		emit _parent->stateChanged(QString::fromAscii(path), informations);
		return;
		break;
	case svn_wc_notify_update_add:
		informations = _parent->info(QString::fromAscii(path));
		actionStr = tr("Update add");
		niveau = RCS::LogRemotlyModified;
		emit _parent->stateChanged(QString::fromAscii(path), informations);
		return;
		break;
	case svn_wc_notify_update_update:
		informations = _parent->info(QString::fromAscii(path));
		actionStr = tr("Update update");
		niveau = RCS::LogRemotlyModified;
		emit _parent->stateChanged(QString::fromAscii(path), informations);
		return;
		break;
		informations = _parent->info(QString::fromAscii(path));
	case svn_wc_notify_update_completed:
		actionStr = tr("Update completed");
		niveau = RCS::LogRemotlyModified;
		emit _parent->stateChanged(QString::fromAscii(path), informations);
		return;
		break;
	case svn_wc_notify_update_external:
		actionStr = tr("Update external");
		niveau = RCS::LogRemotlyModified;
		return;
		break;
	case svn_wc_notify_status_completed:
		actionStr = tr("Status completed");
		return;
		break;
	case svn_wc_notify_status_external:
		actionStr = tr("Status external");
		break;
	case svn_wc_notify_commit_modified:
		informations = _parent->info(QString::fromAscii(path));
		actionStr = tr("Commit modified");
		niveau = RCS::LogLocallyModified;
		emit _parent->stateChanged(QString::fromAscii(path), informations);
		break;
	case svn_wc_notify_commit_added:
		informations = _parent->info(QString::fromAscii(path));
		actionStr = tr("Commit added");
		niveau = RCS::LogLocallyModified;
		emit _parent->stateChanged(QString::fromAscii(path), informations);
		break;
	case svn_wc_notify_commit_deleted:
		informations = _parent->info(QString::fromAscii(path));
		actionStr = tr("Commit deleted");
		niveau = RCS::LogLocallyModified;
		emit _parent->stateChanged(QString::fromAscii(path), informations);
		break;
	case svn_wc_notify_commit_replaced:
		informations = _parent->info(QString::fromAscii(path));
		actionStr = tr("Commit replaced");
		niveau = RCS::LogLocallyModified;
		emit _parent->stateChanged(QString::fromAscii(path), informations);
		break;
	case svn_wc_notify_commit_postfix_txdelta:
		informations = _parent->info(QString::fromAscii(path));
		actionStr = tr("Commit postfix txdelta");
		niveau = RCS::LogLocallyModified;
		emit _parent->stateChanged(QString::fromAscii(path), informations);
		return;
		break;
	case svn_wc_notify_blame_revision:
		actionStr = tr("Blame revision");
		break;
	case svn_wc_notify_locked:
		actionStr = tr("Locked");
		break;
	case svn_wc_notify_unlocked:
		actionStr = tr("Unlocked");
		break;
	case svn_wc_notify_failed_lock:
		actionStr = tr("Failed lock");
		niveau = RCS::LogError;
		break;
	case svn_wc_notify_failed_unlock:
		actionStr = tr("Failed unlock");
		niveau = RCS::LogError;
		break;
	case svn_wc_notify_exists:
		actionStr = tr("Exists");
		break;
	case svn_wc_notify_changelist_set:
		actionStr = tr("Changelist set");
		break;
	case svn_wc_notify_changelist_clear:
		actionStr = tr("Changelist clear");
		break;
	case svn_wc_notify_changelist_moved:
		actionStr = tr("Changelist moved");
		break;
	case svn_wc_notify_merge_begin:
		actionStr = tr("Merge begin");
		break;
	case svn_wc_notify_foreign_merge_begin:
		actionStr = tr("Foreign merge begin");
		break;
	case svn_wc_notify_update_replace:
		actionStr = tr("Update replace");
		break;
	case svn_wc_notify_property_added:
		actionStr = tr("Property added");
		break;
	case svn_wc_notify_property_modified:
		actionStr = tr("Property modified");
		break;
	case svn_wc_notify_property_deleted:
		actionStr = tr("Property deleted");
		break;
	case svn_wc_notify_property_deleted_nonexistent:
		actionStr = tr("Property deleted nonexistent");
		break;
	case svn_wc_notify_revprop_set:
		actionStr = tr("Revision property set");
		break;
	case svn_wc_notify_revprop_deleted:
		actionStr = tr("Revision property deleted");
		break;
	case svn_wc_notify_merge_completed:
		actionStr = tr("Merge completed");
		break;
	case svn_wc_notify_tree_conflict:
		actionStr = tr("Tree conflict");
		niveau = RCS::LogConflict;
		break;
	case svn_wc_notify_failed_external:
		actionStr = tr("Failed external");
		niveau = RCS::LogError;
		break;
	};

	const QString info = actionStr + " " + QString::fromAscii(path) + (revision >= 0 ? " (rev " + QString::number(revision) + ")" : QString());

	// Affichage de la nofication
	emit _parent->alert(niveau, info);
}

bool SubVersionContextListener::contextGetLogMessage(std::string& msg)
{
	// Retourne le message utilisé pour le commit
	qDebug() << "contextGetLogMessage(" << QString::fromStdString(msg) << ")";
	return true;
}

void SubVersionContextListener::slot_cert_prompt()
{
	_certificate_prompt.file_name = QFileDialog::getOpenFileName(qApp->activeWindow(), RCS_SVN::tr("Select a certificate file"), RCS_SVN::tr("Please select a certificate file."), RCS_SVN::tr("All files (*.*)"));
	_certificate_prompt.result = ! _certificate_prompt.file_name.isEmpty();
}

bool SubVersionContextListener::contextSslClientCertPrompt(std::string& certFile)
{
	_certificate_prompt.file_name = QString::fromStdString(certFile);

	emit signal_cert_prompt();

	certFile = _certificate_prompt.file_name.toStdString();
	return _certificate_prompt.result;
}

void SubVersionContextListener::slot_cert_password_prompt()
{
	_certificate_password_prompt.password = QInputDialog::getText(
												qApp->activeWindow(),
												RCS_SVN::tr("Paswword for the certificate file"),
												RCS_SVN::tr("Please give the password for the certificate of %1").arg(_certificate_password_prompt.realm),
												QLineEdit::Password, _certificate_password_prompt.password);
	_certificate_password_prompt.result   = !_certificate_password_prompt.password.isEmpty();
}

bool SubVersionContextListener::contextSslClientCertPwPrompt(std::string& password, const std::string& realm, bool& maySave)
{
	_certificate_password_prompt.realm       = QString::fromStdString(realm);
	_certificate_password_prompt.password    = QString::fromStdString(password);
	_certificate_password_prompt.may_be_save = maySave;

	emit signal_cert_password_prompt();

	password = _certificate_password_prompt.password.toStdString();
	maySave = _certificate_password_prompt.may_be_save;

	return _certificate_password_prompt.result;
}

svn::ContextListener::SslServerTrustAnswer SubVersionContextListener::contextSslServerTrustPrompt(const svn::ContextListener::SslServerTrustData& data, apr_uint32_t& acceptedFailures)
{
	qDebug() << "contextSslServerTrustPrompt" << QString::fromStdString(data.realm);
	// Ouvre une boite de dialogue avec data
	// Expliquer en quoi le certificat n'est pas sur :There were errors validating the server certificate.\nDo you want to trust this certificate?
	//static const FailureEntry CERT_FAILURES [] =
	//	{SVN_AUTH_SSL_UNKNOWNCA   , _("- The certificate is not issued by a trusted authority.\n  Use the fingerprint to validate the certificate manually!")},
	//	{SVN_AUTH_SSL_CNMISMATCH  , _("- The certificate hostname does not match.")},
	//	{SVN_AUTH_SSL_NOTYETVALID , _("- The certificate is not yet valid.")},
	//	{SVN_AUTH_SSL_EXPIRED     , _("- The certificate has expired.")},
	//	{SVN_AUTH_SSL_OTHER       , _("- The certificate has an unknown error.")}
	/*
		const int count = sizeof(CERT_FAILURES)/sizeof(CERT_FAILURES[0]);
		for (int i=0; i < count; i++)
		{
			if ((CERT_FAILURES[i].failure & trustData.failures) != 0)
			{
				failureStr += CERT_FAILURES[i].descr;
				failureStr += wxT("\n");
			}
		}
	*/

	/*
		Affichage de :
		- Hostname....

	*/
	return svn::ContextListener::ACCEPT_PERMANENTLY;
}

/* RCS_SVN */

RCS_SVN::RCS_SVN(const QString & basePath) : RCS(basePath)
{
//	const QString configDirectory    = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
//	const QString homeDirectory      = QDir::home().absoluteFilePath(configDirectory);

//	m_context = new svn::Context(QDir(homeDirectory).absoluteFilePath("SubVersion").toStdString ());
	m_context = new svn::Context();
	m_context->setListener(_listener = new SubVersionContextListener(this));
	m_client  = new svn::Client(m_context);
}

RCS_SVN::~RCS_SVN()
{
	delete m_context;
	delete m_client;
}

RCS::rcsFeatures RCS_SVN::features() const
{
	return RCS::RcsFeatureAdd | RCS::RcsFeatureRemove | RCS::RcsFeatureUpdateAndCommit | RCS::RcsFeatureBlame | RCS::RcsFeatureRevert | RCS::RcsFeatureLog;
}

RCS::rcsState RCS_SVN::svnStateToRcsState(svn_wc_status_kind textState, svn_wc_status_kind reposTextStatus)
{
	switch (textState)
	{
	case svn_wc_status_none:
	case svn_wc_status_unversioned:
	case svn_wc_status_obstructed:
		return RCS::Unknown;
	case svn_wc_status_normal:
		if (reposTextStatus == svn_wc_status_modified)
		{
			return RCS::NeedsCheckout;
		}
		else
		{
			return RCS::Updated;
		}
	case svn_wc_status_added:
		return RCS::LocallyAdded;
	case svn_wc_status_missing:
		return RCS::NeedsCheckout;
	case svn_wc_status_deleted:
		return RCS::LocallyRemoved;
	case svn_wc_status_replaced:
	case svn_wc_status_modified:
	case svn_wc_status_merged:
		return RCS::LocallyModified;
	case svn_wc_status_conflicted:
		return RCS::FileHadConflictsOnMerge;
	case svn_wc_status_ignored:
		return RCS::UnresolvedConflict;
	case svn_wc_status_external:
		return RCS::Updated;
	case svn_wc_status_incomplete:
		return RCS::NeedsCheckout;
	default:
		return RCS::Unknown;
	}
}

RCS::struct_rcs_infos RCS_SVN::svnInfoToRcsInfos(svn::Status infos)
{
	RCS::struct_rcs_infos rcsInfos = { QDir::fromNativeSeparators(infos.path()), RCS::Unknown, "0", QDateTime() };
	if (infos.isVersioned())
	{
		rcsInfos.version = QString("%1").arg(infos.entry().revision());

		uint cmtDate  = (quint64)infos.entry().cmtDate() / 1000000;
		rcsInfos.rcsDate = QDateTime::fromTime_t(cmtDate);
	}

	rcsInfos.state = svnStateToRcsState(infos.textStatus(), infos.reposTextStatus());

	return rcsInfos;
}

RCS::struct_rcs_infos RCS_SVN::info(const QString & path)
{
	_listener->_cancel = false;

	RCS::struct_rcs_infos result = { QDir::fromNativeSeparators(path), RCS::Unknown, "0", QDateTime() };
	svn::StatusEntries entries;
	try
	{
		try
		{
			entries = m_client->status(qPrintable(path), /* descend */ false, /* get_all */ true, /* update */ true, /* no_ignore */ false, /* ignore_externals */ false);
		}
		catch (svn::ClientException e)
		{
			emit alert(RCS::LogError, e.message());
			entries = m_client->status(qPrintable(path), /* descend */ false, /* get_all */ true, /* update */ false, /* no_ignore */ false, /* ignore_externals */ false);
		}

		if (! entries.size())
		{
			result = svnInfoToRcsInfos(entries.at(0));
		}
	}
	catch (svn::ClientException e)
	{
		emit alert(RCS::LogError, e.message());
	}

	return result;
}

QList<RCS::struct_rcs_infos> RCS_SVN::infos(const QString & path)
{
	_listener->_cancel = false;

	QList<RCS::struct_rcs_infos> result;
	svn::StatusEntries entries;
	try
	{
		try
		{
			entries = m_client->status(qPrintable(path), /* descend */ false, /* get_all */ true, /* update */ true, /* no_ignore */ false, /* ignore_externals */ false);
		}
		catch (svn::ClientException e)
		{
			emit alert(RCS::LogError, e.message());
			entries = m_client->status(qPrintable(path), /* descend */ false, /* get_all */ true, /* update */ false, /* no_ignore */ false, /* ignore_externals */ false);
		}

		for (size_t i = 0; i < entries.size(); i++)
		{
			result << svnInfoToRcsInfos(entries.at(i));
		}
	}
	catch (svn::ClientException e)
	{
		emit alert(RCS::LogError, e.message());
	}

	return result;
}

RCS::FilesOperation RCS_SVN::operations(const QStringList & paths)
{
	_listener->_cancel = false;

	RCS::FilesOperation operations;
	foreach(const QString & path, paths)
	{
		svn::StatusEntries entries;
		try
		{
			entries = m_client->status(qPrintable(path), /* descend */ true, /* get_all */ false, /* update */ false, /* no_ignore */ false, /* ignore_externals */ false);

			for (size_t i = 0; i < entries.size(); i++)
			{
				svn::Status status = entries.at(i);

				RCS::FileOperation operation(QDir::fromNativeSeparators(status.path()), RCS::Nothing);

				switch (status.textStatus())
				{
				case svn_wc_status_unversioned:
					operation.operation = RCS::AddAndCommit;
					break;
				case svn_wc_status_added:
				case svn_wc_status_deleted:
				case svn_wc_status_replaced:
				case svn_wc_status_modified:
				case svn_wc_status_merged:
					operation.operation = RCS::Commit;
					break;
				case svn_wc_status_missing:
					operation.operation = RCS::RemoveAndCommit;
					break;
				default:
					break;
				}

				if ((QFileInfo(operation.filename).isFile() && paths.contains(operation.filename)) || (operation.operation != RCS::Nothing))
				{
					operations << operation;
				}
			}
		}
		catch (svn::ClientException e)
		{
			emit alert(RCS::LogError, e.message());
		}
	}

	return operations;
}

void RCS_SVN::update(const QStringList & paths)
{
	_listener->_cancel = false;

	/* Create the target list */
	svn::Targets targets;
	foreach(const QString & path, paths)
	{
		svn::Path svnPath(path.toStdString());
		targets.push_back(svnPath);
	}

	/* Update */
	try
	{
		std::vector<svn_revnum_t> revisions = m_client->update(targets, svn::Revision::HEAD, true, false);
		emit alert(RCS::LogApplication, tr("Files updated."));
	}
	catch (svn::ClientException e)
	{
		emit alert(RCS::LogError, e.message());
	}
}

void RCS_SVN::commit(const QStringList & paths, const QString & message)
{
	_listener->_cancel = false;

	_listener->_message = message;
	/* Create the target list */
	svn::Targets targets;
	foreach(const QString & path, paths)
	{
		svn::Path svnPath(path.toStdString());
		targets.push_back(svnPath);
	}

	/* Commit */
	try
	{
		svn_revnum_t revision = m_client->commit(targets, message.toUtf8().data(), false, false);
		emit alert(RCS::LogApplication, tr("Files commited at revision %2.").arg(revision));
	}
	catch (svn::ClientException e)
	{
		emit alert(RCS::LogError, e.message());
	}
}

void RCS_SVN::add(const QStringList & paths)
{
	_listener->_cancel = false;

	foreach(const QString & path, paths)
	{
		try
		{
			svn::Path svnPath(path.toStdString());
			m_client->add(svnPath, false);
		}
		catch (svn::ClientException e)
		{
			emit alert(RCS::LogError, e.message());
		}
	}
}

void RCS_SVN::remove(const QStringList & paths)
{
	_listener->_cancel = false;

	foreach(const QString & path, paths)
	{
		try
		{
			svn::Path svnPath(path.toStdString());
			m_client->remove(svnPath, false);
		}
		catch (svn::ClientException e)
		{
			emit alert(RCS::LogError, e.message());
		}
	}
}

void RCS_SVN::revert(const QStringList & paths)
{
	_listener->_cancel = false;

	svn::Targets targets;
	foreach(const QString & path, paths)
	{
		svn::Path svnPath(path.toStdString());
		targets.push_back(svnPath);
	}

	try
	{
		m_client->revert(targets, false);
	}
	catch (svn::ClientException e)
	{
		emit alert(RCS::LogError, e.message());
	}
}

svn::Revision RCS_SVN::stringToRevision(const QString& rev)
{
	bool intConvert;
	svn_revnum_t revnumber = svn_revnum_t(rev.toInt(&intConvert));
	if (intConvert) return svn::Revision(revnumber);

	QDateTime dateTime = QDateTime::fromString(rev, Qt::DefaultLocaleShortDate);
	if (dateTime.isValid())
	{
		return svn::Revision(apr_time_t(dateTime.toTime_t() * 1000000));
	}

	return svn::Revision::UNSPECIFIED;
}

LogEntries RCS_SVN::log(const QString& path, const QString& revisionStart, const QString& revisionEnd)
{
	_listener->_cancel = false;

	LogEntries result;
	try
	{
		svn::Revision svnRevisionStart = svn::Revision::START;
		svn::Revision svnRevisionEnd   = svn::Revision::HEAD;

		if (! revisionStart.isEmpty())
		{
			svnRevisionStart = stringToRevision(revisionStart);
		}
		if (! revisionEnd.isEmpty())
		{
			svnRevisionEnd = stringToRevision(revisionEnd);
		}

		const svn::LogEntries* logs = m_client->log(qPrintable(path), svnRevisionStart, svnRevisionEnd);

		svn::LogEntries::const_iterator it = logs->begin();
		while (it != logs->end())
		{
			LogEntry logEntry;
			svn::LogEntry logLine = *it;

			logEntry.dateTime = QDateTime::fromTime_t (quint64(logLine.date / 1000000));
			logEntry.author   = QString::fromStdString(logLine.author);
			logEntry.message  = QString::fromStdString(logLine.message);
			logEntry.revision = QString::number(logLine.revision);

			std::list< svn::LogChangePathEntry >::iterator pathIt = logLine.changedPaths.begin();
			while (pathIt != logLine.changedPaths.end())
			{
				LogPath logPath;
				svn::LogChangePathEntry logPathLine = *pathIt;
				logPath.path  = QString::fromStdString(logPathLine.path);
				logPath.informations.append(QString("%1 (rev %2)").arg(QString::fromStdString(logPathLine.copyFromPath)).arg(QString::number(logPathLine.copyFromRevision)));

				logEntry.changedPath.append(logPath);
			}

			result.append(logEntry);
			it++;
		}

		delete logs;
	}
	catch (svn::ClientException e)
	{
		emit alert(RCS::LogError, e.message());
	}

	return result;
}

void RCS_SVN::blame(const QString & path)
{
	_listener->_cancel = false;

	try
	{
		svn::Path svnPath(path.toStdString());
		svn::AnnotatedFile* annotatedFile = m_client->annotate(svnPath, svn::Revision::START, svn::Revision::HEAD);

		svn::AnnotatedFile::iterator it = annotatedFile->begin();
		while (it != annotatedFile->end())
		{
			svn::AnnotateLine line = *it;
			emit alert(RCS::LogNormal, QString("%1 - %2").arg(line.lineNumber()).arg(QString::fromStdString(line.line())));
			it++;
		}

		delete annotatedFile;
	}
	catch (svn::ClientException e)
	{
		emit alert(RCS::LogError, e.message());
	}
}


void RCS_SVN::updateToRevision(const QString & path, const QString & revision, QByteArray * content)
{
	_listener->_cancel = false;

	/* Create the target list */
	svn::Path svnPath(path.toStdString());

	/* Update */
	try
	{
		content->clear();
		content->append(QString::fromStdString(m_client->cat(svnPath, svn::Revision(revision.toLong()))));
		emit alert(RCS::LogApplication, tr("Update %1 to revision %2.").arg(path).arg(revision));
	}
	catch (svn::ClientException e)
	{
		emit alert(RCS::LogError, e.message());
	}
}

void RCS_SVN::abort()
{
	_listener->_cancel = true;
}
