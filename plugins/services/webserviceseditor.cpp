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
#include <project/xinxproject.h>
#include <editors/xinxcodeedit.h>
#include "webserviceseditor.h"
#include "webservices.h"
#include "borderlayout.h"
#include <editors/prettyprint/xmlprettyprinter.h>
#include <editors/widgeteditor/xml/xmltexteditor.h>
#include <editors/bookmarktexteditorinterface.h>

// Qt header
#include <QToolButton>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <QApplication>
#include <QFileInfo>
#include <QDir>
#include <QDomDocument>
#include <QSplitter>
#include <QPlainTextEdit>
#include <QThread>

/* WebServicesEditor */

WebServicesEditor::WebServicesEditor(IFileTypePlugin * fileType, QWidget *parent) : TextFileEditor(new XmlTextEditor(), fileType, parent)
{
	initObjects();
}

WebServicesEditor::~WebServicesEditor()
{

}

void WebServicesEditor::initObjects()
{
	m_http = new QtSoapHttpTransport(this);

	m_oldParamValue = QString();

	m_servicesList = new QComboBox(this);
	m_actionList = new QComboBox(this);
	m_paramList = new QComboBox(this);
	m_paramList->setEditable(true);

	m_resultWidget = new QWidget(this);

	m_progressBar = new QProgressBar(m_resultWidget);
	m_progressBar->setMinimum(0);
	m_progressBar->setMaximum(0);
	m_progressBar->setVisible(false);

	m_benchmark = new QLabel(m_resultWidget);

	m_resultList = new QComboBox(m_resultWidget);

	m_resultEdit = new XinxCodeEdit(true, m_resultWidget);
	m_resultEdit->editor()->setFlag(QEditor::ReadOnly, true);
	//m_resultEdit->editor()->setContextMenuPolicy( Qt::NoContextMenu );
	m_resultEdit->setHighlighter("XML");

	connect(WebServicesManager::self(), SIGNAL(changed()), this, SLOT(webServicesChanged()));
	connect(m_servicesList, SIGNAL(activated(int)), this, SLOT(webServicesActivated(int)));
	connect(m_actionList, SIGNAL(activated(int)), this, SLOT(webServicesParamActivated(int)));
	connect(m_paramList, SIGNAL(activated(int)), this, SLOT(webServicesValueActivated()));

	connect(m_paramList->lineEdit(), SIGNAL(editingFinished()), this, SLOT(paramListEditingFinished()));

	connect(m_http, SIGNAL(responseReady()), SLOT(readResponse()));

	loadServicesList();
	loadActionsList(m_servicesList->currentIndex());
	loadValuesList(m_actionList->currentIndex());

	setModified(false);
}

void WebServicesEditor::initLayout()
{
	/* Widget for message */
	QSplitter * splitter = new QSplitter(Qt::Vertical, this);

	/* Layouts */
	BorderLayout * grid = new BorderLayout(this, 0, 0);
	grid->addWidget(splitter, BorderLayout::Center);

	setLayout(grid);

	QLabel * label1 = new QLabel(tr("WebServices : "), this);
	QLabel * label2 = new QLabel(tr("Action : "), this);
	QLabel * label3 = new QLabel(tr("Parameter : "), this);

	QHBoxLayout * hbox = new QHBoxLayout;
	hbox->addWidget(label1);
	hbox->addWidget(m_servicesList);
	hbox->addSpacing(10);
	hbox->addWidget(label2);
	hbox->addWidget(m_actionList);
	hbox->addSpacing(10);
	hbox->addWidget(label3);
	hbox->addWidget(m_paramList);
	//hbox->addStretch();

	grid->add(hbox, BorderLayout::North);

	/* Layout of the result widget */
	QVBoxLayout * vbox = new QVBoxLayout(m_resultWidget);
	hbox = new QHBoxLayout;

	QLabel * label4 = new QLabel(tr("Result values: "), m_resultWidget);
	hbox->addWidget(label4);

	hbox->addWidget(m_resultList);
	hbox->addSpacing(10);
	hbox->addWidget(m_progressBar);
	hbox->addSpacing(10);
	hbox->addWidget(m_benchmark);
	hbox->addStretch();

	/* Edit part */
	vbox->addLayout(hbox);
	vbox->addWidget(m_resultEdit);

	splitter->addWidget(textEdit());
	splitter->addWidget(m_resultWidget);
}

bool WebServicesEditor::autoIndent()
{
	try
	{
		XMLPrettyPrinter prettyPrinter;
		prettyPrinter.setText(textEdit()->toPlainText());
		prettyPrinter.process();

		textEdit()->textCursor().beginEditBlock();
		textEdit()->editor()->selectAll();
		textEdit()->textCursor().insertText(prettyPrinter.getResult());
		textEdit()->textCursor().endEditBlock();
	}
	catch (XMLPrettyPrinterException e)
	{
		ErrorManager::self()->addMessage(lastFileName(), e.m_line, ErrorManager::MessageError, e);
		return false;
	}
	return true;
}

const QHash<QString,QString> & WebServicesEditor::values()
{
	store(m_paramList->currentText());

	return m_paramValues;
}

WebServices * WebServicesEditor::service()
{
	int index = m_servicesList->currentIndex();
	if ((index >= 0) && m_servicesList->itemData(index).isValid())
		return static_cast<WebServices*>(m_servicesList->itemData(index).value<void*>());
	else
		return NULL;
}

Operation * WebServicesEditor::operation()
{
	int index = m_actionList->currentIndex();
	if ((index >= 0) && m_actionList->itemData(index).isValid())
		return (Operation*)(m_actionList->itemData(index).value<void*>());
	else
		return NULL;
}

class WrongFwsFormatException : public XinxException
{
public:
	WrongFwsFormatException(const QString & message) : XinxException(message)
	{
	}
};

void WebServicesEditor::loadFromDevice(QIODevice & d)
{
	m_paramValues.clear();
	m_oldParamValue = QString();
	try
	{
		QDomDocument document;
		if (! document.setContent(&d, false)) throw WrongFwsFormatException(tr("Can't read content of file"));

		QDomElement element = document.documentElement();
		if (element.tagName() != "webservice") throw WrongFwsFormatException(tr("Not a WebServices file"));

		m_serviceName = element.attribute("service");
		m_operationName = element.attribute("action");

		QDomElement param = element.firstChildElement();
		while (! param.isNull())
		{
			QString paramStr = param.tagName();
			QString paramValue;
			QDomNode text = param.firstChild();
			while (! text.isNull())
			{
				if (text.isText())
				{
					paramValue += text.toText().data();
					break;
				}
				text = text.nextSibling();
			}

			m_paramValues[ paramStr ] = paramValue;

			param = param.nextSiblingElement();
		}

		loadServicesList();
		loadActionsList(m_servicesList->currentIndex());
		loadValuesList(m_actionList->currentIndex());
		restore(m_paramList->currentText());
	}
	catch (WrongFwsFormatException)
	{
		d.seek(0);
		QTextStream in(&d);
		QApplication::setOverrideCursor(Qt::WaitCursor);
		textEdit()->setPlainText(in.readAll());
	}
}

void WebServicesEditor::saveToDevice(QIODevice & d)
{
	store(m_paramList->currentText());

	QDomDocument document;
	QDomElement element = document.createElement("webservice");
	document.appendChild(element);
	element.setAttribute("service", m_servicesList->currentText());
	element.setAttribute("action", m_actionList->currentText());

	foreach(const QString & param, m_paramValues.keys())
	{
		QDomElement paramElement = document.createElement(param) ;
		element.appendChild(paramElement);

		QDomText text = document.createTextNode(m_paramValues[param]);
		paramElement.appendChild(text);
	}

	QTextStream out(&d);
	document.save(out, 3);
}

void WebServicesEditor::serialize(XinxProjectSessionEditor * data, bool content)
{
	AbstractEditor::serialize(data, content);
	store(m_paramList->currentText());

	data->writeProperty("Service", QVariant(m_servicesList->currentText()));
	data->writeProperty("Action", QVariant(m_actionList->currentText()));
	data->writeProperty("Param", QVariant(m_paramList->currentText()));
	data->writeProperty("Position", QVariant(textEdit()->textCursor().position()));

	int i = 0;
	if (content && isModified())
	{
		foreach(const QString & param, m_paramValues.keys())
		{
			data->writeProperty(QString("Key_%1").arg(i), QVariant(param));
			data->writeProperty(QString("Value_%1").arg(i++), QVariant(m_paramValues[ param ]));
		}
		data->writeProperty("KeyCount", QVariant(i));
	}

	i = 0;
	foreach(int line, bookmarkTextInterface()->bookmarks())
	{
		data->writeProperty(QString("Bookmark_%1").arg(i++), QVariant(line));
	}
	data->writeProperty("BookmarkCount", QVariant(i));
}

void WebServicesEditor::deserialize(XinxProjectSessionEditor * data)
{
	int position = 0;
	QString key, value, param;

	m_paramValues.clear();
	m_oldParamValue = QString();

	position  = data->readProperty("Position") .toInt();

	int bc = data->readProperty("BookmarkCount").toInt();
	for (int i = 0 ; i < bc; i++)
	{
		bookmarkTextInterface()->setBookmark(data->readProperty(QString("Bookmark_%1").arg(i)).toInt(), true);
	}

	m_serviceName = data->readProperty("Service").toString();
	m_operationName = data->readProperty("Action").toString();
	param = data->readProperty("Param").toString();

	int pc = data->readProperty("KeyCount").toInt();
	for (int i = 0 ; i < pc; i++)
	{
		QString name  = data->readProperty(QString("Key_%1").arg(i)).toString(),
		                value = data->readProperty(QString("Value_%1").arg(i)).toString();

		m_paramValues[ name ] = value;
	}

	if (m_paramValues.keys().count() == 0)
	{
		AbstractEditor::deserialize(data);

		if (! lastFileName().isEmpty())
			loadFromFile(lastFileName());
	}
	else
	{
		AbstractEditor::deserialize(data);
	}

	loadServicesList();
	loadActionsList(m_servicesList->currentIndex());
	loadValuesList(m_actionList->currentIndex());
	if (! param.isEmpty())
		m_paramList->setCurrentIndex(m_paramList->findText(param));
	restore(m_paramList->currentText());

	if (m_paramValues.keys().count() > 0)
		textEdit()->setModified(isModified());

	QDocumentCursor tc(textEdit()->editor()->document());
	tc.movePosition(position, QDocumentCursor::Right);
	textEdit()->setTextCursor(tc);
}


void WebServicesEditor::paramListEditingFinished()
{
	int index = m_paramList->findText(m_paramList->lineEdit()->text());
	if (index >= 0) m_paramList->setItemIcon(index, QIcon(":/services/images/serviceparam.png"));
}

void WebServicesEditor::loadServicesList()
{
	m_servicesList->clear();
	foreach(WebServices * ed, *(WebServicesManager::self()))
	if (! ed->name().isEmpty())
		m_servicesList->addItem(QIcon(":/services/images/services.png"), ed->name(), qVariantFromValue((void*)ed));

	int findIndex = m_servicesList->findText(m_serviceName);
	if ((! m_serviceName.isEmpty()) && (findIndex == -1))
	{
		m_servicesList->addItem(QIcon(":/services/images/services.png"), m_serviceName);
		m_servicesList->setItemData(m_servicesList->count() - 1, Qt::gray, Qt::ForegroundRole);
		m_servicesList->setCurrentIndex(m_servicesList->count() - 1);
	}
	else if (findIndex >= 0)
	{
		m_servicesList->setCurrentIndex(findIndex);
	}
	else if (m_servicesList->count() > 0)
		m_servicesList->setCurrentIndex(0);
}

void WebServicesEditor::loadActionsList(int index)
{
	m_actionList->clear();
	if ((index >= 0) && (m_servicesList->itemData(index).isValid()))
	{
		WebServices * ed = (WebServices*)(m_servicesList->itemData(index).value<void*>());
		foreach(Operation * op, ed->operations())
		m_actionList->addItem(QIcon(":/services/images/action.png"), op->name(), qVariantFromValue((void*)op));
	}

	int findIndex = m_actionList->findText(m_operationName);
	if ((! m_operationName.isEmpty()) && (findIndex == -1))
	{
		m_actionList->addItem(QIcon(":/services/images/action.png"), m_operationName);
		m_actionList->setItemData(m_actionList->count() - 1, Qt::gray, Qt::ForegroundRole);
		m_actionList->setCurrentIndex(m_actionList->count() - 1);
	}
	else if (findIndex >= 0)
	{
		m_actionList->setCurrentIndex(findIndex);
	}
	else if (m_actionList->count() > 0)
		m_actionList->setCurrentIndex(0);
}

void WebServicesEditor::loadValuesList(int index)
{
	QString param = m_paramList->currentText();

	m_paramList->clear();
	if ((index >= 0) && (m_actionList->itemData(index).isValid()))
	{
		Operation * op = (Operation*)(m_actionList->itemData(index).value<void*>());
		foreach(Parameter * param, op->inputParam())
		m_paramList->addItem(QIcon(":/services/images/serviceparam.png"), param->paramString());
	}

	foreach(const QString & param, m_paramValues.keys())
	{
		if ((!param.isEmpty()) && m_paramList->findText(param) == -1)
		{
			m_paramList->addItem(QIcon(":/services/images/serviceparam.png"), param);
			m_paramList->setItemData(m_paramList->count() - 1, Qt::gray, Qt::ForegroundRole);
		}
	}

	if (! param.isEmpty())
	{
		int paramIndex = m_paramList->findText(param);
		if (paramIndex != -1)
			m_paramList->setCurrentIndex(paramIndex);
	}
	else if (m_paramList->count() > 0)
		m_paramList->setCurrentIndex(0);

	emit updateActions();
}

void WebServicesEditor::webServicesChanged()
{
	loadServicesList();
	loadActionsList(m_servicesList->currentIndex());
	loadValuesList(m_actionList->currentIndex());
}

void WebServicesEditor::webServicesActivated(int index)
{
	setModified(true);
	loadActionsList(index);
	loadValuesList(m_actionList->currentIndex());
}

void WebServicesEditor::webServicesParamActivated(int index)
{
	setModified(true);
	loadValuesList(index);
}

void WebServicesEditor::webServicesValueActivated()
{
	if (! m_oldParamValue.isEmpty()) store(m_oldParamValue);
	m_oldParamValue = m_paramList->currentText();
	restore(m_oldParamValue);
}

void WebServicesEditor::store(const QString & paramStr)
{
	if (textEdit()->editor()->isContentModified()) setModified(true);
	m_paramValues[ paramStr ] = textEdit()->toPlainText();
}

void WebServicesEditor::restore(const QString & paramStr)
{
	textEdit()->setPlainText(m_paramValues[ paramStr ]);
}

int WebServicesEditor::executionTime() const
{
	return m_executionTime;
}

const QString & WebServicesEditor::faultString() const
{
	return m_faultString;
}

void WebServicesEditor::readResponse()
{
	m_executionTime = m_benchmarkTimer.elapsed();

	m_benchmark->setText(tr("Server has respond in %1 ms").arg(m_executionTime));

	const QtSoapMessage & response = m_http->getResponse();
	if (response.isFault())
	{
		m_faultString = response.faultString().value().toString();
		if (!m_batch)
			QMessageBox::warning(qApp->activeWindow(), tr("WebServices Error"), tr("Web services has error %1").arg(m_faultString));
		m_progressBar->setVisible(false);

		emit operationTerminated();
		return;
	}

	const QtSoapType & res = response.returnValue();
	if (! res.isValid())
	{
		m_faultString = tr("Invalid return value");
		if (!m_batch)
			QMessageBox::warning(qApp->activeWindow(), tr("WebServices Error"), tr("Web services has error %1").arg(m_faultString));
		m_progressBar->setVisible(false);

		emit operationTerminated();
		return;
	}

	QHash<QString,QString> hashResponse;
	if (res.count() > 0)
	{
		QDomDocument document;
		QDomElement rootElement = res.toDomElement(document);
		QDomElement childElement = rootElement.firstChildElement();
		while (! childElement.isNull())
		{
			hashResponse[ childElement.nodeName()] = childElement.text();
			childElement = childElement.nextSiblingElement();
		}
	}
	else
	{
		hashResponse[ res.name().name()] = res.value().toString();
	}

	m_resultValues = hashResponse;
	m_resultList->clear();
	m_resultList->addItems(m_resultValues.keys());
	m_resultEdit->setPlainText(m_resultValues.values().at(0));
	m_progressBar->setVisible(false);

	emit operationTerminated();
}

void WebServicesEditor::runBatch()
{
	run(true);
}

void WebServicesEditor::run(bool batch)
{
	m_batch = batch;

	if(!m_batch)
		m_progressBar->setVisible(true);

	Operation * op = operation();

	m_namespace = op->namespaceString();

	QtSoapMessage request;
	request.setMethod(QtSoapQName(op->name(), m_namespace));

	QHashIterator<QString,QString> i(m_paramValues);
	while (i.hasNext())
	{
		i.next();
		request.addMethodArgument(i.key(), op->namespaceString(), i.value());
	}

	QUrl queryUrl(service()->wsdl().services()[0].port().addressLocation());
	m_http->setHost(queryUrl.host(), queryUrl.port());

	m_benchmarkTimer.start();
	m_http->submitRequest(request, queryUrl.path());
}

