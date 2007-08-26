/***************************************************************************
 *   Copyright (C) 2006 by Ulrich Van Den Hekke                            *
 *   ulrich.vdh@free.fr                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

// Xinx header
#include "xmlfileeditor.h"
#include "xmleditor.h"
#include "xsleditor.h"

// Qt header
#include <QDomDocument>

XMLFileEditor::XMLFileEditor( QWidget *parent, TextEditor * textEditor ) : FileEditor( textEditor ? textEditor : new XMLEditor( parent ), parent ) {
	
}

XMLFileEditor::~XMLFileEditor() {
	
}

QString XMLFileEditor::getSuffix() const {
	if( getFileName().isEmpty() ) 
		return "xml";
	else
		return FileEditor::getSuffix();
}

void XMLFileEditor::autoIndent() {
	QDomDocument document;
	QString errorStr;
	int errorLine = 0, errorColumn = 0;  
	
	if ( document.setContent(textEdit()->toPlainText(), true, &errorStr, &errorLine, &errorColumn) ) {
		// TODO : Use Alex algo
		textEdit()->setPlainText( document.toString() );
		setModified( true );
	} else {
		setMessage( tr("Parse error column %1:%2").arg(errorColumn).arg(errorStr).arg(errorLine) );
	}
}
/*
        /**
         * @param level Niveau utilisé pour le pretty-print
         * @param result Le StringBuilder qui contient la chaîne finale
         * @param n Le noeud à écrire
         *
        private void constructXML(int level, StringBuilder result, Node n)
        {
                Node node = n;

                while (node != null)
                {
                        // Traitement d'un élément
                        if (node.getNodeType() == Node.ELEMENT_NODE)
                        {
                                // Les tabulations de départ
                                for (int i = 0; i < level; i++)
                                        result.append('\t');
                                result.append("<");
                                result.append(node.getNodeName());

                                if (node.hasAttributes())
                                        result.append(constructAttributes(node));

                                if (node.hasChildNodes())
                                {
                                        result.append(">");
                                        // Si c'est un noeud texte, il ne faut pas faire de saut de ligne
                                        if (node.getFirstChild().getNodeType() == Node.TEXT_NODE)
                                        {
                                                result.append(node.getFirstChild().getNodeValue());
                                        }
                                        else
                                        {
                                                result.append('\r');
                                                result.append('\n');

                                                constructXML(level + 1, result, node.getFirstChild());

                                                for (int i = 0; i < level; i++)
                                                        result.append('\t');
                                        }

                                        result.append("</");
                                        result.append(node.getNodeName());
                                        result.append(">");
                                }
                                else
                                {
                                        result.append("/>");
                                }

                                result.append('\r');
                                result.append('\n');
                        }
                        // Elément contenant une valeur texte
                        else if (node.getNodeType() == Node.TEXT_NODE)
                        {
                                if (!empty(node.getNodeValue()))
                                        result.append(normalize(node.getNodeValue()));
                        }

                        // Noeud suivant
                        node = node.getNextSibling();
                }
        }

        /**
         * @param node
         * @return
         *
        private String constructAttributes(Node node)
        {
                StringBuilder result = new StringBuilder();
                NamedNodeMap nnm = node.getAttributes();

                for (int i = 0; i < nnm.getLength(); i++)
                {
                        result.append(" ");
                        result.append(nnm.item(i).getNodeName());
                        result.append("=\"");
                        result.append(normalize(nnm.item(i).getNodeValue()));
                        result.append("\"");
                }

                return result.toString();
        }

        /**
         * Normalisation de la chaîne passée en paramètre. On ne transforme que les caractères "<" en
         * leur entité html
         *
         * @param value
         * @return
         *
        private String normalize(String value)
        {
                StringBuilder sb = new StringBuilder(value.length());

                for (int i = 0; i < value.length(); i++)
                {
                        if (value.charAt(i) == '<')
                                sb.append("&lt;");
                        // else if (value.charAt(i) == '>')
                        // sb.append("&gt;");
                        // else if (value.charAt(i) == '"')
                        // sb.append("&quot;");
                        else
                                sb.append(value.charAt(i));
                }

                return sb.toString();
        }

        /**
         * @param value
         * @return
         *
        private boolean empty(String value)
        {
                boolean ret = true;

                for (int i = 0; i < value.length(); i++)
                {
                        ret = ret && (value.charAt(i) == ' ' || value.charAt(i) == '\t' || value.charAt(i) == '\r' || value.charAt(i) == '\n');
                }

                return ret;
        }
*/

XSLFileEditor::XSLFileEditor( QWidget *parent ) : XMLFileEditor( parent, new XSLEditor( parent ) ) {
	connect( m_view, SIGNAL(hasError(QString)), this, SLOT(setMessage(QString)) );
}

XSLFileEditor::~XSLFileEditor() {
	
}

QString XSLFileEditor::getSuffix() const {
	if( getFileName().isEmpty() ) 
		return "xsl";
	else
		return XMLFileEditor::getSuffix();
}
