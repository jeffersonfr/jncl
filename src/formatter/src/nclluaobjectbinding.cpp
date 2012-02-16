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
#include "nclluaobjectbinding.h"

NCLLuaObjectBinding::NCLLuaObjectBinding()
{
}

NCLLuaObjectBinding::~NCLLuaObjectBinding()
{
}

void * NCLLuaObjectBinding::GetPointer(lua_State *L, int narg, const char *tname, int type)
{
	if (lua_type(L, narg) == LUA_TNONE || lua_type(L, narg) == LUA_TNIL || lua_type(L, narg) != type) {
		return NULL;
	}

	luaL_checktype(L, narg, type);

	void *p = lua_touserdata(L, narg);
	if (p != NULL) {						// value is a userdata? 
		if (lua_getmetatable(L, narg)) {	// does it have a metatable? 
			lua_getfield(L, LUA_REGISTRYINDEX, tname);	// get correct metatable 
			if (lua_rawequal(L, -1, -2)) {	// does it have the correct mt? 
				lua_pop(L, 2);				// remove both metatables 

				return p;
			} else {
				lua_pop(L, 2);				// remove both metatables 
			}
		}
	}
	return NULL;	// to avoid warnings
}

