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

#include "versionavailabledialog_p.h"
#include <QDomDocument>
#include <QTextStream>
#include <core/version.h>

/* VersionAvailableDialogPrivate */
#include <core/xinxconfig.h>

void VersionAvailableDialogPrivate::releaseFinished()
{
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

	QDomDocument document;
	document.setContent(reply);

	QDomNodeList homepageList = document.elementsByTagName("homepage");
	QDomNodeList stableList = document.elementsByTagName("stable_version");
	QDomNodeList stableChangeLogList = document.elementsByTagName("stable_changelog");
	QDomNodeList unstableList = document.elementsByTagName("unstable_version");
	QDomNodeList unstableChangeLogList = document.elementsByTagName("unstable_changelog");

	QString stable, unstable;
	if (stableList.length())
	{
		stable = stableList.at(0).toElement().text();
	}
	if (unstableList.length())
	{
		unstable = unstableList.at(0).toElement().text();
	}
	if (homepageList.length())
	{
		_homepage = homepageList.at(0).toElement().text();
	}
	if (stableChangeLogList.length())
	{
		_stable_changelog = stableChangeLogList.at(0).toElement().text();
	}
	if (unstableChangeLogList.length())
	{
		_unstable_changelog = unstableChangeLogList.at(0).toElement().text();
	}

	_stable = stable;
	_unstable = unstable;

	updateText();
}

void VersionAvailableDialogPrivate::changelogFinished()
{
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
	QTextStream stream(reply);

	QRegExp regexp("xinx \\((.*)\\) .*\\; urgency=.*");
	Version changeLogVersion;
	while (!stream.atEnd())
	{
		QString line = stream.readLine();
		if (regexp.exactMatch(line))
		{
			changeLogVersion = regexp.cap(1);
			if (changeLogVersion > _current)
			{
				_changelog.append(tr("For version %1 :").arg(changeLogVersion.toString()));
			}
		}
		else if (changeLogVersion > _current)
		{
			if (line.startsWith("  *"))
			{
				_changelog.append(line);
			}
		}
	}

	_dialog->setDetailedText(_changelog.join("\n"));

	if ((_current < _stable) || (_current < _unstable))
	{
		_dialog->exec();
	}
	_dialog->deleteLater();
}

void VersionAvailableDialogPrivate::updateText()
{
	_update_version = XINXConfig::self()->config().lastUpdate;
	QString message = tr("The current version is %1.\n\nThe version %2 is available on %3.");
	if ((_current < _stable) && (_stable != _update_version))
	{
		_dialog->setText(tr("<span style=\"font-weight: bold\">A new version of XINX is available !</span>"));
		_update_version = _stable;

		QNetworkRequest request(QUrl(QString("http://xinx.shadoware.org/files/" + _stable_changelog)));
		request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysNetwork);
		QNetworkReply* reply = _manager->get(request);
		connect(reply, SIGNAL(finished()), this, SLOT(changelogFinished()));
	}
	else if ((_current < _unstable) && (_unstable != _update_version))
	{
		_dialog->setText(tr("<span style=\"font-weight: bold\">A new snapshot version of XINX is available !</span>"));
		_update_version = _unstable;

		QNetworkRequest request(QUrl(QString("http://xinx.shadoware.org/files/" + _unstable_changelog)));
		request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysNetwork);
		QNetworkReply* reply = _manager->get(request);
		connect(reply, SIGNAL(finished()), this, SLOT(changelogFinished()));
	}
	else
	{
		_dialog->setText(tr("<span style=\"font-weight: bold\">There is no new version of XINX.</span>"));
		_dialog->deleteLater();
	}

	if (_update_version.isValid())
	{
		_dialog->setInformativeText(message.arg(_current.toString()).arg(_update_version.toString()).arg(_homepage));

		QFontMetrics metrics(_dialog->font());
		_width = metrics.width(_dialog->informativeText()) /*+ _dialog->iconPixmap().size().width() */;
	}
}

/* VersionAvailableDialog */

VersionAvailableDialog::VersionAvailableDialog(QWidget* parent): QMessageBox(QMessageBox::Information, tr("New release of XINX available"), QString(), QMessageBox::Ok | QMessageBox::Ignore, parent), d(new VersionAvailableDialogPrivate)
{
	setTextFormat(Qt::RichText);
	d->_width   = 0;
	d->_dialog  = this;
	d->_current = VERSION;
	d->_manager = new QNetworkAccessManager(d.data());
	connect(this, SIGNAL(finished(int)), this, SLOT(deleteLater()));

	QNetworkRequest request(QUrl("http://xinx.shadoware.org/files/release.xml"));
	request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysNetwork);
	QNetworkReply * reply = d->_manager->get(request);
	connect(reply, SIGNAL(finished()), d.data(), SLOT(releaseFinished()));
}

VersionAvailableDialog::~VersionAvailableDialog()
{

}

const Version& VersionAvailableDialog::currentVersion() const
{
	return d->_current;
}

const Version& VersionAvailableDialog::stableVersion() const
{
	return d->_stable;
}

const Version& VersionAvailableDialog::unstableVersion() const
{
	return d->_unstable;
}

void VersionAvailableDialog::done(int code)
{
	if (clickedButton() == button(QMessageBox::Ignore))
	{
		XINXConfig::self()->config().lastUpdate = d->_update_version.toString();
		XINXConfig::self()->save();
	}

    QDialog::done(code);
}

void VersionAvailableDialog::showEvent(QShowEvent* event)
{
    QMessageBox::showEvent(event);
	/*
	QWidget *textField = findChild<QWidget *>("qt_msgbox_label");
	textField->setMinimumWidth(d->_width);
	textField->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

	if (parentWidget())
	{
		QRect parentRect = parentWidget()->geometry();
		QRect dialogRect = rect();

		move(parentRect.center() - dialogRect.center());
	}
	*/
}
