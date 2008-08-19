var obj = new Object;

obj.text = "Just an example";

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