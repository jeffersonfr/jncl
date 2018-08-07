/***************************************************************************
 *   Copyright (C) 2005 by Jeff Ferr                                       *
 *   root@sat                                                              *
 *                                                                         *
 *   This program is free software; you can redistr.c_str()ibute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distr.c_str()ibuted in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "nclhelper.h"

#include "jcommon/jstringutils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

namespace jncl {

NCLHelper::NCLHelper()
{
}

NCLHelper::~NCLHelper()
{
}

double NCLHelper::ParseTime(std::string str)
{
	if (str.empty() == true) {
		return 0.0;
	}

	double t = atof(str.c_str());

	if (str.find("ms") != std::string::npos) {
		t = t * 1.0;
	} else if (str.find("s") != std::string::npos) {
		t = t * 1000.0;
	} else if (str.find("m") != std::string::npos) {
		t = t * 60000.0;
	} else if (str.find("h") != std::string::npos) {
		t = t * 3600000.0;
	}

	return t;
}

double NCLHelper::ParseNumber(std::string str, double min, double max)
{
	double i = atof(str.c_str());

	if (i < min) {
		i = min;
	}

	if (i > max) {
		i = max;
	}

	return i;
}

double NCLHelper::ParseNumber(std::string str, double reference, double min, double max)
{
	double i = atof(str.c_str());

	if (str.find("%") != std::string::npos) {
		i = (i/100.0)*reference;
	} else if (str.find(".") != std::string::npos) {
		if (i >= 0 && i <= 1) {
			i = i*reference;
		}
	}

	if (i < min) {
		i = min;
	}

	if (i > max) {
		i = max;
	}

	return i;
}

uint32_t NCLHelper::ParseColor(std::string str)
{
	uint32_t color = 0xff000000;

	const char *cstr = jcommon::StringUtils::ToLower(jcommon::StringUtils::Trim(str)).c_str();

	if (strstr(cstr, "0x") == cstr) {
		color = (uint32_t)strtoul(str.c_str()+2, NULL, 16);
	} else {
		if (strcmp(cstr, "white") == 0) {
			color = 0xffffffff;
		} else if (strcmp(cstr, "aqua") == 0) {
			color = 0xff00ffff;
		} else if (strcmp(cstr, "lime") == 0) {
			color = 0xff00ff00;
		} else if (strcmp(cstr, "yellow") == 0) {
			color = 0xffffff00;
		} else if (strcmp(cstr, "red") == 0) {
			color = 0xffff0000;
		} else if (strcmp(cstr, "fuchsia") == 0) {
			color = 0xffff00ff;
		} else if (strcmp(cstr, "purple") == 0) {
			color = 0xff800080;
		} else if (strcmp(cstr, "maroon") == 0) {
			color = 0xff800000;
		} else if (strcmp(cstr, "blue") == 0) {
			color = 0xff0000ff;
		} else if (strcmp(cstr, "navy") == 0) {
			color = 0xff000080;
		} else if (strcmp(cstr, "teal") == 0) {
			color = 0xff008080;
		} else if (strcmp(cstr, "green") == 0) {
			color = 0xff008000;
		} else if (strcmp(cstr, "olive") == 0) {
			color = 0xff808000;
		} else if (strcmp(cstr, "silver") == 0) {
			color = 0xffc0c0c0;
		} else if (strcmp(cstr, "gray") == 0) {
			color = 0xff808080;
		} else if (strcmp(cstr, "black") == 0) {
			color = 0xff000000;
		} else if (strcmp(cstr, "aliceblue") == 0) {
			color = 0xfff0f8ff;
		} else if (strcmp(cstr, "antiquewhite") == 0) {
			color = 0xfffaebd7;
		} else if (strcmp(cstr, "aquamarine") == 0) {
			color = 0xff7fffd4;
		} else if (strcmp(cstr, "azure") == 0) {
			color = 0xfff0ffff;
		} else if (strcmp(cstr, "beige") == 0) {
			color = 0xfff5f5dc;
		} else if (strcmp(cstr, "bisque") == 0) {
			color = 0xffffe4c4;
		} else if (strcmp(cstr, "black") == 0) {
			color = 0xff000000;
		} else if (strcmp(cstr, "blanchedalmond") == 0) {
			color = 0xffffebcd;
		} else if (strcmp(cstr, "blueviolet") == 0) {
			color = 0xff8a2be2;
		} else if (strcmp(cstr, "brown") == 0) {
			color = 0xffa52a2a;
		} else if (strcmp(cstr, "burlywood") == 0) {
			color = 0xffdeb887;
		} else if (strcmp(cstr, "cadetblue") == 0) {
			color = 0xff5f9ea0;
		} else if (strcmp(cstr, "chartreuse") == 0) {
			color = 0xff7fff00;
		} else if (strcmp(cstr, "chocolate") == 0) {
			color = 0xffd2691e;
		} else if (strcmp(cstr, "coral") == 0) {
			color = 0xffff7f50;
		} else if (strcmp(cstr, "cornflowerblue") == 0) {
			color = 0xff6495ed;
		} else if (strcmp(cstr, "cornsilk") == 0) {
			color = 0xfffff8dc;
		} else if (strcmp(cstr, "crimson") == 0) {
			color = 0xffdc143c;
		} else if (strcmp(cstr, "cyan") == 0) {
			color = 0xff00ffff;
		} else if (strcmp(cstr, "darkblue") == 0) {
			color = 0xff00008b;
		} else if (strcmp(cstr, "darkcyan") == 0) {
			color = 0xff008b8b;
		} else if (strcmp(cstr, "darkGoldenrod") == 0) {
			color = 0xffb8860b;
		} else if (strcmp(cstr, "darkgray") == 0) {
			color = 0xffa9a9a9;
		} else if (strcmp(cstr, "darkgrey") == 0) {
			color = 0xffa9a9a9;
		} else if (strcmp(cstr, "darkgreen") == 0) {
			color = 0xff006400;
		} else if (strcmp(cstr, "darkkhaki") == 0) {
			color = 0xffbdb76b;
		} else if (strcmp(cstr, "darkmagenta") == 0) {
			color = 0xff8b008b;
		} else if (strcmp(cstr, "darkolivegreen") == 0) {
			color = 0xff556b2f;
		} else if (strcmp(cstr, "darkorange") == 0) {
			color = 0xffff8c00;
		} else if (strcmp(cstr, "darkorchid") == 0) {
			color = 0xff9932cc;
		} else if (strcmp(cstr, "darkred") == 0) {
			color = 0xff8b0000;
		} else if (strcmp(cstr, "darksalmon") == 0) {
			color = 0xffe9967a;
		} else if (strcmp(cstr, "darkseagreen") == 0) {
			color = 0xff8fbc8f;
		} else if (strcmp(cstr, "darkslateblue") == 0) {
			color = 0xff483d8b;
		} else if (strcmp(cstr, "darkslategray") == 0) {
			color = 0xff2f4f4f;
		} else if (strcmp(cstr, "darkslategrey") == 0) {
			color = 0xff3f4f4f;
		} else if (strcmp(cstr, "darkturquoise") == 0) {
			color = 0xff00ced1;
		} else if (strcmp(cstr, "darkviolet") == 0) {
			color = 0xff9400d3;
		} else if (strcmp(cstr, "deeppink") == 0) {
			color = 0xffff1493;
		} else if (strcmp(cstr, "deepskyblue") == 0) {
			color = 0xff00bf00;
		} else if (strcmp(cstr, "dimgray") == 0) {
			color = 0xff696969;
		} else if (strcmp(cstr, "dimgrey") == 0) {
			color = 0xff696969;
		} else if (strcmp(cstr, "dodgerblue") == 0) {
			color = 0xff1e90ff;
		} else if (strcmp(cstr, "firebrick") == 0) {
			color = 0xffb22222;
		} else if (strcmp(cstr, "floralwhite") == 0) {
			color = 0xfffffaf0;
		} else if (strcmp(cstr, "forestgreen") == 0) {
			color = 0xff228b22;
		} else if (strcmp(cstr, "fuchsia") == 0) {
			color = 0xffff00ff;
		} else if (strcmp(cstr, "gainsboro") == 0) {
			color = 0xffdcdcdc;
		} else if (strcmp(cstr, "ghostwhite") == 0) {
			color = 0xfff8f8ff;
		} else if (strcmp(cstr, "gold") == 0) {
			color = 0xffffd700;
		} else if (strcmp(cstr, "goldenrod") == 0) {
			color = 0xffdaa520;
		} else if (strcmp(cstr, "gray") == 0) {
			color = 0xff808080;
		} else if (strcmp(cstr, "grey") == 0) {
			color = 0xff808080;
		} else if (strcmp(cstr, "greenyellow") == 0) {
			color = 0xffadff2f;
		} else if (strcmp(cstr, "honeydew") == 0) {
			color = 0xfff0fff0;
		} else if (strcmp(cstr, "hotpink") == 0) {
			color = 0xffff69b4;
		} else if (strcmp(cstr, "indianred") == 0) {
			color = 0xffcd5c5c;
		} else if (strcmp(cstr, "indigo") == 0) {
			color = 0xff4b0082;
		} else if (strcmp(cstr, "ivory") == 0) {
			color = 0xfffffff0;
		} else if (strcmp(cstr, "khaki") == 0) {
			color = 0xfff0e68c;
		} else if (strcmp(cstr, "lavender") == 0) {
			color = 0xffe6e6fa;
		} else if (strcmp(cstr, "kavenderblush") == 0) {
			color = 0xfffff0f5;
		} else if (strcmp(cstr, "lawngreen") == 0) {
			color = 0xff7cfc00;
		} else if (strcmp(cstr, "lemonchiffon") == 0) {
			color = 0xfffffacd;
		} else if (strcmp(cstr, "lightblue") == 0) {
			color = 0xffadd8e6;
		} else if (strcmp(cstr, "lightcoral") == 0) {
			color = 0xfff08080;
		} else if (strcmp(cstr, "lightcyan") == 0) {
			color = 0xffe0ffff;
		} else if (strcmp(cstr, "lightgoldenrodyellow") == 0) {
			color = 0xfffafad2;
		} else if (strcmp(cstr, "lightgray") == 0) {
			color = 0xffd3d3d3;
		} else if (strcmp(cstr, "lightgrey") == 0) {
			color = 0xffd3d3d3;
		} else if (strcmp(cstr, "lightgreen") == 0) {
			color = 0xff90ee90;
		} else if (strcmp(cstr, "lightpink") == 0) {
			color = 0xffffb6c1;
		} else if (strcmp(cstr, "lightsalmon") == 0) {
			color = 0xffffa07a;
		} else if (strcmp(cstr, "lightseagreen") == 0) {
			color = 0xff20b2aa;
		} else if (strcmp(cstr, "lightskyblue") == 0) {
			color = 0xff87cefa;
		} else if (strcmp(cstr, "lightslategray") == 0) {
			color = 0xff778899;
		} else if (strcmp(cstr, "lightslategrey") == 0) {
			color = 0xff778899;
		} else if (strcmp(cstr, "lightsteelblue") == 0) {
			color = 0xffb0c4de;
		} else if (strcmp(cstr, "lightyellow") == 0) {
			color = 0xffffffe0;
		} else if (strcmp(cstr, "limegreen") == 0) {
			color = 0xff32cd32;
		} else if (strcmp(cstr, "linen") == 0) {
			color = 0xfffaf0e6;
		} else if (strcmp(cstr, "magenta") == 0) {
			color = 0xffff00ff;
		} else if (strcmp(cstr, "mediumaquamarine") == 0) {
			color = 0xff6ccdaa;
		} else if (strcmp(cstr, "mediumblue") == 0) {
			color = 0xff0000cd;
		} else if (strcmp(cstr, "mediumorchid") == 0) {
			color = 0xffba55d3;
		} else if (strcmp(cstr, "mediumpurple") == 0) {
			color = 0xff9370d6;
		} else if (strcmp(cstr, "mediumseagreen") == 0) {
			color = 0xff3cb371;
		} else if (strcmp(cstr, "mediumslateblue") == 0) {
			color = 0xff7b68ee;
		} else if (strcmp(cstr, "mediumspringgreen") == 0) {
			color = 0xff00fa9a;
		} else if (strcmp(cstr, "mediumturquoise") == 0) {
			color = 0xff48d1cc;
		} else if (strcmp(cstr, "mediumvioletred") == 0) {
			color = 0xffc71585;
		} else if (strcmp(cstr, "midnightblue") == 0) {
			color = 0xff191970;
		} else if (strcmp(cstr, "mintcream") == 0) {
			color = 0xfff5fffa;
		} else if (strcmp(cstr, "mistyrose") == 0) {
			color = 0xffffe4e1;
		} else if (strcmp(cstr, "moccasin") == 0) {
			color = 0xffffe4b5;
		} else if (strcmp(cstr, "navajowhite") == 0) {
			color = 0xffffdead;
		} else if (strcmp(cstr, "navy") == 0) {
			color = 0xff000080;
		} else if (strcmp(cstr, "oldlace") == 0) {
			color = 0xfffdf5e6;
		} else if (strcmp(cstr, "olivedrab") == 0) {
			color = 0xff6b8e23;
		} else if (strcmp(cstr, "orange") == 0) {
			color = 0xffffa500;
		} else if (strcmp(cstr, "orangered") == 0) {
			color = 0xffff4500;
		} else if (strcmp(cstr, "orchid") == 0) {
			color = 0xffda70d6;
		} else if (strcmp(cstr, "palegoldenrod") == 0) {
			color = 0xffeee8aa;
		} else if (strcmp(cstr, "palegreen") == 0) {
			color = 0xff98fb98;
		} else if (strcmp(cstr, "paleturquoise") == 0) {
			color = 0xffafeeee;
		} else if (strcmp(cstr, "palevioletred") == 0) {
			color = 0xffd87093;
		} else if (strcmp(cstr, "papayawhip") == 0) {
			color = 0xffffefd5;
		} else if (strcmp(cstr, "peachpuff") == 0) {
			color = 0xffffdab9;
		} else if (strcmp(cstr, "peru") == 0) {
			color = 0xffcd853f;
		} else if (strcmp(cstr, "pink") == 0) {
			color = 0xffffc0cb;
		} else if (strcmp(cstr, "plum") == 0) {
			color = 0xffdda0dd;
		} else if (strcmp(cstr, "powderblue") == 0) {
			color = 0xffb0e0e6;
		} else if (strcmp(cstr, "rosybrown") == 0) {
			color = 0xffbc8f8f;
		} else if (strcmp(cstr, "royalblue") == 0) {
			color = 0xff4169e1;
		} else if (strcmp(cstr, "saddlebrown") == 0) {
			color = 0xff8b4513;
		} else if (strcmp(cstr, "salmon") == 0) {
			color = 0xfffa8072;
		} else if (strcmp(cstr, "sandybrown") == 0) {
			color = 0xfff4a460;
		} else if (strcmp(cstr, "seagreen") == 0) {
			color = 0xff2e8b57;
		} else if (strcmp(cstr, "seashell") == 0) {
			color = 0xfffff5ee;
		} else if (strcmp(cstr, "sienna") == 0) {
			color = 0xffa0522d;
		} else if (strcmp(cstr, "skyblue") == 0) {
			color = 0xff87ceeb;
		} else if (strcmp(cstr, "stateblue") == 0) {
			color = 0xff6a5acd;
		} else if (strcmp(cstr, "stategray") == 0) {
			color = 0xff708090;
		} else if (strcmp(cstr, "stategrey") == 0) {
			color = 0xff708090;
		} else if (strcmp(cstr, "snow") == 0) {
			color = 0xfffffafa;
		} else if (strcmp(cstr, "springgreen") == 0) {
			color = 0xff00ff7f;
		} else if (strcmp(cstr, "steelblue") == 0) {
			color = 0xff4682b4;
		} else if (strcmp(cstr, "tan") == 0) {
			color = 0xffd2b48c;
		} else if (strcmp(cstr, "thistle") == 0) {
			color = 0xffd8bfd8;
		} else if (strcmp(cstr, "tomato") == 0) {
			color = 0xffff6347;
		} else if (strcmp(cstr, "turquoise") == 0) {
			color = 0xff40e0d0;
		} else if (strcmp(cstr, "violet") == 0) {
			color = 0xffee82ee;
		} else if (strcmp(cstr, "wheat") == 0) {
			color = 0xfff5deb3;
		} else if (strcmp(cstr, "whitesmoke") == 0) {
			color = 0xfff5f5f5;
		} else if (strcmp(cstr, "yellowgreen") == 0) {
			color = 0xff9acd32;
		} else if (strcmp(cstr, "transparent") == 0) {
			color = 0x00000000;
		}
	}

	return color;
}

std::string NCLHelper::ParseEscape(std::string str)
{
	str = jcommon::StringUtils::Trim(str);

	if (str.find("&") == std::string::npos) {
		return str;
	}

	str = jcommon::StringUtils::ReplaceString(str, "&quot;", "\"");
	str = jcommon::StringUtils::ReplaceString(str, "&amp;", "&");
	str = jcommon::StringUtils::ReplaceString(str, "&lt;", "<");
	str = jcommon::StringUtils::ReplaceString(str, "&gt;", ">");
	str = jcommon::StringUtils::ReplaceString(str, "&nbsp;", "\xa0");
	str = jcommon::StringUtils::ReplaceString(str, "&iexcl;", "\xa1");
	str = jcommon::StringUtils::ReplaceString(str, "&cent;", "\xa2");
	str = jcommon::StringUtils::ReplaceString(str, "&pound;", "\xa3");
	str = jcommon::StringUtils::ReplaceString(str, "&curren;", "\xa4");
	str = jcommon::StringUtils::ReplaceString(str, "&yen;", "\xa5");
	str = jcommon::StringUtils::ReplaceString(str, "&brvbar;", "\xa6");
	str = jcommon::StringUtils::ReplaceString(str, "&sect;", "\xa7");
	str = jcommon::StringUtils::ReplaceString(str, "&uml;", "\xa8");
	str = jcommon::StringUtils::ReplaceString(str, "&copy;", "\xa9");
	str = jcommon::StringUtils::ReplaceString(str, "&ordf;", "\xaa");
	str = jcommon::StringUtils::ReplaceString(str, "&laquo;", "\xab");
	str = jcommon::StringUtils::ReplaceString(str, "&not;", "\xac");
	str = jcommon::StringUtils::ReplaceString(str, "&shy;", "\xad");
	str = jcommon::StringUtils::ReplaceString(str, "&reg;", "\xae");
	str = jcommon::StringUtils::ReplaceString(str, "&macr;", "\xaf");
	str = jcommon::StringUtils::ReplaceString(str, "&deg;", "\xb0");
	str = jcommon::StringUtils::ReplaceString(str, "&plusmn;", "\xb1");
	str = jcommon::StringUtils::ReplaceString(str, "&sup2;", "\xb2");
	str = jcommon::StringUtils::ReplaceString(str, "&sup3;", "\xb3");
	str = jcommon::StringUtils::ReplaceString(str, "&acute;", "\xb4");
	str = jcommon::StringUtils::ReplaceString(str, "&micro;", "\xb5");
	str = jcommon::StringUtils::ReplaceString(str, "&para;", "\xb6");
	str = jcommon::StringUtils::ReplaceString(str, "&middot;", "\xb7");
	str = jcommon::StringUtils::ReplaceString(str, "&cedil;", "\xb8");
	str = jcommon::StringUtils::ReplaceString(str, "&sup1;", "\xb9");
	str = jcommon::StringUtils::ReplaceString(str, "&ordm;", "\xba");
	str = jcommon::StringUtils::ReplaceString(str, "&raquo;", "\xbb");
	str = jcommon::StringUtils::ReplaceString(str, "&frac14;", "\xbc");
	str = jcommon::StringUtils::ReplaceString(str, "&frac12;", "\xbd");
	str = jcommon::StringUtils::ReplaceString(str, "&frac34;", "\xbe");
	str = jcommon::StringUtils::ReplaceString(str, "&iquest;", "\xbf");
	str = jcommon::StringUtils::ReplaceString(str, "&Agrave;", "\xc0");
	str = jcommon::StringUtils::ReplaceString(str, "&Aacute;", "\xc1");
	str = jcommon::StringUtils::ReplaceString(str, "&Acirc;", "\xc2");
	str = jcommon::StringUtils::ReplaceString(str, "&Atilde;", "\xc3");
	str = jcommon::StringUtils::ReplaceString(str, "&Amul;", "\xc4");
	str = jcommon::StringUtils::ReplaceString(str, "&Aring;", "\xc5");
	str = jcommon::StringUtils::ReplaceString(str, "&AElig;", "\xc6");
	str = jcommon::StringUtils::ReplaceString(str, "&Ccdedil;", "\xc7");
	str = jcommon::StringUtils::ReplaceString(str, "&Egrave;", "\xc8");
	str = jcommon::StringUtils::ReplaceString(str, "&Eacute;", "\xc9");
	str = jcommon::StringUtils::ReplaceString(str, "&Ecirc;", "\xca");
	str = jcommon::StringUtils::ReplaceString(str, "&Emul;", "\xcb");
	str = jcommon::StringUtils::ReplaceString(str, "&Igrave;", "\xcc");
	str = jcommon::StringUtils::ReplaceString(str, "&Iacute;", "\xcd");
	str = jcommon::StringUtils::ReplaceString(str, "&Icirc;", "\xce");
	str = jcommon::StringUtils::ReplaceString(str, "&Emul;", "\xcf");
	str = jcommon::StringUtils::ReplaceString(str, "&ETH;", "\xd0");
	str = jcommon::StringUtils::ReplaceString(str, "&Ntilde;", "\xd1");
	str = jcommon::StringUtils::ReplaceString(str, "&Ograve;", "\xd2");
	str = jcommon::StringUtils::ReplaceString(str, "&Oacute;", "\xd3");
	str = jcommon::StringUtils::ReplaceString(str, "&Ocirc;", "\xd4");
	str = jcommon::StringUtils::ReplaceString(str, "&Otilde;", "\xd5");
	str = jcommon::StringUtils::ReplaceString(str, "&Omul;", "\xd6");
	str = jcommon::StringUtils::ReplaceString(str, "&times;", "\xd7");
	str = jcommon::StringUtils::ReplaceString(str, "&Oslash;", "\xd8");
	str = jcommon::StringUtils::ReplaceString(str, "&Ugrave;", "\xd9");
	str = jcommon::StringUtils::ReplaceString(str, "&Uacute;", "\xda");
	str = jcommon::StringUtils::ReplaceString(str, "&Ucirc;", "\xdb");
	str = jcommon::StringUtils::ReplaceString(str, "&Uuml;", "\xdc");
	str = jcommon::StringUtils::ReplaceString(str, "&Yacute;", "\xdd");
	str = jcommon::StringUtils::ReplaceString(str, "&THORN;", "\xde");
	str = jcommon::StringUtils::ReplaceString(str, "&szlig;", "\xdf");
	str = jcommon::StringUtils::ReplaceString(str, "&agrave;", "\xe0");
	str = jcommon::StringUtils::ReplaceString(str, "&aacute;", "\xe1");
	str = jcommon::StringUtils::ReplaceString(str, "&acirc;", "\xe2");
	str = jcommon::StringUtils::ReplaceString(str, "&atilde;", "\xe3");
	str = jcommon::StringUtils::ReplaceString(str, "&auml;", "\xe4");
	str = jcommon::StringUtils::ReplaceString(str, "&aring;", "\xe5");
	str = jcommon::StringUtils::ReplaceString(str, "&aelig;", "\xe6");
	str = jcommon::StringUtils::ReplaceString(str, "&ccedil;", "\xe7");
	str = jcommon::StringUtils::ReplaceString(str, "&egrave;", "\xe8");
	str = jcommon::StringUtils::ReplaceString(str, "&eacute;", "\xe9");
	str = jcommon::StringUtils::ReplaceString(str, "&ecirc;", "\xea");
	str = jcommon::StringUtils::ReplaceString(str, "&euml;", "\xeb");
	str = jcommon::StringUtils::ReplaceString(str, "&igrave;", "\xec");
	str = jcommon::StringUtils::ReplaceString(str, "&iacute;", "\xed");
	str = jcommon::StringUtils::ReplaceString(str, "&icirc;", "\xee");
	str = jcommon::StringUtils::ReplaceString(str, "&iuml;", "\xef");
	str = jcommon::StringUtils::ReplaceString(str, "&eth;", "\xf0");
	str = jcommon::StringUtils::ReplaceString(str, "&ntilde;", "\xf1");
	str = jcommon::StringUtils::ReplaceString(str, "&ograve;", "\xf2");
	str = jcommon::StringUtils::ReplaceString(str, "&oacute;", "\xf3");
	str = jcommon::StringUtils::ReplaceString(str, "&ocirc;", "\xf4");
	str = jcommon::StringUtils::ReplaceString(str, "&otilde;", "\xf5");
	str = jcommon::StringUtils::ReplaceString(str, "&ouml;", "\xf6");
	str = jcommon::StringUtils::ReplaceString(str, "&divide;", "\xf7");
	str = jcommon::StringUtils::ReplaceString(str, "&slash;", "\xf8");
	str = jcommon::StringUtils::ReplaceString(str, "&ugrave;", "\xf9");
	str = jcommon::StringUtils::ReplaceString(str, "&uacute;", "\xfa");
	str = jcommon::StringUtils::ReplaceString(str, "&ucirc;", "\xfb");
	str = jcommon::StringUtils::ReplaceString(str, "&uuml;", "\xfc");
	str = jcommon::StringUtils::ReplaceString(str, "&yacute;", "\xfd");
	str = jcommon::StringUtils::ReplaceString(str, "&thorn;", "\xfe");
	str = jcommon::StringUtils::ReplaceString(str, "&yuml;", "\xff");
	str = jcommon::StringUtils::ReplaceString(str, "&;", "\0");

	return str;
}

nclevent_symbol_t NCLHelper::TranslateKey(std::string key)
{
	nclevent_symbol_t symbol = NCL_SYMBOL_UNKNOWN;

	key = jcommon::StringUtils::Trim(jcommon::StringUtils::ToUpper(key));

	if (key == "0") {
		symbol = NCL_SYMBOL_0;
	} else if (key == "1") {
		symbol = NCL_SYMBOL_1;
	} else if (key == "2") {
		symbol = NCL_SYMBOL_2;
	} else if (key == "3") {
		symbol = NCL_SYMBOL_3;
	} else if (key == "4") {
		symbol = NCL_SYMBOL_4;
	} else if (key == "5") {
		symbol = NCL_SYMBOL_5;
	} else if (key == "6") {
		symbol = NCL_SYMBOL_6;
	} else if (key == "7") {
		symbol = NCL_SYMBOL_7;
	} else if (key == "8") {
		symbol = NCL_SYMBOL_8;
	} else if (key == "9") {
		symbol = NCL_SYMBOL_9;
	} else if (key == "MENU") {
		symbol = NCL_SYMBOL_MENU;
	} else if (key == "INFO") {
		symbol = NCL_SYMBOL_INFO;
	} else if (key == "GUIDE") {
		symbol = NCL_SYMBOL_GUIDE;
	} else if (key == "CURSOR_DOWN" || key == "VK_DOWN") {
		symbol = NCL_SYMBOL_CURSOR_DOWN;
	} else if (key == "CURSOR_LEFT" || key == "VK_LEFT") {
		symbol = NCL_SYMBOL_CURSOR_LEFT;
	} else if (key == "CURSOR_RIGHT" || key == "VK_RIGHT") {
		symbol = NCL_SYMBOL_CURSOR_RIGHT;
	} else if (key == "CURSOR_UP" || key == "VK_UP") {
		symbol = NCL_SYMBOL_CURSOR_UP;
	} else if (key == "CHANNEL_DOWN") {
		symbol = NCL_SYMBOL_CHANNEL_DOWN;
	} else if (key == "CHANNEL_UP") {
		symbol = NCL_SYMBOL_CHANNEL_UP;
	} else if (key == "VOLUME_DOWN") {
		symbol = NCL_SYMBOL_VOLUME_DOWN;
	} else if (key == "VOLUME_UP") {
		symbol = NCL_SYMBOL_VOLUME_UP;
	} else if (key == "ENTER") {
		symbol = NCL_SYMBOL_ENTER;
	} else if (key == "RED") {
		symbol = NCL_SYMBOL_RED;
	} else if (key == "GREEN") {
		symbol = NCL_SYMBOL_GREEN;
	} else if (key == "YELLOW") {
		symbol = NCL_SYMBOL_YELLOW;
	} else if (key == "BLUE") {
		symbol = NCL_SYMBOL_BLUE;
	} else if (key == "BACK") {
		symbol = NCL_SYMBOL_BACK;
	} else if (key == "EXIT") {
		symbol = NCL_SYMBOL_EXIT;
	} else if (key == "POWER") {
		symbol = NCL_SYMBOL_POWER;
	} else if (key == "REWIND") {
		symbol = NCL_SYMBOL_REWIND;
	} else if (key == "STOP") {
		symbol = NCL_SYMBOL_STOP;
	} else if (key == "EJECT") {
		symbol = NCL_SYMBOL_EJECT;
	} else if (key == "PLAY") {
		symbol = NCL_SYMBOL_PLAY;
	} else if (key == "RECORD") {
		symbol = NCL_SYMBOL_RECORD;
	} else if (key == "PAUSE") {
		symbol = NCL_SYMBOL_PAUSE;
	} else if (key == "A") {
		symbol = NCL_SYMBOL_A;
	} else if (key == "B") {
		symbol = NCL_SYMBOL_B;
	} else if (key == "C") {
		symbol = NCL_SYMBOL_C;
	} else if (key == "D") {
		symbol = NCL_SYMBOL_D;
	} else if (key == "E") {
		symbol = NCL_SYMBOL_E;
	} else if (key == "F") {
		symbol = NCL_SYMBOL_F;
	} else if (key == "G") {
		symbol = NCL_SYMBOL_G;
	} else if (key == "H") {
		symbol = NCL_SYMBOL_H;
	} else if (key == "I") {
		symbol = NCL_SYMBOL_I;
	} else if (key == "J") {
		symbol = NCL_SYMBOL_J;
	} else if (key == "K") {
		symbol = NCL_SYMBOL_K;
	} else if (key == "L") {
		symbol = NCL_SYMBOL_L;
	} else if (key == "M") {
		symbol = NCL_SYMBOL_M;
	} else if (key == "N") {
		symbol = NCL_SYMBOL_N;
	} else if (key == "O") {
		symbol = NCL_SYMBOL_O;
	} else if (key == "P") {
		symbol = NCL_SYMBOL_P;
	} else if (key == "Q") {
		symbol = NCL_SYMBOL_Q;
	} else if (key == "R") {
		symbol = NCL_SYMBOL_R;
	} else if (key == "S") {
		symbol = NCL_SYMBOL_S;
	} else if (key == "T") {
		symbol = NCL_SYMBOL_T;
	} else if (key == "U") {
		symbol = NCL_SYMBOL_U;
	} else if (key == "V") {
		symbol = NCL_SYMBOL_V;
	} else if (key == "W") {
		symbol = NCL_SYMBOL_W;
	} else if (key == "X") {
		symbol = NCL_SYMBOL_X;
	} else if (key == "Y") {
		symbol = NCL_SYMBOL_Y;
	} else if (key == "Z") {
		symbol = NCL_SYMBOL_Z;
	} else if (key == "*") {
		symbol = NCL_SYMBOL_ASTERISTIC;
	} else if (key == "#") {
		symbol = NCL_SYMBOL_SHARP;
	}

	return symbol;
}

nclevent_symbol_t NCLHelper::TranslateKey(jevent::jkeyevent_symbol_t key)
{
	nclevent_symbol_t symbol = NCL_SYMBOL_UNKNOWN;

	if (key == jevent::JKS_0) {
		symbol = NCL_SYMBOL_0;
	} else if (key == jevent::JKS_1) {
		symbol = NCL_SYMBOL_1;
	} else if (key == jevent::JKS_2) {
		symbol = NCL_SYMBOL_2;
	} else if (key == jevent::JKS_3) {
		symbol = NCL_SYMBOL_3;
	} else if (key == jevent::JKS_4) {
		symbol = NCL_SYMBOL_4;
	} else if (key == jevent::JKS_5) {
		symbol = NCL_SYMBOL_5;
	} else if (key == jevent::JKS_6) {
		symbol = NCL_SYMBOL_6;
	} else if (key == jevent::JKS_7) {
		symbol = NCL_SYMBOL_7;
	} else if (key == jevent::JKS_8) {
		symbol = NCL_SYMBOL_8;
	} else if (key == jevent::JKS_9) {
		symbol = NCL_SYMBOL_9;
	} else if (key == jevent::JKS_STAR) {
		symbol = NCL_SYMBOL_ASTERISTIC;
	} else if (key == jevent::JKS_SHARP) {
		symbol = NCL_SYMBOL_SHARP;
	} else if (key == jevent::JKS_MENU) {
		symbol = NCL_SYMBOL_MENU;
	} else if (key == jevent::JKS_INFO) {
		symbol = NCL_SYMBOL_INFO;
	} else if (key == jevent::JKS_GUIDE) {
		symbol = NCL_SYMBOL_GUIDE;
	} else if (key == jevent::JKS_CURSOR_DOWN) {
		symbol = NCL_SYMBOL_CURSOR_DOWN;
	} else if (key == jevent::JKS_CURSOR_LEFT) {
		symbol = NCL_SYMBOL_CURSOR_LEFT;
	} else if (key == jevent::JKS_CURSOR_RIGHT) {
		symbol = NCL_SYMBOL_CURSOR_RIGHT;
	} else if (key == jevent::JKS_CURSOR_UP) {
		symbol = NCL_SYMBOL_CURSOR_UP;
	} else if (key == jevent::JKS_CHANNEL_DOWN) {
		symbol = NCL_SYMBOL_CHANNEL_DOWN;
	} else if (key == jevent::JKS_CHANNEL_UP) {
		symbol = NCL_SYMBOL_CHANNEL_UP;
	} else if (key == jevent::JKS_VOLUME_DOWN) {
		symbol = NCL_SYMBOL_VOLUME_DOWN;
	} else if (key == jevent::JKS_VOLUME_UP) {
		symbol = NCL_SYMBOL_VOLUME_UP;
	} else if (key == jevent::JKS_ENTER) {
		symbol = NCL_SYMBOL_ENTER;
	} else if (key == jevent::JKS_RED || key == jevent::JKS_F1) {
		symbol = NCL_SYMBOL_RED;
	} else if (key == jevent::JKS_GREEN || key == jevent::JKS_F2) {
		symbol = NCL_SYMBOL_GREEN;
	} else if (key == jevent::JKS_YELLOW || key == jevent::JKS_F3) {
		symbol = NCL_SYMBOL_YELLOW;
	} else if (key == jevent::JKS_BLUE || key == jevent::JKS_F4) {
		symbol = NCL_SYMBOL_BLUE;
	} else if (key == jevent::JKS_BACK) {
		symbol = NCL_SYMBOL_BACK;
	} else if (key == jevent::JKS_EXIT) {
		symbol = NCL_SYMBOL_EXIT;
	} else if (key == jevent::JKS_POWER) {
		symbol = NCL_SYMBOL_POWER;
	} else if (key == jevent::JKS_REWIND) {
		symbol = NCL_SYMBOL_REWIND;
	} else if (key == jevent::JKS_STOP) {
		symbol = NCL_SYMBOL_STOP;
	} else if (key == jevent::JKS_EJECT) {
		symbol = NCL_SYMBOL_EJECT;
	} else if (key == jevent::JKS_PLAY) {
		symbol = NCL_SYMBOL_PLAY;
	} else if (key == jevent::JKS_RECORD) {
		symbol = NCL_SYMBOL_RECORD;
	} else if (key == jevent::JKS_PAUSE) {
		symbol = NCL_SYMBOL_PAUSE;
	} else if (key == jevent::JKS_ESCAPE) {
		symbol = NCL_SYMBOL_EXIT;
	} else if (key == jevent::JKS_A) {
		symbol = NCL_SYMBOL_A;
	} else if (key == jevent::JKS_B) {
		symbol = NCL_SYMBOL_B;
	} else if (key == jevent::JKS_C) {
		symbol = NCL_SYMBOL_C;
	} else if (key == jevent::JKS_D) {
		symbol = NCL_SYMBOL_D;
	} else if (key == jevent::JKS_E) {
		symbol = NCL_SYMBOL_E;
	} else if (key == jevent::JKS_F) {
		symbol = NCL_SYMBOL_F;
	} else if (key == jevent::JKS_G) {
		symbol = NCL_SYMBOL_G;
	} else if (key == jevent::JKS_H) {
		symbol = NCL_SYMBOL_H;
	} else if (key == jevent::JKS_I) {
		symbol = NCL_SYMBOL_I;
	} else if (key == jevent::JKS_J) {
		symbol = NCL_SYMBOL_J;
	} else if (key == jevent::JKS_K) {
		symbol = NCL_SYMBOL_K;
	} else if (key == jevent::JKS_L) {
		symbol = NCL_SYMBOL_L;
	} else if (key == jevent::JKS_M) {
		symbol = NCL_SYMBOL_M;
	} else if (key == jevent::JKS_N) {
		symbol = NCL_SYMBOL_N;
	} else if (key == jevent::JKS_O) {
		symbol = NCL_SYMBOL_O;
	} else if (key == jevent::JKS_P) {
		symbol = NCL_SYMBOL_P;
	} else if (key == jevent::JKS_Q) {
		symbol = NCL_SYMBOL_Q;
	} else if (key == jevent::JKS_R) {
		symbol = NCL_SYMBOL_R;
	} else if (key == jevent::JKS_S) {
		symbol = NCL_SYMBOL_S;
	} else if (key == jevent::JKS_T) {
		symbol = NCL_SYMBOL_T;
	} else if (key == jevent::JKS_U) {
		symbol = NCL_SYMBOL_U;
	} else if (key == jevent::JKS_V) {
		symbol = NCL_SYMBOL_V;
	} else if (key == jevent::JKS_W) {
		symbol = NCL_SYMBOL_W;
	} else if (key == jevent::JKS_X) {
		symbol = NCL_SYMBOL_X;
	} else if (key == jevent::JKS_Y) {
		symbol = NCL_SYMBOL_Y;
	} else if (key == jevent::JKS_Z) {
		symbol = NCL_SYMBOL_Z;
	}

	return symbol;
}

nclmimetype_t NCLHelper::GetMimeTypeByID(std::string id)
{
	nclmimetype_t mime = NCL_MIME_UNKNOWN;

	id = jcommon::StringUtils::Trim(jcommon::StringUtils::ToLower(id));

	// INFO:: handle genric type of medias
	if (id == "text") {
		id = "text/plain";
	} else if (id == "image") {
		id = "image/png";
	} else if (id == "audio") {
		id = "audio/mp3";
	} else if (id == "video") {
		id = "video/mpeg";
	} else if (id == "lua" || id == "nclua") {
		id = "application/x-ginga-nclua";
	} else if (id == "nclet") {
		id = "application/x-ginga-nclet";
	} else if (id == "settings") {
		id = "application/x-ginga-settings";
	} else if (id == "time") {
		id = "application/x-ginga-time";
	}
	
	if (id == "html" || id == "text/html") {
		mime = NCL_MIME_HTML;
	} else if (id == "wml" || id == "text/wml") {
		mime = NCL_MIME_WML;
	} else if (id == "plain" || id == "text/plain") {
		mime = NCL_MIME_PLAIN;
	} else if (id == "css" || id == "text/css") {
		mime = NCL_MIME_CSS;
	} else if (id == "xml" || id == "text/xml") {
		mime = NCL_MIME_XML;
	} else if (id == "bmp" || id == "image/bmp") {
		mime = NCL_MIME_BMP;
	} else if (id == "png" || id == "image/png") {
		mime = NCL_MIME_PNG;
	} else if (id == "gif" || id == "image/gif") {
		mime = NCL_MIME_GIF;
	} else if (id == "jpeg" || id == "image/jpeg") {
		mime = NCL_MIME_JPG;
	} else if (id == "basic" || id == "audio/basic") {
		mime = NCL_MIME_BASIC_AUDIO;
	} else if (id == "mp3" || id == "audio/mp3") {
		mime = NCL_MIME_MP3;
	} else if (id == "mp2" || id == "audio/mp2") {
		mime = NCL_MIME_MP2;
	} else if (id == "audio/mpg" || id == "audio/mpeg") {
		mime = NCL_MIME_MPEG_AUDIO;
	} else if (id == "mpg" || id == "mpeg" || id == "video/mpg" || id == "video/mpeg") {
		mime = NCL_MIME_MPEG_VIDEO;
	} else if (id == "mpg4" || id == "mpeg4" || id == "audio/mpg4" || id == "audio/mpeg4") {
		mime = NCL_MIME_MPEG4;
	} else if (id == "mov" || id == "video/mov") {
		mime = NCL_MIME_MOV;
	} else if (id == "x-ginga-nclua" || id == "application/x-ginga-nclua") {
		mime = NCL_MIME_LUA;
	} else if (id == "x-ginga-nclet" || id == "application/x-ginga-nclet") {
		mime = NCL_MIME_NCLET;
	} else if (id == "x-ginga-settings" || id == "application/x-ginga-settings") {
		mime = NCL_MIME_SETTINGS;
	} else if (id == "x-ginga-time" || id == "application/x-ginga-time") {
		mime = NCL_MIME_TIME;
	}

	return mime;
}

nclmimetype_t NCLHelper::GetMimeTypeByExtension(std::string file)
{
	file = jcommon::StringUtils::Trim(jcommon::StringUtils::ToLower(file));
	
	if (file.find("isdtv-ts://") == 0 || file.find("sbtvd-ts://") == 0) {
		return NCL_MIME_MPEG_VIDEO;
	}

	if (file.find(".") == std::string::npos) {
		return NCL_MIME_UNKNOWN;
	}
	
	std::string type,
		id = file.substr(file.find(".")+1);

	if (id == "gif") {
		type = "image/gif";
	} else if (id == "png") {
		type = "image/png";
	} else if (id == "bmp") {
		type = "image/bmp";
	} else if (id == "jpg" || id == "jpeg") {
		type = "image/jpeg";
	} else if (id == "htm" || id == "html" || id.find("htm#") == 0 || id.find("html#") == 0) {
		type = "text/html";
	} else if (id == "wml") {
		type = "text/wml";
	} else if (id == "txt") {
		type = "text/plain";
	} else if (id == "css") {
		type = "text/css";
	} else if (id == "xml") {
		type = "text/xml";
	} else if (id == "wav") {
		type = "audio/basic";
	} else if (id == "mp3") {
		type = "audio/mp3";
	} else if (id == "mp2") {
		type = "audio/mp2";	
	} else if (id == "mpg" || id == "mpeg") {
		type = "video/mpeg";
	} else if (id == "mp4" || id == "mpg4") {
		type = "audio/mpeg4";
	} else if (id == "mov") {
		type = "video/mov";
	} else if (id == "lua") {
		type = "application/x-ginga-nclua";
	} else if (id == "xlt" || id == "xlet" || id == "class") {
		type = "application/x-ginga-nclet";
	}

	return GetMimeTypeByID(type);
}

}

