<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:import href="patients.xsl"/>
	<xsl:template match="/">
		<html>
			<h1>Liste des patients</h1>
			<body>
					<xsl:apply-templates select="/patients"/>
			</body>
		</html>
	</xsl:template>
	<xsl:template match="patients">
		<ul>
			<xsl:apply-templates select="patient"/>
		</ul>
	</xsl:template>
</xsl:stylesheet>
