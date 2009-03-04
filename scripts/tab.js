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

obj.text    = "&Replace left space";
obj.author  = "Ulrich Van Den Hekke";
obj.licence = "GPL v3 or later"
obj.version = "0.1"

obj.run = function() {
	var text = textEdit.selection;
	var result = "";

	var nbSpace = input( "Number of space to be replaced", 4 );
	var start = true;
	var spaces = 0;

	for( var i = 0; i < text.length; i++ ) {
		if( start && (text[i] == ' ') ) {
			spaces++;
			if( spaces == 4 ) {
				result += '\t';
				spaces = 0;
			}
		} else if( text[i] == '\n' ) {
			start = true;
			result += text[i];
		} else {
			for( var j = 0; j < spaces ; j++ )
				result += ' ';
			spaces = 0;
			start = false;
			result += text[i];
		}
	}

	textEdit.selection = result;
};

obj;