/***************************************************************************
 *   Copyright (C) 2006 by Ulrich Van Den Hekke                            *
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

#ifndef __XINXCORE_H__
#define __XINXCORE_H__

/*! \mainpage XINX Library
 *
 * \section preface Preface
 *
 * This document explain how to use the XINX Library to create a plugins. This document can be used
 * to known how XINX work, in part, too.
 *
 * You can find in the menu, documented class present in the libxinxcmp and libsharedxinx. The first
 * library contains components, the second, contains shared object beetween XINX and plugins. Source of
 * existing plugins can be use too, as documentation.
 *
 * Plugins is used to extend XINX in different way.
 *  - Add a revision control system
 *  - Add Syntax Highlighter on file editor
 *  - Add new editor
 *
 * \section pluginbase Base of plugin
 *
 * A plugins must implements at least one interface : \e IXinxPlugin. This interface is used to know
 * informations about the plugins. A plugin that implement only this interface do nothing but is
 * regocgnize by XINX.
 *
 * To implemente the interface you must write some code like this :
 * \code
 *
 * class FooPlugin : public QObject, public IXinxPlugin {
 *	Q_OBJECT
 *	Q_INTERFACES(IXinxPlugin)
 * public:
 * 	FooPlugin();
 *	virtual ~FooPlugin();
 *
 *	virtual bool initializePlugin( const QString & lang );
 *	virtual QVariant getPluginAttribute( const enum IXinxPlugin::PluginAttribute & attr );
 * };
 * \endcode
 *
 * The implementation of this class can be :
 *
 * \code
 * FooPlugin::FooPlugin() {
 *	// Make some initialization
 * }
 *
 * FooPlugin::~FooPlugin() {
 *	// Make some cleanup
 * }
 *
 * bool FooPlugin::initializePlugin( const QString & lang ) {
 *	// Make some initialization about the localization
 *	return true;
 * }

 * QVariant FooPlugin::getPluginAttribute( const enum IXinxPlugin::PluginAttribute & attr ) {
 *	switch( attr ) {
 *	case PLG_NAME:
 *		return tr("A foo plugins");
 *	case PLG_DESCRIPTION:
 *		return tr("Just a foo plugins that do nothing.");
 *	case PLG_AUTHOR:
 *		return "Ulrich Van Den Hekke";
 *	case PLG_VERSION:
 *		return "0.1";
 *	case PLG_LICENCE:
 *		return "GPL v2.0 or later";
 *	}
 *	return QVariant();
 * }
 * Q_EXPORT_PLUGIN2(fooplugin, FooPlugin)
 * \endcode
 *
 * The plugins must inherits from the \e QObject class (see Qt documentation), and must call the \e Q_OBJECT macro.
 * He can inherits \e IXinxPlugin indirectly (some interface as \e IRCSPlugin inherits from \e IXinxPlugin).
 *
 * The plugin must call the \e Q_EXPORT_PLUGIN2 macro in the implementation. This create a static variable for the
 * plugin.
 *
 * If the plugin need to call new tools, you can reimplement \e IXinxPlugin::pluginTools() and returns the list of
 * tool needed by your plugin with the default value.
 *
 * You can also reimplement \e IXinxPlugin::initializeProject() and \e IXinxPlugin::destroyProject() if you have
 * specifique code when the current project is opened or destroyed.
 *
 * A plugin can propose a configuration dialog throw the \e IXinxPluginConfiguration interface.
 *
 * \section rcsplugin The Revision Control System Plugin
 *
 * A RCS Plugin must implemente the IRCSPlugin interface.
 *
 * \code
 * class FooPlugin : public QObject, public IRCSPlugin {
 *	Q_OBJECT
 *	Q_INTERFACES(IXinxPlugin)
 *	Q_INTERFACES(IRCSPlugin)
 * public:
 * 	FooPlugin();
 *	virtual ~FooPlugin();
 *
 *	virtual bool initializePlugin( const QString & lang );
 *	virtual QVariant getPluginAttribute( const enum IXinxPlugin::PluginAttribute & attr );
 *
 *	virtual QStringList rcs();
 *	virtual QString descriptionOfRCS( const QString & rcs );
 *	virtual RCS * createRCS( const QString & rcs, const QString & basePath );
 * };
 * \endcode
 *
 * In the implementation method create a derivated object of RCS
 *
 * \code
 * QStringList FooPlugin::rcs() {
 * 	return QStringList() << "foo";
 * }
 *
 * QString FooPlugin::descriptionOfRCS( const QString & rcs ) {
 * 	if( rcs.toLower() == "foo" )
 *		return tr( "Foo - Foo Version System" );
 *	return QString();
 * }

 * RCS * FooPlugin::createRCS( const QString & rcs, const QString & basePath ) {
 *	if( rcs.toLower() == "foo" ) {
 *		return new RCS_Foo( basePath );
 *	}
 *	return NULL;
 * }
 * \endcode
 *
 * Next this is the RCS_Foo object that will be used in the application to \e update,
 * \e commit, \e remove and \e add.
 *
 * \section projectconfiguration Project configuration plugins
 *
 * We can configure a plugin throw the \e IXinxPluginConfiguration interface, if the
 * configuration is global, or throw the \e IXinxPluginProjectConfiguration interface,
 * if the configuration depends on the project.
 *
 * Methods \e IXinxPluginConfiguration::createSettingsDialog() and \e IXinxPluginProjectConfiguration::createProjectSettingsPage()
 * return a widget thar will be include in the project configuration box, or in the configuration dialog.
 *
 * Settings are load and saved with methods \e loadSettingsDialog(), \e loadProjectSettingsPage(),
 * \e saveSettingsDialog(), and \e saveProjectSettingsPage().
 *
 * If you want to add page in the new project wizard dialog, the plugin IXinxPluginProjectConfiguration
 * give you too, two method :
 *  * \e createNewProjectSettingsPages()
 *  * \e saveNewProjectSettingsPage()
 *
 * \section neweditor Creation of a new editor
 *
 * It's possible to create a new editor with the interface \e IFileTypePlugin and
 * \e IFilePlugin
 *
 */


#include <QObject>
#include <QObjectCleanupHandler>

/*!
 * All static class add self to the static deleter who delete all static class when the application is stopped
 * (if necessary).
 */
class  XINXStaticDeleter : public QObjectCleanupHandler {
	Q_OBJECT
public:
	/*! Intialize default value of object. */
	XINXStaticDeleter();
	/*! Destruct the global object. */
	~XINXStaticDeleter();

	/*!
	 * \deprecated
	 * Add an object to the static deleter class. This function is replaced by
	 * the add() method of the QObjectCleanupHandler.
	 */
	void addObject( QObject * object ) { add( object ); };

	/*!
	 * Retrieve the global object created at the first call of the this method.
	 */
	static XINXStaticDeleter * self();
private:
	static XINXStaticDeleter * s_self;
};

/*!
 * Numbre of maximum recent file that can be opened
 */
#define MAXRECENTFILES 10

/*!
 * Default project options filtre
 */
#define DEFAULT_PROJECT_FILTRE_OPTIONS QDir::AllDirs | QDir::Files | QDir::Readable | QDir::NoDotAndDotDot

/*!
 * Template used to delete all the object of list with an iterator. Instead of using C++
 * delete, this method called the deleteLater method of the QObject class, if the object
 * is a QObject.
 * Objects are delete when application process messages.
 * \param begin First iterator to delete
 * \param end Last iterator to delete
 * \sa qDeleteAllLater(const Container &c)
 */
template <typename ForwardIterator>
void qDeleteAllLater(ForwardIterator begin, ForwardIterator end) {
	while (begin != end) {
		if( dynamic_cast<QObject*>( *begin ) )
			dynamic_cast<QObject*>( *begin )->deleteLater();
		else
			delete *begin;
		++begin;
	}
}

/*!
 * Delete all the object of a list later
 * \param c Container where we must delete all object.
 * \sa qDeleteAllLater(ForwardIterator begin, ForwardIterator end)
 */
template <typename Container>
inline void qDeleteAllLater(const Container &c) {
	qDeleteAllLater(c.begin(), c.end());
}


#endif // __XINXCORE_H__
