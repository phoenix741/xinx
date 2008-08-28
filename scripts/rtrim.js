var obj = new Object;

obj.text    = "&Delete right spaces";
obj.author  = "Ulrich Van Den Hekke";
obj.licence = "GPL v2 or later"
obj.version = "0.1"

obj.run = function() {
	var text = textEdit.selection;
	var result = "";

	result = text.replace(/\s+\n/g,"\n");

	textEdit.selection = result;
};

return obj;