var obj = new Object;

obj.text = "&Replace left space";

obj.run = function() {
	var text = this.textEdit.plainText;
	var result = "";
	
	var nbSpace = input( "Number of space to be replaced", 4 );
	var start = true;
	var spaces = 0;

	for( i = 0; i < text.length; i++ ) {
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
	    	for( j = 0; j < spaces ; j++ ) 
				result += ' ';
			spaces = 0;
			start = false;
			result += text[i];
		}
	}
	
	this.textEdit.plainText = result;
	this.textEdit.document.modified = true;
};

return obj;