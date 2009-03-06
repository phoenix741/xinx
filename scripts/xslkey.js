var obj = new Object;

obj.text    = "Replace use of xsl:key by full xpath";
obj.author  = "Alexandre Rocher";
obj.licence = "See the author"
obj.version = "0.1"

obj.beforeSave = function() {
	this.run();
}

obj.run = function()
{
	var search = new DocumentSearch( textEdit );

	search.options.regExp = true;
	search.searchText = "key\\('url-param', '(.*)'\\)";
	search.replaceText = "/layout_data/application_data/temporaire/param[@name='\\1']";

	while( search.next() ) {}
};

obj;
