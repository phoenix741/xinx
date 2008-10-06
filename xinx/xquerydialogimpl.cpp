/***************************************************************************
 *   Copyright (C) 2008 by Ulrich Van Den Hekke                            *
 *   ulrich.vdh@free.fr                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

// Xinx header
#include "xquerydialogimpl.h"
#include "xinxpluginsloader.h"

// Qt header
#include <QStringList>
#include <QPushButton>
#include <QtXmlPatterns>

/* XQueryDialogMessageHandler */

class XQueryDialogMessageHandler : public QAbstractMessageHandler {
public:
	XQueryDialogMessageHandler( QObject * parent = 0 ) : QAbstractMessageHandler( parent ) { }
	virtual ~XQueryDialogMessageHandler() { }

	const QStringList & messages() const { return m_messages; }
protected:
	virtual void handleMessage ( QtMsgType type, const QString & description, const QUrl & identifier, const QSourceLocation & sourceLocation ) {
	    const bool hasLine = sourceLocation.line() != -1;

	    switch( type ) {
        case QtWarningMsg:
            if(hasLine) {
                m_messages += tr("Warning in %1, at line %2, column %3: %4").arg(QString::fromLatin1(sourceLocation.uri().toEncoded()),
																				 QString::number(sourceLocation.line()),
                                                                                 QString::number(sourceLocation.column()),
                                                                                 description);
            } else {
                m_messages += tr("Warning in %1: %2").arg(QString::fromLatin1(sourceLocation.uri().toEncoded()),
                                                          description);
            }

            break;
        case QtFatalMsg: {
            Q_ASSERT(!sourceLocation.isNull());
            const QString errorCode(identifier.fragment());
            Q_ASSERT(!errorCode.isEmpty());
            QUrl uri(identifier);
            uri.setFragment(QString());

            QString errorId;
            if(uri.toString() == QLatin1String("http://www.w3.org/2005/xqt-errors"))
                errorId = errorCode;
            else
                errorId = QString::fromLatin1(identifier.toEncoded());

            if(hasLine) {
                m_messages += tr("Error %1 in %2, at line %3, column %4: %5").arg(errorId,
                                                                    QString::fromLatin1(sourceLocation.uri().toEncoded()),
                                                                    QString::number(sourceLocation.line()),
                                                                    QString::number(sourceLocation.column()),
                                                                    description);
            } else {
            	m_messages += tr("Error %1 in %2: %3").arg(errorId,
                                                           QString::fromLatin1(sourceLocation.uri().toEncoded()),
                                                           description);
            }
            break;
        }
        case QtCriticalMsg:
        case QtDebugMsg:
            Q_ASSERT_X(false, Q_FUNC_INFO, "message() is not supposed to receive QtCriticalMsg or QtDebugMsg.");
            return;
		}
	};
private:
    QStringList m_messages;
};

/* XQueryDialogImpl */

XQueryDialogImpl::XQueryDialogImpl( QWidget * parent, Qt::WindowFlags f ) : QDialog( parent, f ) {
	setupUi( this );

	XinxPluginsLoader::self()->createHighlighter( XinxPluginsLoader::self()->highlighterOfSuffix( "xml" ), m_resultTextEdit->document() );

	QPushButton * evaluateBtn = new QPushButton(tr("&Evaluate"));
	evaluateBtn->setDefault( true );
	m_buttonBox->addButton( evaluateBtn, QDialogButtonBox::ActionRole );
	connect( evaluateBtn, SIGNAL(clicked()), this, SLOT(evaluate()) );
}

XQueryDialogImpl::~XQueryDialogImpl() {

}

void XQueryDialogImpl::setFileName( const QString & filename ) {
	if( m_fileName != filename )
		m_fileName = filename;
}

void XQueryDialogImpl::setCurrentXPath( const QString & currentXPath ) {
	m_xpathTextEdit->setText( currentXPath );
}

void XQueryDialogImpl::evaluate() {
	// Open the file to read it
	QFile sourceDocument;
	sourceDocument.setFileName( m_fileName );
	if(! sourceDocument.open( QIODevice::ReadOnly ) ) {
		qWarning( qPrintable( tr("Error while opening presentation file : %1").arg( sourceDocument.errorString() ) ) );
		return;
	}

	// Get the XPath
	QString xpath = m_queryTextEdit->toPlainText();
	if( xpath.isEmpty() ) return;
	xpath = "doc($d)" + m_xpathTextEdit->text() + "/\n(" + xpath + ")";

	// Prepare the query
	QBuffer result;
	result.open( QIODevice::ReadWrite );
	XQueryDialogMessageHandler handler;
	QXmlQuery query;
	query.setMessageHandler( &handler );
	QXmlFormatter serializer( query, &result );

	// Execute the query
	query.bindVariable( "d", &sourceDocument );
	query.setQuery( xpath );

	query.evaluateTo( &serializer );

	m_resultTextEdit->setPlainText( result.data() );
	m_resultTextEdit->appendHtml( handler.messages().join("\n") );
}
