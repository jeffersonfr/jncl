/***************************************************************************
 *   Copyright (C) 2005 by Jeff Ferr                                       *
 *   root@sat                                                              *
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
#include "nclenviroment.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

namespace jncl {

NCLEnviroment::NCLEnviroment()
{
	Reset();
}

NCLEnviroment::~NCLEnviroment()
{
}

void NCLEnviroment::Reset()
{
	_maindocument = "";
	_basedirectory = "";
	_width = 0;
	_height = 0;

	char tmp[255];

	sprintf(tmp, "%d, %d", _width, _height);

	// pg.20; NCL3.0-DTV
	_params["system.language"] = "ISO 639-1";
	_params["system.caption"] = "ISO 639";
	_params["system.subtitle"] = "ISO 639";
	_params["system.returnBitRate(0)"] = "";
	_params["system.screenSize (0)"] = tmp;
	_params["system.screenGraphicSize (0)"] = tmp;
	_params["system.audioType(i)"] = "stereo";
	_params["system.CPU"] = "1000";
	_params["system.memory"] = "256";
	_params["system.operatingSystem"] = "Linux";
	_params["system.javaConfiguration"] = "";
	_params["system.javaProfile"] = "";
	_params["system.luaVersion"] = "2.0.3 alpha";

	_params["user.age"] = "";
	_params["user.location"] = "";
	_params["user.genre"] = "";

	_params["default.focusBorderColor"] = "white";
	_params["default.selBorderColor"] = "blue";
	_params["default.focusBorderWidth"] = "4";
	_params["default.focusTransparency"] = "0%";
	
	_params["service.currentFocus"] = "";
	_params["service.currentKeyMaster"] = "";

	_params["channel.xxx"] = "";

	_params["shared.xxx"] = "";
}
	
void NCLEnviroment::SetMainDocument(std::string str)
{
	_maindocument = str;
}

void NCLEnviroment::SetBaseDirectory(std::string str)
{
	_basedirectory = str;
}

std::string NCLEnviroment::GetMainDocument()
{
	return _maindocument;
}

std::string NCLEnviroment::GetBaseDirectory()
{
	return _basedirectory;
}

void NCLEnviroment::SetWidth(int width)
{
	_width = width;
}

void NCLEnviroment::SetHeight(int height)
{
	_height = height;
}

int NCLEnviroment::GetWidth()
{
	return _width;
}

int NCLEnviroment::GetHeight()
{
	return _height;
}

void NCLEnviroment::SetProperty(std::string key, std::string value)
{
	_params[key] = value;
}

std::string NCLEnviroment::GetProperty(std::string key)
{
	std::map<std::string, std::string>::iterator i = _params.find(key);

	if (i != _params.end()) {
		return i->second;
	}

	return "";
}
	
}

