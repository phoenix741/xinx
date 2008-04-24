var obj = new Object;

obj.text = "&Replace left space";

obj.run = function() {
	var text = this.textEdit.plainText;
	var result = "";
	
	result = text.replace(/\n(\ \ \ \ )+/g,"\n\t");
	
	this.textEdit.plainText = result;
	this.textEdit.document.modified = true;
};

return obj;