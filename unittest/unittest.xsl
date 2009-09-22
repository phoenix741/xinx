<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:output method="xml" encoding="ISO-8859-1"/>
	
	<xsl:template match="/">
		<report category="test">
			<xsl:apply-templates select="document('unittest.xml')/TestCase/TestFunction"/>
			<xsl:apply-templates select="document('unittest2.xml')/TestCase/TestFunction"/>
			<xsl:apply-templates select="document('unittest3.xml')/TestCase/TestFunction"/>
		</report>
	</xsl:template>

	<xsl:template match="TestFunction">
		<test>
			<xsl:if test="Incident/@file!=''">
				<file>unittest/<xsl:value-of select="Incident/@file"/></file>
			</xsl:if>
			<fixture>
				<xsl:value-of select="@name"/>
			</fixture>
			<xsl:choose>
				<xsl:when test="Incident/@type='fail'">
					<status>failure</status>
					<stdout>In <xsl:value-of select="Incident/@file"/>(<xsl:value-of select="Incident/@line"/>) : <xsl:value-of select="Incident/Description/text()"/></stdout>
				</xsl:when>
				<xsl:when test="Incident/@type='pass'">
					<status>success</status>
				</xsl:when>
				<xsl:otherwise>
					<status>error</status>
				</xsl:otherwise>
			</xsl:choose>
		</test>
	</xsl:template>
</xsl:stylesheet>
 
