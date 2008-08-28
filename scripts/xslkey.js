var obj = new Object;

obj.text    = "Replace use of xsl:key by full xpath";
obj.author  = "Alexandre Rocher";
obj.licence = "See the author"
obj.version = "0.1"

obj.run = function()
{
	var text = textEdit.selection;

	textEdit.selection = this.formatXslKey(text);
};

obj.formatXslKey = function(text)
{
	// <xsl:value-of select="key('url-param', 'chp:Codmdl')"/>
	while (text.indexOf("key('url-param'") != -1)
	{
		var pos = text.indexOf("key('url-param'");

		// Ce qui suit la virgule d�fini le nom du champ (� trimer) jusqu'� la
		// quote suivante
		var posV1 = text.indexOf(",", pos);
		var posQ1 = text.indexOf("'", posV1);

		var posQ2 = text.indexOf("'", posQ1 + 1);
		var field = text.substring(posQ1 + 1, posQ2);

		// La parenth�se suivante d�fini la position de fin de la cha�ne
		// � remplacer
		var posE = text.indexOf(")", posQ2) + 1;

		text = text.substring(0, pos)
			+ "/layout_data/application_data/temporaire/param[@name='" + field + "']"
			+ text.substring(posE);
	}

	return text;
}

return obj;
