package cx.ath.matthew.io;
import java.io.BufferedReader;
import java.io.InputStreamReader;
class test2
{
   public static void main(String[] args) throws Exception
   {
      BufferedReader in = new BufferedReader(new InputStreamReader(new ExecInputStream(System.in, "xsltproc mcr.xsl -")));
      String s;
      while (null != (s = in.readLine())) System.out.println(s);
   }
}
