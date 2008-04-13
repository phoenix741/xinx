/***************************************************************************
 *   Copyright (C) 2007 by Ulrich Van Den Hekke                            *
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
#include <xinxconfig.h>
#include "csshighlighter.h"

// Qt header
#include <QStringList>

/* Static member */
QStringList webplugin_css::cssKey, webplugin_css::cssValues, webplugin_css::css2Key, webplugin_css::css2Values;

/* webplugin_css */

webplugin_css::webplugin_css( QObject* parent, XINXConfig * config ) : SyntaxHighlighter( parent, config ) {
	
}

webplugin_css::webplugin_css( QTextDocument* parent, XINXConfig * config ) : SyntaxHighlighter( parent, config ) {
	
}

webplugin_css::webplugin_css( QTextEdit* parent, XINXConfig * config ) : SyntaxHighlighter( parent, config ) {
	
}

void webplugin_css::init() {
	cssKey << "color" << "background-color" << "background-image" 
		<< "background-repeat" << "background-attachment" << "background-position" 
		<< "background" << "font-family" << "font-style" << "font-variant" 
		<< "font-weight" << "font-size" << "font" << "word-spacing" 
		<< "letter-spacing" << "text-decoration" << "vertical-align" 
		<< "text-transform" << "text-align" << "text-indent" << "line-height" 
		<< "margin-top" << "margin-right" << "margin-bottom" << "margin-left" << "margin" 
		<< "padding-top" << "padding-right" << "padding-bottom" << "padding-left"
		<< "padding" << "border-top-width" << "border-right-width" 
		<< "border-bottom-width" << "border-left-width" << "border-width" 
		<< "border-top" << "border-right" << "border-bottom" << "border-left" << "border" 
		<< "border-color" << "border-style" << "width" << "height" << "float" << "clear" 
		<< "display" << "white-space" << "list-style-type" << "list-style-image" 
		<< "list-style-position" << "list-style";
	cssValues << "auto" << "none" << "normal" << "italic" << "oblique" 
		<< "small-caps" << "bold" << "bolder" << "lighter" << "xx-small" << "x-small" 
		<< "small" << "medium" << "large" << "x-large" << "xx-large" << "larger" 
		<< "smaller" << "transparent" << "repeat" << "repeat-x" << "repeat-y" 
		<< "no-repeat" << "scroll" << "fixed" << "top" << "bottom" << "left" 
		<< "center" << "right" << "justify" << "both" << "underline" << "overline" 
		<< "line-through" << "blink" << "baseline" << "sub" << "super" << "text-top" 
		<< "middle" << "text-bottom" << "capitalize" << "uppercase" << "lowercase" 
		<< "thin" << "medium" << "thick" << "dotted" << "dashed" << "solid" << "double" 
		<< "groove" << "ridge" << "inset" << "outset" << "block" << "inline" 
		<< "list-item" << "pre" << "no-wrap" << "inside" << "outside" << "disc" 
		<< "circle" << "square" << "decimal" << "lower-roman" << "upper-roman" 
		<< "lower-alpha" << "upper-alpha" << "aqua" << "black" << "blue" << "fuchsia" 
		<< "gray" << "green" << "lime" << "maroon" << "navy" << "olive" << "purple" 
		<< "red" << "silver" << "teal" << "white" << "yellow";
	css2Key << "border-top-color" << "border-right-color" 
		<< "border-bottom-color" << "border-left-color" << "border-color" << "border-top-style" 
		<< "border-right-style" << "border-bottom-style" << "border-left-style" << "border-style" 
		<< "top" << "right" << "bottom" << "left" << "position" << "z-index" << "direction" 
		<< "unicode-bidi" << "min-width" << "max-width" << "min-height" << "max-height" 
		<< "overflow" << "clip" << "visibility" << "content" << "quotes" << "counter-reset" 
		<< "counter-increment" << "marker-offset" << "size" << "marks" << "page-break-before" 
		<< "page-break-after" << "page-break-inside" << "page" << "orphans" << "widows" 
		<< "font-stretch" << "font-size-adjust" << "unicode-range" << "units-per-em" << "src" 
		<< "panose-1" << "stemv" << "stemh" << "slope" << "cap-height" << "x-height" 
		<< "ascent" << "descent" << "widths" << "bbox" << "definition-src" << "baseline" 
		<< "centerline" << "mathline" << "topline" << "text-shadow" << "caption-side" 
		<< "table-layout" << "border-collapse" << "border-spacing" << "empty-cells" 
		<< "speak-header" << "cursor" << "outline" << "outline-width" << "outline-style" 
		<< "outline-color" << "volume" << "speak" << "pause-before" << "pause-after" 
		<< "pause" << "cue-before" << "cue-after" << "cue" << "play-during" << "azimuth" 
		<< "elevation" << "speech-rate" << "voice-family" << "pitch" << "pitch-range" 
		<< "stress" << "richness" << "speak-punctuation" << "speak-numeral";
	css2Values << "inherit" << "run-in" << "compact" << "marker" 
		<< "table" << "inline-table" << "table-row-group" << "table-header-group" 
		<< "table-footer-group" << "table-row" << "table-column-group" << "table-column" 
		<< "table-cell" << "table-caption" << "static" << "relative" << "absolute" 
		<< "fixed" << "ltr" << "rtl" << "embed" << "bidi-override" << "visible" << "hidden" 
		<< "scroll" << "collapse" << "open-quote" << "close-quote" << "no-open-quote" 
		<< "no-close-quote" << "decimal-leading-zero" << "lower-greek" << "lower-latin" 
		<< "upper-latin" << "hebrew" << "armenian" << "georgian" << "cjk-ideographic" 
		<< "hiragana" << "katakana" << "hiragana-iroha" << "katakana-iroha" << "landscape" 
		<< "portrait" << "crop" << "cross" << "always" << "avoid" << "wider" << "narrower" 
		<< "ultra-condensed" << "extra-condensed" << "condensed" << "semi-condensed" 
		<< "semi-expanded" << "expanded" << "extra-expanded" << "ultra-expanded" << "caption" 
		<< "icon" << "menu" << "message-box" << "small-caption" << "status-bar" << "separate" 
		<< "show" << "hide" << "once" << "crosshair" << "default" << "pointer" << "move" 
		<< "text" << "wait" << "help" << "e-resize" << "ne-resize" << "nw-resize" << "n-resize" 
		<< "se-resize" << "sw-resize" << "s-resize" << "w-resize" << "ActiveBorder" 
		<< "ActiveCaption" << "AppWorkspace" << "Background" << "ButtonFace" << "ButtonHighlight" 
		<< "ButtonShadow" << "InactiveCaptionText" << "ButtonText" << "CaptionText" << "GrayText" 
		<< "Highlight" << "HighlightText" << "InactiveBorder" << "InactiveCaption" << "InfoBackground" 
		<< "InfoText" << "Menu" << "MenuText" << "Scrollbar" << "ThreeDDarkShadow" << "ThreeDFace" 
		<< "ThreeDHighlight" << "ThreeDLightShadow" << "ThreeDShadow" << "Window" << "WindowFrame" 
		<< "WindowText" << "silent" << "x-soft" << "soft" << "medium" << "loud" 
		<< "x-loud" << "spell-out" << "mix" << "left-side" << "far-left" << "center-left" 
		<< "center-right" << "far-right" << "right-side" << "behind" << "leftwards" << "rightwards" 
		<< "below" << "level" << "above" << "higher" << "lower" << "x-slow" << "slow" 
		<< "medium" << "fast" << "x-fast" << "faster" << "slower" << "male" << "female" 
		<< "child" << "x-low" << "low" << "high" << "x-high" << "code" << "digits" << "continous";
}

void webplugin_css::highlightBlock( const QString & text ) {
	const QHash<QString,QTextCharFormat> & formats = m_config->config().formats;
	
	int i = 0, pos = 0;

	QRegExp commentStartExpression("^/\\*");
	QRegExp commentEndExpression("\\*/");
	QRegExp string1Expression("^'[^\']*'");
	QRegExp string2Expression("^\"[^\"]*\"");
	QRegExp directiveExpression("^@.*[{;]");
	QRegExp numberExpression("^\\b[\\-\\+]?[0-9]+(\\.[0-9]+)?(pt|px)?\\b");
	QRegExp keywordExpression("[A-Za-z_][A-Za-z0-9_-]*");
	QRegExp attributeExpression("\\[.*\\]");
	
	ParsingState state = previousBlockState() == InBracket ? CssIdentifier : CssDefault;

	setCurrentBlockState( NoBlock );
	
	if ( previousBlockState() == InComment ) {
		pos = commentEndExpression.indexIn( text, i );

		if (pos >= 0) {
			// end comment found
			const int iLength = commentEndExpression.matchedLength();
			setFormat( 0, pos + iLength, formats["css_comment"] );
			i += pos + iLength; // skip comment
		} else {
			// in comment
			setFormat( 0, text.length(), formats["css_comment"] );
			setCurrentBlockState( InComment );
			return;
		}
	}

	for(; i < text.length(); i++ ) {
		char c = text.at(i).toLower().toAscii();
		if( ( c >= 'a' ) && ( c <= 'z' ) ) {
			pos = keywordExpression.indexIn( text, i, QRegExp::CaretAtOffset );
			
			if( pos == i ) {
				const int iLength = keywordExpression.matchedLength();
				if( state == CssDefault ) {
					setFormat( pos, iLength, formats["css_tag"] );
				} else if( state == CssValue ) {
					if( cssValues.contains( keywordExpression.cap() ) )
						setFormat( pos, iLength, formats["css_value1"] );
					else if( css2Values.contains( keywordExpression.cap() ) )
						setFormat( pos, iLength, formats["css_value2"] );
					else
						setFormat( pos, iLength, formats["css_value"] );
				} else if( state == CssIdentifier ) {
					if( cssKey.contains( keywordExpression.cap() ) )
						setFormat( pos, iLength, formats["css_identifier1"] );
					else if( css2Key.contains( keywordExpression.cap() ) )
						setFormat( pos, iLength, formats["css_identifier2"] );
					else
						setFormat( pos, iLength, formats["css_identifier"] );
				}
				i += iLength - 1;
			}
		} else if( ( c >= '0' ) && ( c <= '9' ) ) {
			pos = numberExpression.indexIn( text, i, QRegExp::CaretAtOffset );
		
			if( pos == i ) {
				const int iLength = numberExpression.matchedLength();
				if( state == CssValue )
					setFormat( pos, iLength, formats["css_number"] );
				i += iLength - 1;
			}
		} else if( c == '/' ) {
			pos = commentStartExpression.indexIn( text, i, QRegExp::CaretAtOffset );
			
			if( pos == i ) {
				int posEnd = commentEndExpression.indexIn( text, i + 2 );
				int length = (posEnd >= 0) ? posEnd + 2 : ( text.length() - pos );
	
				setFormat( pos, length, formats["css_comment"] );
				i += length;
				if( posEnd == -1 ) {
					setCurrentBlockState( InComment );
					return;
				}
			} else {
				setFormat( i, 1, formats["css_other"] );
			}
		} else if( ( c == '\'' ) || ( c == '"' ) ) {
			int iLength;
			if( c == '\'' ) {
				pos = string1Expression.indexIn( text, i, QRegExp::CaretAtOffset );
				iLength = string1Expression.matchedLength();
			} else { 
				pos = string2Expression.indexIn( text, i, QRegExp::CaretAtOffset );
				iLength = string2Expression.matchedLength();
			}

			if( pos == i ) {
				setFormat( pos, iLength, formats["css_string"] );
				i += iLength;
			}
		} else if( c == '@' ) { // CSS Directive
			pos = directiveExpression.indexIn( text, i, QRegExp::CaretAtOffset );
			if( pos == i ) {
				const int iLength = directiveExpression.matchedLength();
				setFormat( pos, 1, formats["css_operator"] );
				setFormat( pos + 1, iLength - 2, formats["css_directive"] );
				setFormat( pos + iLength - 1, 1, formats["css_operator"] );
				i += iLength;
			}
		} else if( c == ':' ) {
			setFormat( i, 1, formats["css_operator"] );
			if( state == CssIdentifier ) {
				state = CssValue;
			} else if( state != CssValue ){
				pos = keywordExpression.indexIn( text, i + 1, QRegExp::CaretAtOffset );
				if( pos == ( i + 1 ) ) {
					const int iLength = keywordExpression.matchedLength();
					setFormat( pos, iLength, formats["css_pseudoclass"] );
					i += iLength;
				}
			}
		} else if( c == '.' ) {
			setFormat( i, 1, formats["css_operator"] );
			if( ( state != CssIdentifier ) && ( state != CssValue ) ) {
				pos = keywordExpression.indexIn( text, i + 1, QRegExp::CaretAtOffset );
				if( pos == ( i + 1 ) ) {
					const int iLength = keywordExpression.matchedLength();
					setFormat( pos, iLength, formats["css_class"] );
					i += iLength;
				}
			}
		} else if( c == '#' ) {
			setFormat( i, 1, formats["css_operator"] );
			if( ( state != CssIdentifier ) && ( state != CssValue ) ) {
				pos = keywordExpression.indexIn( text, i + 1, QRegExp::CaretAtOffset );
				if( pos == ( i + 1 ) ) {
					const int iLength = keywordExpression.matchedLength();
					setFormat( pos, iLength, formats["css_id"] );
					i += iLength;
				}
			}
		} else if( c == ';' ) {
			setFormat( i, 1, formats["css_operator"] );
			if( state == CssValue )
				state = CssIdentifier;
		} else if( c == '*' ) {
			if( state == CssDefault )
				setFormat( i, 1, formats["css_tag"] );
			else
				setFormat( i, 1, formats["css_operator"] );
		} else if( c == '[' ) {
			pos = attributeExpression.indexIn( text, i, QRegExp::CaretAtOffset );
			if( pos == i ) {
				const int iLength = attributeExpression.matchedLength();
				setFormat( pos, 1, formats["css_operator"] );
				setFormat( pos + 1, iLength - 2, formats["css_attribute"] );
				setFormat( pos + iLength - 1, 1, formats["css_operator"] );
				i += iLength;
			}
		} else if( c == '{' ) {
			setFormat( i, 1, formats["css_operator"] );
			if( state == CssDefault )
				state = CssIdentifier;
		} else if( c == '}' ) {
			setFormat( i, 1, formats["css_operator"] );
			state = CssDefault;
		} else if( c == '.' ) {
			setFormat( i, 1, formats["css_operator"] );
		} else if( ( c == '>' ) || ( c == '+' ) || ( c == ',' ) || ( c == '!' ) ) {
			setFormat( i, 1, formats["css_operator"] );
		}
			
	}
	
	if( ( state == CssIdentifier ) || ( state == CssValue ) ) {
		setCurrentBlockState( InBracket );
	}
}
