declare default element namespace 'http://www.generix.fr/technicalframework/configuration';

(doc($inputDocument)/config/application/presentation)[1]/*[@fileRef=$relativeFileName]/
string(@businessview)
