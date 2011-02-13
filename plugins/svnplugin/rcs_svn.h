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

#ifndef RCS_SVN_H_
#define RCS_SVN_H_
#pragma once

// Xinx header
#include <versioncontrol/rcs.h>

// Qt header
#include <QProcess>
#include <QPointer>
#include <QCache>

// Svncpp header
#include <svncpp/context.hpp>
#include <svncpp/client.hpp>
#include <svncpp/context_listener.hpp>

class RCS_SVN;

class SubVersionContextListener : public QObject, public svn::ContextListener
{
	Q_OBJECT
public:
	SubVersionContextListener(RCS_SVN * parent);
	virtual ~SubVersionContextListener();

	virtual bool contextGetLogin(const std::string& realm, std::string& username, std::string& password, bool& maySave);
	virtual void contextNotify(const char* path, svn_wc_notify_action_t action, svn_node_kind_t kind, const char* mime_type, svn_wc_notify_state_t content_state, svn_wc_notify_state_t prop_state, svn_revnum_t revision);
	virtual bool contextCancel();

	virtual bool contextGetLogMessage(std::string& msg);
	virtual SslServerTrustAnswer contextSslServerTrustPrompt(const svn::ContextListener::SslServerTrustData& data, apr_uint32_t& acceptedFailures);
	virtual bool contextSslClientCertPrompt(std::string& certFile);
	virtual bool contextSslClientCertPwPrompt(std::string& password, const std::string& realm, bool& maySave);

	struct
	{
		bool result;
		QString realm;
		QString username;
		QString password;
		bool may_be_save;
	} _login;

	struct
	{
		bool result;
		QString file_name;
	} _certificate_prompt;

	struct
	{
		bool result;
		QString password;
		QString realm;
		bool may_be_save;
	} _certificate_password_prompt;

	bool _cancel;
	QString _message;
	RCS_SVN * _parent;
signals:
	void signal_login();
	void signal_cert_prompt();
	void signal_cert_password_prompt();
public slots:
	void slot_login();
	void slot_cert_prompt();
	void slot_cert_password_prompt();
};

class RCS_SVN : public RCS
{
	Q_OBJECT
public:
	RCS_SVN(const QString & basePath);
	virtual ~RCS_SVN();

	virtual struct_rcs_infos info(const QString & path);
	virtual QList<RCS::struct_rcs_infos> infos(const QString & path);
	virtual FilesOperation operations(const QStringList & paths);
	virtual void update(const QStringList & paths);
	virtual void commit(const QStringList & path, const QString & message);
	virtual void add(const QStringList & path);
	virtual void remove(const QStringList & path);
	virtual void updateToRevision(const QString & path, const QString & revision, QByteArray * content = 0);

public slots:
	virtual void abort();
private slots:

private:
	SubVersionContextListener * _listener;
	svn::Context * m_context;
	svn::Client  * m_client;

	friend class SubVersionContextListener;
};

#endif /*RCS_SVN_H_*/
