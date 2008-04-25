var obj = new Object;

obj.text = "Just an example";

obj.run = function() {
	alert( confirm( "Oui ?" ) );
	
	alert( input( "test" ) );
	alert( input( "test n", 4 ) );
	alert( input( "test s", "toto" ) );
};

return obj;