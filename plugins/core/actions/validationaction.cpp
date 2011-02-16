/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2006-2010 by Ulrich Van Den Hekke                         *
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
#include "validationaction.h"
#include <editors/editormanager.h>
#include <editors/textfileeditor.h>
#include "editors/widgeteditor/xml/xmltexteditor.h"

// Qt header
#include <QFileDialog>
#include <QMessageBox>
#include <QXmlSchema>
#include <QXmlSchemaValidator>

/* MessageHandler */

class MessageHandler : public QAbstractMessageHandler
{
public:
	virtual void handleMessage(QtMsgType type, const QString & description, const QUrl & identifier, const QSourceLocation & sourceLocation);

};

void MessageHandler::handleMessage(QtMsgType type, const QString & description, const QUrl & identifier, const QSourceLocation & sourceLocation)
{
	Q_UNUSED(identifier);

	switch (type)
	{
	case QtCriticalMsg:
	case QtFatalMsg:
		QMessageBox::critical(qApp->activeWindow(), tr("XML Validation"), tr("Line %1: %2").arg(sourceLocation.line()).arg(description));
		break;
	case QtWarningMsg:
		QMessageBox::warning(qApp->activeWindow(), tr("XML Validation"), tr("Line %1: %2").arg(sourceLocation.line()).arg(description));
		break;
	case QtDebugMsg:
		break;
	}
}

/* ValidationAction */

ValidationAction::ValidationAction() : XinxAction::Action(tr("&Validate ..."), QString())
{

}

bool ValidationAction::isVisible() const
{
	if (! EditorManager::self()) return false;
	TextFileEditor * tfe = qobject_cast<TextFileEditor*>(EditorManager::self()->currentEditor());
	if (! tfe) return false;
	XmlTextEditor * xte  = qobject_cast<XmlTextEditor*>(tfe->textEdit());
	if (! xte) return false;
	return true;
}

void ValidationAction::actionTriggered()
{
	QString filename = QFileDialog::getOpenFileName(qApp->activeWindow(), tr("XML Validation"), QString(), tr("Schemas (*.xsd)"));
	if (! filename.isEmpty())
	{
		MessageHandler handler;
		QXmlSchema schema;
		schema.setMessageHandler(&handler);
		if (schema.load(QUrl::fromLocalFile(filename)))
		{
			if (EditorManager::self())
			{
				TextFileEditor * tfe = qobject_cast<TextFileEditor*>(EditorManager::self()->currentEditor());

				if (tfe)
				{
					QByteArray textContent = tfe->textEdit()->toPlainText().toUtf8();

					QXmlSchemaValidator validator(schema);
					validator.setMessageHandler(&handler);
					if (validator.validate(textContent))
					{
						QMessageBox::information(qApp->activeWindow(), tr("XML Validation"), tr("File validate with success"));
					}
				}
			}
		}
	}
}
