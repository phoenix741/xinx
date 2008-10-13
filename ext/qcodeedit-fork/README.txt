
----------------------------------------------------------------------------------
----------------------------------------------------------------------------------
  
        QCodeEdit, copyright (c) 2006-2008 Luc Bruant aka fullmetalcoder,
                  is a free and open source software
  
  QCodeEdit sources are part of Edyuk and thus GNU General Public License (GPL)
  version 2, as published by the Free Software Fondation, applies to them.

----------------------------------------------------------------------------------
----------------------------------------------------------------------------------

QCodeEdit is a project aiming at the creation of a flexible and powerful text
editing framework for Qt4. It has started as a sub-project of Edyuk, copyright
(c) Luc Bruant, a free and open source IDE. As it proved successful it is also
released as a standalone library to make it easy for everyone to use such
a framework within other apps without having to reinvent the wheel.

Writing closed source applications with QCodeEdit is possible after buying a
proper license. For more informations about pricing and licensing conditions
please contact the author directly <fullmetalcoder@hotmail.fr>.

QCodeEdit depends on Qt 4.3, copyright (c) Trolltech AS , which can be downloaded at :
ftp://ftp.trolltech.com/qt/sources

More information about Qt and Trolltech :
http://www.trolltech.com

Hoping you'll like it.


IMPORTANT : If you encounter any sort of troubles trying to build or run QCodeEdit
please send a bug report with as many details as possible (including but not limited
to : OS, Qt version, compiler, QCodeEdit version, compile log, backtrace, ...) to the
team using either of these :
	* edyuk-devel@lists.sf.net : Edyuk devel mailing list
	* Edyuk task tracker on Sf.net : http://sf.net/projects/edyuk
	* Edyuk webissues server (needs a WebIssues client) : http://edyuk.org/webissues/
		- login		: anonymous
		- password	: anonymous
	* QtCentre dedicated thread in Qt Software section : http://www.qtcentre.org/forum
	* direct mail to fullmetalcoder@hotmail.fr


In case you don't understand, blocks of text enclosed between lines of minus signs are
shell commands. The dollar signs just stand for command prompt.


 >>> Building :

------------------------------------------------------------------------------------------
$ qmake
$ make
------------------------------------------------------------------------------------------

This will build the library and the example. You may want to alter the build mode to force
either debug or release. If so just pass the mode you want to make, e.g. :

------------------------------------------------------------------------------------------
$ make release
------------------------------------------------------------------------------------------
or
------------------------------------------------------------------------------------------
$ make debug
------------------------------------------------------------------------------------------

Finally, with admins rights/root privilegdes, you can install QCodeEdit so as to be able
to use it simply :

------------------------------------------------------------------------------------------
$ qmake
$ make install
------------------------------------------------------------------------------------------

NB : the extra "qmake" is NEEDED to ensure that binaries will be located and copied properly.
NB 2 : Only the "make install" command requires root priviledges, "qmake" can and should
always be run as normal user.
NB 3 : Apart from libs and headers, QCodeEdit also provides a .prf file which makes it
easier to use the lib in another project. Under non-Unix platforms it is recommended to
copy the files manually (and possibly edit them to fit your needs).


 >>> Using within an app :

To have one of your app building with QCodeEdit just add the following line to your project
file (this won't work if you have not installed QCodeEdit as described above....) :

CONFIG += qcodeedit

Then, add proper headers in your sources and start coding. :D


 >>> Testing :

A very basic example is provided which open a list of files passed as parameters
and try to highlight them according to their file extension. Only a few language
definitions are provided :
  * C++ (with Doxygen support)
  * PHP
  * XML
  * Doxygen alone (for .dox files)
  * QMake project files
  * Python (experimental)
  * C# (experimental)

Feel free to add other ;)

------------------------------------------------------------------------------------------
$ example/example [file]
------------------------------------------------------------------------------------------

Note : on Unix system it is recommended to use the script due to the need of setting
the LD_LIBRARY_PATH variable properly :

------------------------------------------------------------------------------------------
$ ./example.sh [file]
------------------------------------------------------------------------------------------

NB : [file] stands for a filename. If omitted a minimal string will be loaded and
considered as C++ source code


 >>> Documentation [needs Doxygen : http://www.doxygen.org] :

------------------------------------------------------------------------------------------
$ doxygen
------------------------------------------------------------------------------------------

NB :
	* This will create the documentation in the doc folder. Just open doc/html/index.html
	* These are only API docs (which lack completeness ATM...)


 >>> Fetching bleeding edge sources [needs Subversion : http://subversion.tigris.org] :

------------------------------------------------------------------------------------------
$ svn co http://edyuk.svn.sf.net/svnroot/edyuk/trunk/3rdparty/qcodeedit2
------------------------------------------------------------------------------------------

NB : Using a graphical client this command extends to a "checkout" action using the above
repository URL.

