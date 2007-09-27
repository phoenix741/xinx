/*
 * Java Hexdump Library
 *
 * Copyright (c) Matthew Johnson 2005
 *
 * This program is free software; you can redistribute it and/or 
 * modify it under the terms of the GNU Lesser General Public License 
 * as published by the Free Software Foundation, version 2 only.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details. 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * To Contact the author, please email src@matthew.ath.cx
 *
 */

package cx.ath.matthew.utils;

import java.io.PrintStream;

public class Hexdump
{
   public static final char[] hexchars = new char[] { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
   public static String toHex(byte[] buf)
   {
      return toHex(buf, 0, buf.length);
   }
   public static String toHex(byte[] buf, int ofs, int len)
   {
      StringBuffer sb = new StringBuffer();
      int j = ofs+len;
      for (int i = ofs; i < j; i++) {
         if (i < buf.length) {
            sb.append(hexchars[(buf[i] & 0xF0) >> 4]);
            sb.append(hexchars[buf[i] & 0x0F]);
            sb.append(' ');
         } else {
            sb.append(' ');
            sb.append(' ');
            sb.append(' ');
         }
      }
      return sb.toString();
   }
   
   public static String toAscii(byte[] buf)
   {
      return toAscii(buf, 0, buf.length);
   }
   public static String toAscii(byte[] buf, int ofs, int len)
   {
      StringBuffer sb = new StringBuffer();
      int j = ofs+len;
      for (int i = ofs; i < j ; i++) {
         if (i < buf.length) {
            if (20 <= buf[i] && 126 >= buf[i])
               sb.append((char) buf[i]);
            else
               sb.append('.');
         } else
            sb.append(' ');
      }
      return sb.toString();
   }
   public static String format(byte[] buf)
   {
      return format(buf, 80);
   }
   public static String format(byte[] buf, int width)
   {
      int bs = (width - 8) / 4;
      int i = 0;
      StringBuffer sb = new StringBuffer();
      do {
         for (int j = 0; j < 6; j++) {
            sb.append(hexchars[(i << (j*4)  & 0xF00000) >> 20]);
         }
         sb.append('\t');
         sb.append(toHex(buf, i, bs));
         sb.append(' ');
         sb.append(toAscii(buf, i, bs));
         sb.append('\n');
         i += bs;
      } while (i < buf.length);
      return sb.toString();
   }
   public static void print(byte[] buf)
   {
      print(buf, System.err);
   }
   public static void print(byte[] buf, int width)
   {
      print(buf, width, System.err);
   }
   public static void print(byte[] buf, int width, PrintStream out)
   {
      out.print(format(buf, width));
   }
   public static void print(byte[] buf, PrintStream out)
   {
      out.print(format(buf));
   }
}
