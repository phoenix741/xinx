<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:output method="html" indent="yes" encoding="ISO-8859-1"/>
	
	<xsl:template match="/">
		<html>
			<head>
				<title>My books</title>
			</head>
			<body>
				<table border="1">
					<tr>
						<th>Name</th>
						<th>Published</th>
					</tr>
					<tr>
						<th colspan="2">Description</th>
					</tr>
					<xsl:apply-templates select="/livres/livre">
						<xsl:sort select="annee" order="ascending"/>
					</xsl:apply-templates>		
				</table>
			</body>
		</html>
	</xsl:template>
	
	<xsl:template match="livre">
		<xsl:variable name="STYLE">
			<xsl:choose>
				<xsl:when test="position() mod 2 = 0">background-color: gray</xsl:when>
				<xsl:otherwise>background-color: lightgray</xsl:otherwise>
			</xsl:choose>			
		</xsl:variable>
		<tr style="{$STYLE}">
			
			<td>
				<xsl:value-of select="nom"/>
			</td>
			<td>
				<xsl:value-of select="annee"/>
			</td>
		</tr>
		<tr style="{$STYLE}">
			<td colspan="2">
				<xsl:copy-of select="description"/>
			</td>
		</tr>
	</xsl:template>
</xsl:stylesheet>
