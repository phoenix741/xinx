package cx.ath.matthew.io;
import java.io.PrintWriter;
import java.io.OutputStreamWriter;
class test
{
   public static void main(String[] args) throws Exception
   {
      PrintWriter out = new PrintWriter(new OutputStreamWriter(new ExecOutputStream(System.out, "xsltproc mcr.xsl -")));///java cx.ath.matthew.io.findeof")));
      
      out.println("<?xml version='1.0'?>");
      out.println("   <?xml-stylesheet href='style/mcr.xsl' type='text/xsl'?>");
      out.println("   <mcr xmlns:xi='http://www.w3.org/2001/XInclude'>");
      out.println("   <title>TEST</title>");
      out.println("   <content title='TEST'>");
      out.println("hello, he is helping tie up helen's lemmings");
      out.println("we are being followed and we break out");
      out.println("   </content>");
      out.println("   </mcr>");
      out.close();
   }
}
