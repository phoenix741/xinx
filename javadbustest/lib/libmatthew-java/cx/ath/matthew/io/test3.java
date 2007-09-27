package cx.ath.matthew.io;
import java.io.PrintWriter;
import java.io.BufferedReader;
import java.io.InputStreamReader;
class test3
{
   public static void main(String[] args) throws Exception
   {
      String file = args[0];
      PrintWriter p = new PrintWriter(new TeeOutputStream(System.out, file));
      BufferedReader r = new BufferedReader(new InputStreamReader(System.in));
      String s;
      while (null != (s = r.readLine()))
         p.println(s);
      p.close();
      r.close();
   }
}
