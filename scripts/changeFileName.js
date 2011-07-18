var obj = new Object;

obj.text    = "Change the &name of the file with the current one";
obj.author  = "Ulrich VANDENHEKKE";
obj.licence = "GPL v3 or later"
obj.version = "0.1"

obj.beforeSave = function() {
	this.run();
}

obj.run = function()
{
	var search = new DocumentSearch( textEdit );
	var filename;
	var index = editor.filename.lastIndexOf('/');
	if (index == -1)
	{
		index = editor.filename.lastIndexOf('\\');
	}
	
	filename = editor.filename.substring( index + 1 );

	search.options.regExp = true;
	search.searchText = "(\\$RCSfile:\\s+).*(,v\\s+\\$)";
	search.replaceText = "\\1" + filename + "\\2";

	while( search.next() ) {}
	/*
	search.searchText = "(<xsl:variable name=\"CTXTRD\">).*(</xsl:variable>)";
	search.replaceText = "\\1" + filename + "\\2";

	while( search.next() ) {}
*/
};

obj;
