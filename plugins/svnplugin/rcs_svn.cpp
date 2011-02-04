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

/* SubVersionContextListener */

SubVersionContextListener::SubVersionContextListener()
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
	AuthentificationFormImpl dlg(qApp->activeWindow ());
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
	return false;
}

void SubVersionContextListener::contextNotify(const char* path, svn_wc_notify_action_t action, svn_node_kind_t kind, const char* mime_type, svn_wc_notify_state_t content_state, svn_wc_notify_state_t prop_state, svn_revnum_t revision)
{
	// Affichage de la nofication
	// emit
}

bool SubVersionContextListener::contextGetLogMessage(std::string& msg)
{
	// Retourne le message utilisé pour le commit
	return false;
}

void SubVersionContextListener::slot_cert_prompt()
{
	_certificate_prompt.file_name = QFileDialog::getOpenFileName(qApp->activeWindow(), RCS_SVN::tr("Select a certificate file"), RCS_SVN::tr("Please select a certificate file."), RCS_SVN::tr("All files (*.*)"));
	_certificate_prompt.result = ! _certificate_prompt.file_name.isEmpty();
}

bool SubVersionContextListener::contextSslClientCertPrompt(std::string& certFile)
{
	_certificate_prompt.file_name = QString::fromStdString (certFile);

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

	emit signal_cert_password_prompt ();

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
	m_context->setListener(new SubVersionContextListener);
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
			entries = m_client->status(qPrintable(path), /* descend */ false, /* get_all */ true, /* update */ true, /* no_ignore */ false, /* ignore_externals */ false);
		}
		catch(svn::ClientException e)
		{
			emit log(RCS::LogNormal, e.message());
			entries = m_client->status(qPrintable(path), /* descend */ false, /* get_all */ true, /* update */ false, /* no_ignore */ false, /* ignore_externals */ false);
		}

		for(size_t i = 0; i < entries.size(); i++)
		{
			svn::Status status = entries.at(i);

			RCS::struct_rcs_infos rcsInfos = { QDir::fromNativeSeparators(status.path()), RCS::Unknown, "0", QDateTime() };
			if (status.isVersioned())
			{
				rcsInfos.version = QString("%1").arg (status.entry().revision());

				uint cmtDate  = (quint64)status.entry().cmtDate() / 1000000;
				rcsInfos.rcsDate = QDateTime::fromTime_t(cmtDate);
			}

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
				if (status.reposTextStatus() == svn_wc_status_modified)
				{
					rcsInfos.state = RCS::NeedsCheckout;
				}
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
