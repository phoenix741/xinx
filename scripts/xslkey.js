var obj = new Object; 

obj.text = "Replace use of xsl:key by full xpath"; 
obj.author = "Alexandre Rocher";

obj.run = function()
{ 
	var text = this.textEdit.plainText; 

	this.textEdit.plainText = obj.formatXslKey(text); 
	this.textEdit.document.modified = true; 
}; 

obj.formatXslKey = function(text)
{ 
	// <xsl:value-of select="key('url-param', 'chp:Codmdl')/@value"/> 
	while (text.indexOf("key('url-param'") != -1) 
	{ 
		var pos = text.indexOf("key('url-param'"); 

		// Ce qui suit la virgule défini le nom du champ (à trimer) jusqu'à la 
		// quote suivante 
		var posV1 = text.indexOf(",", pos); 
		var posQ1 = text.indexOf("'", posV1); 

		var posQ2 = text.indexOf("'", posQ1 + 1); 
		var field = text.substring(posQ1 + 1, posQ2); 

		// Le guillemet suivant défini la position de fin de la chaîne 
		// à remplacer 
		var posE = text.indexOf("\"", posQ2); 

		text = text.substring(0, pos) 
				+ "/layout_data/application_data/temporaire/param[@name='" + field + "']/@value" 
				+ text.substring(posE); 
	}

	return text; 
} 

return obj; 
