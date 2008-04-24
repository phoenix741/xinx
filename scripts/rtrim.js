var obj = new Object;

obj.text = "&Delete right spaces";

obj.run = function() {
	var text = this.textEdit.plainText;
	var result = "";
	
	result = text.replace(/\s+\n/g,"\n");
	
	this.textEdit.plainText = result;
	this.textEdit.document.modified = true;
};

return obj;