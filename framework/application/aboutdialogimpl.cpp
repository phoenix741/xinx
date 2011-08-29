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

// Xinx header
#include "aboutdialogimpl.h"
#include <core/exceptions.h>
#include <core/version.h>
#include <QFile>
#include <QTextStream>

/* AboutDialogImpl */

/*!
 * \class AboutDialogImpl
 * \brief Implementation of about dialog containts informations as the version, the licence,
 * the logo, and authors.
 *
 * The implementation containts only a constructor who defines default dialog
 * presentation : Windows Style Dialog.
 *
 * \intern
 */

/*!
 * \brief Constructor of The about dialog implementation.
 *
 * We defines a default windows flags. The windows can be resize.
 *
 * \param parent The parent of the dialog
 * \param f Flags to use on Windows. By default, the dialog have a fixed size.
 */
AboutDialogImpl::AboutDialogImpl(QWidget * parent, Qt::WFlags f) : QDialog(parent, f)
{
	setupUi(this);

	QString versionString;
	if(!QString(VERSION_CHANGESET).isEmpty())
	{
		versionString = tr("<b>XINX %1 (Changeset %2 using Qt %3)</b>").arg(VERSION).arg(VERSION_CHANGESET).arg(qVersion());
	}
	else
	{
		versionString = tr("<b>XINX %1 (using Qt %2)</b>").arg(VERSION).arg(qVersion());
	}

	m_labelVersion->setText(versionString);

	m_aboutLabel->setText("<b>XINX</b> Is Not only XML<br/>"
						  "(c) 2006-2011, Ulrich Van Den Hekke<br/>"
						  "<a href=\"http://xinx.shadoware.org/\">http://xinx.shadoware.org/</a><br/>"
						  "<br/>"
						  "<i>This software is cover by the GPL Public Licence v3. <br/>"
						  "Please read the software licence before use it. <br/><br/>"
						  "Icon and Logo are under the GPL Public Licence and comes from <br/>"
						  "K-Desktop Environment, KDevelop.<br/><br/>"
						  "<b>XINX</b> use internally a modified version of QCodeEdit written <br/>"
						  "by fullmetalcoder. This software is cover by the <br/>"
						  "GPL Public Licence too.</i>"
						 );

	m_authorText->setHtml("Please use <a href=\"http://xinx.shadoware.org/\" target=\"_new\">http://xinx.shadoware.org/</a> to report bug<br/>"
						  "<br/>"
						  "<b>Ulrich Van Den Hekke</b><br/>"
						  "Mail: <a href=\"mailto:ulrich.vdh@free.fr\">ulrich.vdh@free.fr</a><br/>"
						  "Home page: <a href=\"http://www.shadoware.org/\" target=\"_new\">http://www.shadoware.org/</a><br/>"
						  "Project leader"
						 );

	m_thanksText->setHtml("<b>Nicolas CARON</b><br/>"
						  "For ideas and reports and more<br/>"
						  "<b>Sebastien DESPREZ</b><br/>"
						  "For ideas and reports and more<br/>"
						  "<b>Mickael BERTELOOT</b><br/>"
						  "For ideas and reports and more<br/>"
						  "<b>Kamal B</b><br/>"
						  "For ideas and reports and more<br/>"
						  "<b>Alexandre ROCHER</b><br/>"
						  "For ideas and reports, some snipets, some scripts and more<br/>"
						  "<b>Matthieu DUBOUT</b><br/>"
						  "For ideas, reports, some snipets, and more<br/>"
						  "<b>Cathy VANDENHEKKE</b><br/>"
						  "For translations<br/>"
						  "<br/>"
						  "<b>Luc Briant</b><br/>"
						  "Home page: <a href=\"http://qcodeedit.edyuk.org/\" target=\"_new\">http://qcodeedit.edyuk.org/</a><br/>"
						  "Author of the QCodeEdit editor used by <b>XINX</b><br/>"
						  "<br/>"
						  "And all people who help me in some ways and i have forgotten to mention"
						 );

	QFile licenceFile(":/document/licence.html");
	if (licenceFile.open(QIODevice::ReadOnly))
	{
		QTextStream textStream(&licenceFile);

		_licence->setHtml(textStream.readAll());
	}
}


