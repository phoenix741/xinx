<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:import href="patients.xsl"/>
	<xsl:template match="/">
		<html>
			<h1>Liste des patients</h1>
			<body>
				<ul>
					<xsl:apply-templates select="/patients/patient"/>
				</ul>
			</body>
		</html>
	</xsl:template>
</xsl:stylesheet>
