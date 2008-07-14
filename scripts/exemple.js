var obj = new Object;

obj.text = "Just an example";

obj.run = function() {
	if( this.project != undefined )
		alert( this.project.projectName );

	alert( "This is exemple show you how use alert, confirm and input dialog." );

	alert( confirm( "Click yes to return true, else click no." ) );

	alert( input( "Write a text" ) );
	alert( input( "Write a number", 4 ) );
	alert( input( "Write a text the default is 'toto'", "toto" ) );
};

return obj;