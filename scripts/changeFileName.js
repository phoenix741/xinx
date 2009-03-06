var obj = new Object;

obj.text    = "Change the &name of the file with the current one";
obj.author  = "Ulrich VANDENHEKKE";
obj.licence = "GPL v3 or later"
obj.version = "0.1"

/*obj.beforeSave = function() {
	this.run();
}*/

obj.run = function()
{
	var search = new DocumentSearch( textEdit );
	var text, filename;
	alert( editor.filename );
	filename = editor.filename.substring( editor.filename.lastIndexOf('/') + 1 );

	search.options.regExp = true;

	while( (text = search.searchText( "(\\$RCSfile:\\s+).*(,v\\s+\\$)" )).length > 0 ) {
		alert( text );
		search.replaceSelection( "\\1" + filename + "\\2" )
	}
	/*

	var text = textEdit.plainText;
	var result = "";

	result = text.replace(/(\$RCSfile:\s+).*(,v\s+\$)/g,"$1" + editor.filename.substring( editor.filename.lastIndexOf('/') + 1 ) + "$2");

	textEdit.plainText = result;
	textEdit.modified = true;
	*/
};

obj;
