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
#ifndef NCLLUAOBJECT_H
#define NCLLUAOBJECT_H

#include <string>
#include <map>

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

#define lua_boxpointer(L,u) \
	        (*(void **)(lua_newuserdata(L, sizeof(void *))) = (u))

#define lua_unboxpointer(L,i)   (*(void **)(lua_touserdata(L, i)))

#define method(class, name) {#name, class::name}

class NCLLuaObjectBinding{

	private:

	private:
		NCLLuaObjectBinding();

	public:
		virtual ~NCLLuaObjectBinding();

		static void * GetPointer(lua_State *L, int narg, const char *tname, int type = LUA_TUSERDATA);

};

#endif
