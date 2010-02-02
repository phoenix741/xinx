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
#include "contentview2cache.h"
#include "project/xinxproject.h"
#include "plugins/xinxpluginsloader.h"

// Qt header
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QFileInfo>
#include <QDateTime>
#include <QDebug>

namespace ContentView2 {

/* Cache */

Cache::Cache( XinxProject * project ) : m_project( project ) {
	m_watcher = new QFileSystemWatcher( this );
	connect( m_watcher, SIGNAL(fileChanged(QString)), this, SLOT(refreshCache(QString)) );
}

Cache::~Cache() {
}

QStringList Cache::contentsViewLoaded() const {
	return m_watcher->files();
}

void Cache::initializeCache() {
	QSqlQuery cacheQuery( "SELECT id, path, datmod, root_id, loaded FROM cv_file WHERE cached = :cached", XINXProjectManager::self()->session()->database() );
	cacheQuery.bindValue( ":cached", true );
	bool result = cacheQuery.exec();
	Q_ASSERT_X( result, "Cache::initializeCache", qPrintable( cacheQuery.lastError().text() ) );

	while( cacheQuery.next() ) {
		const uint      id           = cacheQuery.value( 0 ).toUInt();
		const QString   path         = cacheQuery.value( 1 ).toString();
		const QString   datmod       = cacheQuery.value( 2 ).toString();
		const uint      nodeId       = cacheQuery.value( 3 ).toUInt();
		const bool      loaded       = cacheQuery.value( 4 ).toBool();
		const QDateTime lastModified = QDateTime::fromString( QFileInfo( path ).lastModified().toString( Qt::ISODate ), Qt::ISODate );
		const QDateTime cacheDate    = QDateTime::fromString( datmod, Qt::ISODate );

		if( ( lastModified > cacheDate ) || !loaded ) {
			Parser * parser = 0;
			try {
				parser = createParser( path, false );
				parser->setFilename( path );
				parser->setRootNode( Node( XINXProjectManager::self()->session()->database(), nodeId ) );
				changeDatmod( XINXProjectManager::self()->session()->database(), id, lastModified );
				m_parsers.append( parser );
				m_watcher->addPath( path );
			} catch( ParserException e ) {
				delete parser;
			}
		}
	}

	start( QThread::IdlePriority );
}

int Cache::createRootId( const QString & filename, bool get, bool cached ) {
	QSqlQuery selectQuery( "SELECT root_id FROM cv_file WHERE path=:filename", XINXProjectManager::self()->session()->database() );
	selectQuery.bindValue( ":filename", filename );
	bool result = selectQuery.exec();
	Q_ASSERT_X( result, "Cache::createRootId", qPrintable( selectQuery.lastError().text() ) );

	if( selectQuery.first() ) {
		if( ! get ) return -1;
		return selectQuery.value(0).toUInt();
	}

	QSqlQuery createQuery( "INSERT INTO cv_file( path, datmod, root_id, loaded, cached ) VALUES( :path, :datmod, -1, :loaded, :cached )", XINXProjectManager::self()->session()->database() );
	createQuery.bindValue( ":path", filename );
	createQuery.bindValue( ":datmod", QFileInfo( filename ).lastModified().toString( Qt::ISODate ) );
	createQuery.bindValue( ":loaded", false );
	createQuery.bindValue( ":cached", cached );
	result = createQuery.exec();
	Q_ASSERT_X( result, "Cache::createRootId", qPrintable( createQuery.lastError().text() ) );

	const uint fileId = createQuery.lastInsertId().toUInt();

	Node node;
	node.setFileName( filename );
	node.setData( filename, Node::NODE_DISPLAY_NAME );
	node.setData( filename, Node::NODE_NAME );
	node.setData( "RootFilename", Node::NODE_TYPE );

	const uint rootId = node.create( XINXProjectManager::self()->session()->database() );

	QSqlQuery updateQuery( "UPDATE cv_file set root_id=:rootId WHERE id=:id", XINXProjectManager::self()->session()->database() );
	updateQuery.bindValue( ":rootId", rootId );
	updateQuery.bindValue( ":id", fileId );
	result = updateQuery.exec();
	Q_ASSERT_X( result, "Cache::createRootId", qPrintable( updateQuery.lastError().text() ) );

	return rootId;
}

void Cache::changeDatmod( QSqlDatabase db, uint fileId, const QDateTime & datmod ) {
	QSqlQuery updateQuery( "UPDATE cv_file SET datmod=:datmod, loaded=:loaded WHERE id=:fileId", db );
	updateQuery.bindValue( ":datmod", QVariant::fromValue( datmod.toString( Qt::ISODate ) ) );
	updateQuery.bindValue( ":loaded", false );
	updateQuery.bindValue( ":fileId", QVariant::fromValue( fileId ) );
	bool result = updateQuery.exec();
	Q_ASSERT_X( result, "Cache::markAsLoaded", qPrintable( updateQuery.lastError().text() ) );
}

void Cache::markAsLoaded( QSqlDatabase db, uint rootId ) {
	QSqlQuery updateQuery( "UPDATE cv_file SET loaded=:loaded WHERE root_id=:rootId", db );
	updateQuery.bindValue( ":loaded", QVariant::fromValue( true ) );
	updateQuery.bindValue( ":rootId", QVariant::fromValue( rootId ) );
	bool result = updateQuery.exec();
	Q_ASSERT_X( result, "Cache::markAsLoaded", qPrintable( updateQuery.lastError().text() ) );
}

void Cache::loadCache( const QStringList & filenames ) {
	QSqlDatabase db = XINXProjectManager::self()->session()->database();
	foreach( const QString & filename, filenames ) {
		if( ! QFileInfo( filename ).exists() ) continue;

		db.transaction();

		int rootId = createRootId( filename, false, true );
		if( rootId == -1 ) continue;

		Parser * parser = 0;
		try {
			parser = createParser( filename, false );
			if( parser ) {
				parser->setFilename( filename );
				parser->setRootNode( Node( db, rootId ) );

				m_parsers.append( parser );
				m_watcher->addPath( filename );

				db.commit();
			} else {
				db.rollback();
			}
		} catch( ParserException e ) {
			delete parser;
			db.rollback();
		}

	}
	start( QThread::IdlePriority );
}

void Cache::addToCache( Parser * parser ) {
	if( parser )
		m_parsers.append( parser );
	start( QThread::IdlePriority );
}

void Cache::run() {
	{
		QSqlDatabase db = QSqlDatabase::cloneDatabase( XINXProjectManager::self()->session()->database(), "CONTENTVIEW_THREAD" );
		bool result =  db.open();
		Q_ASSERT_X( result, "Cache::run", "Can't load session file in the cache for update" );
		// To be quick
		db.exec( "PRAGMA synchronous = OFF" );


		while( m_parsers.size() ) {
			QList<Parser*> parsers;
			while( m_parsers.size() )
				parsers.append( m_parsers.dequeue() );

			emit progressRangeChanged( 0, parsers.size() );
			emit progressValueChanged( 0 );

			for( int i = 0 ; i < parsers.size() ; i++ ) {
				db.transaction();
				Parser * parser = parsers.at( i );
				try {
					uint rootId = parser->rootNode().nodeId();

					parser->setDatabase( db );
					parser->load();
					parser->setDatabase( QSqlDatabase() );

					markAsLoaded( db, rootId );
					db.commit();

					QSqlQuery filesQuery( "SELECT id, path FROM cv_file WHERE root_id=:root_id", db );
					filesQuery.bindValue( ":root_id", rootId );
					bool result = filesQuery.exec();
					Q_ASSERT_X( result, "Cache::run", qPrintable( filesQuery.lastError().text() ) );

					while( filesQuery.next() ) {
						emit cacheLoaded( filesQuery.value( 0 ).toUInt(), filesQuery.value( 1 ).toString() );
					}
				} catch( ContentView2::ParserException e ) {
					// Tant que le parseur Javascript n'est pas au norme on ne n'affiche pas les erreurs des imports �  l'utilisateur
					qDebug( qPrintable( e.getMessage() ) );
					db.rollback();
				}

				if( parser && ! parser->isPersistent() ) {
					delete parser;
				}

				emit progressValueChanged( i );
			}
		}
		emit cacheLoaded();
	}

	QSqlDatabase::database( "CONTENTVIEW_THREAD" ).close();
	QSqlDatabase::removeDatabase( "CONTENTVIEW_THREAD" );
}

void Cache::refreshCache( const QString & filename ) {
	if( QFileInfo( filename ).exists()  ) {
		QSqlQuery selectQuery( "SELECT id, root_id FROM cv_file WHERE path = :path", XINXProjectManager::self()->session()->database() );
		selectQuery.bindValue( ":path", filename );
		bool result = selectQuery.exec();
		Q_ASSERT_X( result, "Cache::fileChanged", qPrintable( selectQuery.lastError().text() ) );

		if( ! selectQuery.first() ) {
			qWarning() << tr( "File %1 not cached" ).arg( filename );
			return;
		}

		XINXProjectManager::self()->session()->database().transaction();

		const uint     id = selectQuery.value( 0 ).toUInt();
		const uint nodeId = selectQuery.value( 1 ).toUInt();

		Parser * parser = 0;
		try {
			parser = createParser( filename, false );
			parser->setFilename( filename );
			parser->setRootNode( Node( XINXProjectManager::self()->session()->database(), nodeId ) );

			changeDatmod( XINXProjectManager::self()->session()->database(), id, QFileInfo( filename ).lastModified() );

			addToCache( parser );
			XINXProjectManager::self()->session()->database().commit();
		} catch( ParserException e ) {
			XINXProjectManager::self()->session()->database().rollback();
			delete parser;
		}
	} else {
		destroyCache( filename );
	}
}

void Cache::destroyCache( const QString & filename ) {
	QSqlQuery selectQuery( "SELECT id, root_id FROM cv_file WHERE path = :path", XINXProjectManager::self()->session()->database() );
	selectQuery.bindValue( ":path", filename );
	bool result = selectQuery.exec();
	Q_ASSERT( result );

	XINXProjectManager::self()->session()->database().transaction();

	while( selectQuery.next() ) {
		const uint id     = selectQuery.value( 0 ).toUInt();
		const uint rootId = selectQuery.value( 1 ).toUInt();

		Node node( XINXProjectManager::self()->session()->database(), rootId );
		node.destroy( XINXProjectManager::self()->session()->database() );

		QSqlQuery removeQuery( "DELETE FROM cv_file WHERE id = :id", XINXProjectManager::self()->session()->database() );
		removeQuery.bindValue( ":id", id );
		bool result = removeQuery.exec();
		Q_ASSERT( result );
	}

	XINXProjectManager::self()->session()->database().commit();
}

Parser * Cache::createParser( const QString & filename, bool persistent ) {
	if( ! QFileInfo( filename ).exists() ) return 0;

	IFileTypePlugin * fileType = XinxPluginsLoader::self()->matchedFileType( filename );
	Parser * parser = 0;
	if( fileType && ( parser = fileType->createParser2() ) ) {
		parser->setPersistent( persistent );
		return parser;
	}
	return 0;
}

}
