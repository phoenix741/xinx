package cx.ath.matthew.cgi;
public class NewTable extends Field {
	
	private String name;
	private String cssClass;
	
	public NewTable (String name, String css) {
		this.name = name;
		this.cssClass = css;
	}
	
	protected String print() {
		return "</table>\n<table class=\""+cssClass+"\">";
	}
}
