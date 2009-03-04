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
	var text = textEdit.plainText;
	var result = "";

	result = text.replace(/(\$RCSfile:\s+).*(,v\s+\$)/g,"$1" + editor.filename.substring( editor.filename.lastIndexOf('/') + 1 ) + "$2");

	textEdit.plainText = result;
	textEdit.modified = true;
};

obj;
