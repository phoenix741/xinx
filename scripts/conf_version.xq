<result xmlns="http://www.generix.fr/technicalframework/configuration">
{
(doc($inputDocument)/config/version/numero)[1]/
<numero>{string(text())}</numero>
}
{
(doc($inputDocument)/config/version/edition_speciale)[1]/
<edition_speciale>{string(text())}</edition_speciale>
}
{
(doc($inputDocument)/config/application/properties)[1]/
<xmlpres>{string(@xmlPresentationFile)}</xmlpres>
}
{
(doc($inputDocument)/config/application/presentation)[1]/
<rootPath>{string(@rootPath)}</rootPath>
}
</result>
