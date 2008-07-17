var obj = new Object;

obj.text = "&Delete right spaces";

obj.run = function() {
	var text = textEdit.selection;
	var result = "";

	result = text.replace(/\s+\n/g,"\n");

	textEdit.selection = result;
	textEdit.document.modified = true;
};

return obj;