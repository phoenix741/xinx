/***************************************************************************
 *   Copyright (C) 2007 by Ulrich Van Den Hekke                            *
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

#ifndef __EXCEPTIONS_H__
#define __EXCEPTIONS_H__

// Xinx header
#include "xinxcore.h"

// Qt header
#include <QStringList>
#include <QHash>

class QErrorMessage;
class QWidget;
class XinxErrorMessage;

/*!
 * Base class for exceptions in XINX. This base class include a message in the exception, and save 
 * the stack.
 */
class XINX_EXPORT XinxException {
public:
	XinxException( QString message );
	/*!
	 * Return the message.
	 * \return The message of the error.
	 */
	const QString & getMessage() const;
	/*!
	 * Return the stack trace.
	 * \return The stack trace
	 */
	const QStringList & getStack() const;
private:
	QString m_message;
	QStringList m_stack;
};

/*!
 * Exception called by the XINX_ASSERT. This assert can now be replace by Q_ASSERT.
 * If QT_NO_DEBUG is defined, this exception is never call.
 */
class XINX_EXPORT XinxAssertException : public XinxException {
public:
	XinxAssertException( const char *assertion, const char *file, int line );
};

#if !defined( XINX_ASSERT )
#  ifndef QT_NO_DEBUG
#    define XINX_ASSERT(cond) do{if(!(cond)) throw XinxAssertException(#cond,__FILE__,__LINE__);} while (0)
#  else
#    define XINX_ASSERT(cond) do{}while(0)
#  endif
#endif

/*!
 * The Exception manager contains an error dialog that can be used when a error occure or when we 
 * use qWarning, qCritical, qError, qDebug. When an fatal error occure, the signal \e errorTriggered
 * is emited.
 */
class XINX_EXPORT ExceptionManager : public QObject {
	Q_OBJECT
public:
	virtual ~ExceptionManager();
	
	/*! 
	* \obsolete
	 * Xinx stack trace is construct by XINX_TRACE macro. This macro make the application
	 * so slow and this stack is desactivated in release mode (and often in debug mode).
	 */
	QHash<unsigned long,QStringList> & xinxStackTrace();
	/*!
	 * Stack trace generated by the Operating System. The application must be compiled with
	 * -rdynamic in Gnu/Linux. This stack is unused on Windows.
	 */
	QStringList stackTrace() const;
	
	/*!
	 * The dialog created and used to show message. This dialog is automatically called when Qt
	 * macro (qDebug, ...) is used.
	 */
	QErrorMessage * errorDialog() const;

	/*!
	 * If set to \e true the dialog is not only showed but executed too. The message is show to user
	 * before the application is sopped. 
	 * Fatal can be set when an assert failed, or when a signal fault is called.
	 */
	void setFatal( bool value );
	/*!
	 * Return true if the error is fatal.
	 */
	bool fatal() const;

	/*!
	 * Return the exception manager of the application and created it if needed.
	 */
	static ExceptionManager * self();

	/*!
	 *Show a dialog and save the error in a trace file.
	 * \param error Message to store in the file
	 * \param stack Stack to store when the error occure.
	 */
	void notifyError( QString error, QStringList stack = QStringList() );
signals:
	/*!
	 * Signal emited when \e notifyError is called and the exception manager is set to be fatal.
	 */
	void errorTriggered();
private:
	ExceptionManager();
	
	static ExceptionManager * s_self;
	QHash<unsigned long,QStringList> m_stackTrace;
	QErrorMessage * m_dialog;
	bool m_fatal;
};

/*!
 * \obsolete
 * Create a stack trace
 * This class is usefull but too slow.
 */
class XINX_EXPORT Trace {
public:
	explicit Trace( char* filename, int line, const QString& func_name, const QString & args );	
	~Trace();
private:
	void LogMsg( int depth, const char * filename, int line, const QString & fonction );
	
	unsigned long m_handle;
	
	static QHash<unsigned long,int> m_depth;
};

//#define XINX_DEBUG
// Attention : Trop lent
#ifdef XINX_DEBUG
#  define XINX_TRACE( fct, args ) Trace __XINX_TRACE__ ( __FILE__, __LINE__, fct, args )
#else
#  define XINX_TRACE( fct, args ) 
#endif

#endif /* __EXCEPTIONS_H__ */
