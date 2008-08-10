var obj = new Object;

obj.text = "Replace &accolade by xsl:value";
obj.author = "Ulrich Van Den Hekke";

obj.run = function() {
	var text = textEdit.selection;
	var result = "";

	result = text.replace("{","<xsl:value-of select=\"");
	result = result.replace("}","\"/>");

	textEdit.selection = result;
	textEdit.document.modified = true;
};

return obj;