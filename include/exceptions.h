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

// Qt header
#include <QStringList>
#include <QHash>

class XinxException {
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

class XinxAssertException : public XinxException {
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

class Trace {
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

extern QHash<unsigned long,QStringList> stackTrace;


#endif /* __EXCEPTIONS_H__ */
