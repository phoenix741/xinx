/*
 * Java CGI Library
 *
 * Copyright (c) Matthew Johnson 2004
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


package cx.ath.matthew.cgi;

public class HiddenField extends Field
{
   String value;
   public HiddenField(String name, String value)
   {
      this.name = name;
      this.label = "";
      this.value = value;
   }
   protected String print()
   {
      return "<input type=\"hidden\" name=\""+name+"\" value=\""+CGITools.escapeChar(value, '"')+"\" />";
   }
}


