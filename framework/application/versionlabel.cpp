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

#include "versionlabel_p.h"
#include <QDomDocument>
#include <core/version.h>

/* VersionLabelPrivate */

void VersionLabelPrivate::finished(QNetworkReply * reply)
{
	QDomDocument document;
	document.setContent(reply);

	QDomNodeList homepageList = document.elementsByTagName("homepage");
	QDomNodeList stableList = document.elementsByTagName("stable_version");
	QDomNodeList unstableList = document.elementsByTagName("unstable_version");
	
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

	_stable = stable;
	_unstable = unstable;

	updateText();
}

void VersionLabelPrivate::updateText()
{
	QStringList versionInfo;
	if (_current.isValid())
	{
		versionInfo << tr("Current Version : %1").arg(_current.toString());
	}
	
	if (_stable.isValid())
	{
		QString stableText = tr("Stable Version : %1").arg(_stable.toString());
		versionInfo << stableText;
		if (_current < _stable)
		{
			stableText = tr("!!! New version released !!!") + "\n<br/>" + stableText;
		} else if (_current < _unstable)
		{
			stableText = tr("!!! New snapshot released !!!") + "\n<br/>" + stableText;
		}
		
		_label->setText(QString("<a href=\"%1\">%2</a>").arg(_homepage).arg(stableText));
	}

	if (_unstable.isValid())
	{
		versionInfo << tr("Unstable Version : %1").arg(_unstable.toString());
	}
	
	_label->setToolTip(versionInfo.join("\n"));
}

/* VersionLabel */

VersionLabel::VersionLabel(QWidget* parent, Qt::WindowFlags f): QLabel(parent, f), d(new VersionLabelPrivate)
{
	setOpenExternalLinks(true);
	
	d->_label   = this;
	d->_current = VERSION;
	d->_manager = new QNetworkAccessManager(d.data());
	connect(d->_manager, SIGNAL(finished(QNetworkReply*)), d.data(), SLOT(finished(QNetworkReply*)));

	QNetworkRequest request(QUrl("http://xinx.shadoware.org/files/release.xml"));
	d->_manager->get(request);

	d->updateText();
}

VersionLabel::~VersionLabel()
{

}

const Version& VersionLabel::currentVersion() const
{
	return d->_current;
}

const Version& VersionLabel::stableVersion() const
{
	return d->_stable;
}

const Version& VersionLabel::unstableVersion() const
{
	return d->_unstable;
}
