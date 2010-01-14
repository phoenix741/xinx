<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:template match="patient">
		<li>
			<xsl:apply-templates/>(age: <xsl:value-of select="@age"/> an, sexe: <xsl:value-of select="@sexe"/>)
		</li>
	</xsl:template>
	<xsl:template match="nom">
		<b>
			<xsl:value-of select="."/>
		</b>
	</xsl:template>
	<xsl:template match="prenom">
		<i>
			<xsl:value-of select="."/>
		</i>
	</xsl:template>
</xsl:stylesheet>
