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
#ifndef NCLLUAEVENT_H
#define NCLLUAEVENT_H

#include "nclluaobjectbinding.h"
#include "ncleventlistener.h"

#include "jgui/jwindow.h"

#include <iostream>
#include <string>
#include <vector>
#include <mutex>

#include <stdio.h>
#include <stdlib.h>

class NCLLuaEventBinding : public jncl::NCLEventListener {

	public:
		std::mutex event_mutex;
		std::vector<int> _handlers;
		lua_State *_lua;

	public:
		static const char className[];
		static const luaL_Reg methods[];

	public:
		NCLLuaEventBinding();
		virtual ~NCLLuaEventBinding();

		void RegisterHandler(lua_State* L);
		void UnregisterHandler(lua_State* L);

		virtual void ActionPerformed(jncl::NCLEvent *event);

		static void Register(lua_State* L);
	
		static int _lua_create(lua_State *L);
		static int _lua_gc(lua_State *L);

		static int aregister(lua_State *L);
		static int unregister(lua_State *L);
		static int post(lua_State *L);
		static int uptime(lua_State *L);
		static int timer(lua_State *L);
		static int delay(lua_State *L);

};

#endif
