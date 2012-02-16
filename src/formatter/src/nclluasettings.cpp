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
#include "nclluasettings.h"

NCLLuaSettings *NCLLuaSettings::_instance = NULL;

NCLLuaSettings::NCLLuaSettings()
{
}

NCLLuaSettings::~NCLLuaSettings()
{
}

NCLLuaSettings * NCLLuaSettings::GetInstance()
{
	if (_instance == NULL) {
		_instance = new NCLLuaSettings();
	}

	return _instance;
}

void NCLLuaSettings::SetProperty(std::string key, std::string value)
{
	_settings[key] = value;
}

std::string NCLLuaSettings::GetProperty(std::string key)
{
	return _settings[key];
}

