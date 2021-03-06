/*
 *  Copyright (C) 2005 IXA Research Group / IXA Ikerkuntza Taldea.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 
 *  02110-1301  USA
 */


wstring header = L""
          L"<xsl:stylesheet xmlns:xsl=\"http://www.w3.org/1999/XSL/Transform\" version=\"1.0\">\n"
          L"  <xsl:template match=\"node()|@*\">\n"
          L"    <xsl:copy><xsl:apply-templates select=\"node()|@*\"/></xsl:copy>\n"
          L"  </xsl:template>" ;
 

wstring footer = L""
                 L"</xsl:stylesheet>" ;

