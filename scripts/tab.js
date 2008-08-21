var obj = new Object;

obj.text    = "&Replace left space";
obj.author  = "Ulrich Van Den Hekke";
obj.licence = "GPL v2 or later"
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
	textEdit.document.modified = true;
};

return obj;