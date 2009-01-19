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
 
var obj = new Object;

obj.text    = "Just an example";
obj.author  = "Ulrich Van Den Hekke";
obj.licence = "GPL v3 or later"
obj.version = "0.1"

obj.run = function() {
	if( project != undefined )
		alert( project.projectName );

	if( configuration != undefined ) {
		var c = configuration.configurations[ 0 ];
		alert( c.filename );
		alert( c.version.toString );
	}

	if( editor != undefined )
		alert( editor.filename + ' or ' + editor.filename.substring( editor.filename.lastIndexOf('/') + 1 ) );

	alert( "This is exemple show you how use alert, confirm and input dialog." );

	alert( confirm( "Click yes to return true, else click no." ) );

	alert( input( "Write a text" ) );
	alert( input( "Write a number", 4 ) );
	alert( input( "Write a text the default is 'toto'", "toto" ) );
};

return obj;