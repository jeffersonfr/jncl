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
#include "nclluaevent.h"
#include "nclselectionevent.h"
#include "nclinput.h"

#include "jautolock.h"
#include "jdate.h"

#include <sstream>

NCLLuaEventBinding::NCLLuaEventBinding()
{
	jncl::NCLInput::GetInstance()->RegisterEventListener(this);
}

NCLLuaEventBinding::~NCLLuaEventBinding()
{
	jncl::NCLInput::GetInstance()->RemoveEventListener(this);

	// jthread::AutoLock lock(&event_mutex);
}

void NCLLuaEventBinding::RegisterHandler(lua_State* L)
{
	if (lua_isfunction (L, 1)) {
		_handlers.push_back(luaL_ref(L, LUA_REGISTRYINDEX));
	}
}

void NCLLuaEventBinding::UnregisterHandler(lua_State* L)
{
	// jthread::AutoLock lock(&event_mutex);

	if (lua_type(L, 1) != LUA_TFUNCTION) {
		return;
	}

	int func = lua_checkstack(L, 1);

	luaL_unref(L, LUA_REGISTRYINDEX, func);

	for (std::vector<int>::iterator i=_handlers.begin(); i!=_handlers.end(); i++) {
		if ((*i) == func) {
			_handlers.erase(i);

			break;
		}
	}

	// jgui::InputManager::GetInstance()->WaitEvents();
}

void NCLLuaEventBinding::ActionPerformed(jncl::NCLEvent *nclevent)
{
	puts("event");
	// jthread::AutoLock lock(&event_mutex);

	if (nclevent->GetType() != jncl::NCL_EVENT_SELECTION) {
		return;
	}
		
	jncl::NCLSelectionEvent *event = (jncl::NCLSelectionEvent *)nclevent;

	// jncl::nclevent_symbol_t key;
	std::string symbol;

	// key = event->GetSymbol();

	if (event->GetSymbol() == jncl::NCL_SYMBOL_CURSOR_LEFT) {
		symbol = "CURSOR_LEFT";
	} else if (event->GetSymbol() == jncl::NCL_SYMBOL_CURSOR_RIGHT) {
		symbol = "CURSOR_RIGHT";
	} else if (event->GetSymbol() == jncl::NCL_SYMBOL_CURSOR_UP) {
		symbol = "CURSOR_UP";
	} else if (event->GetSymbol() == jncl::NCL_SYMBOL_CURSOR_DOWN) {
		symbol = "CURSOR_DOWN";
	} else if (event->GetSymbol() == jncl::NCL_SYMBOL_1) {
		symbol = "1";
	} else if (event->GetSymbol() == jncl::NCL_SYMBOL_2) {
		symbol = "2";
	} else if (event->GetSymbol() == jncl::NCL_SYMBOL_3) {
		symbol = "3";
	} else if (event->GetSymbol() == jncl::NCL_SYMBOL_4) {
		symbol = "4";
	} else if (event->GetSymbol() == jncl::NCL_SYMBOL_5) {
		symbol = "5";
	} else if (event->GetSymbol() == jncl::NCL_SYMBOL_6) {
		symbol = "6";
	} else if (event->GetSymbol() == jncl::NCL_SYMBOL_7) {
		symbol = "7";
	} else if (event->GetSymbol() == jncl::NCL_SYMBOL_8) {
		symbol = "8";
	} else if (event->GetSymbol() == jncl::NCL_SYMBOL_9) {
		symbol = "9";
	} else if (event->GetSymbol() == jncl::NCL_SYMBOL_0) {
		symbol = "0";
	} else if (event->GetSymbol() == jncl::NCL_SYMBOL_ENTER) {
		symbol = "enter";
	}

	if (symbol != "") {
		for (std::vector<int>::iterator i=_handlers.begin(); i!=_handlers.end(); i++) {
			/*
			lua_rawgeti(globalL, LUA_REGISTRYINDEX, (*i));

			lua_newtable(globalL);

			lua_pushstring(globalL, "class");
			lua_pushstring(globalL, "key");
			lua_settable(globalL, -3);

			lua_pushstring(globalL, "type");
			lua_pushstring(globalL, "press");
			lua_settable(globalL, -3);

			lua_pushstring(globalL, "key");
			lua_pushstring(globalL, "CURSOR_LEFT");
			lua_settable(globalL, -3);

			printf("--> %d\n", lua_pcall(globalL, 1, 0, 0));

			lua_pop(globalL, 1);
			*/
		}
	}
}

void NCLLuaEventBinding::Register(lua_State* L) 
{
	lua_newtable(L);                 int methodtable = lua_gettop(L);
	luaL_newmetatable(L, className); int metatable   = lua_gettop(L);

	lua_pushliteral(L, "__metatable");
	lua_pushvalue(L, methodtable);
	lua_settable(L, metatable);  // hide metatable from NCLLua getmetatable()

	lua_pushliteral(L, "__index");
	lua_pushvalue(L, methodtable);
	lua_settable(L, metatable);

	lua_pushliteral(L, "__gc");
	lua_pushcfunction(L, _lua_gc);
	lua_settable(L, metatable);

	lua_pop(L, 1);  // drop metatable

	// luaL_register(L, 0, methods);  // fill methodtable
	// luaL_register(L, className, methods);  // fill methodtable

	lua_pop(L, 1);  // drop methodtable

	// lua_register(L, className, _lua_create);
	
	NCLLuaEventBinding *c = new NCLLuaEventBinding();

	lua_boxpointer(L, c);
	luaL_getmetatable(L, className);
	lua_setmetatable(L, -2);
	lua_setglobal(L, className);
}

int NCLLuaEventBinding::_lua_create(lua_State *L)
{
	NCLLuaEventBinding *c = new NCLLuaEventBinding();

	lua_boxpointer(L, c);
	luaL_getmetatable(L, className);
	lua_setmetatable(L, -2);

	return 1;
}

int NCLLuaEventBinding::_lua_gc(lua_State *L)
{
	// NCLLuaEventBinding *a = (NCLLuaEventBinding *)lua_unboxpointer(L, 1);

	// delete a;

	return 0;
}

int NCLLuaEventBinding::aregister(lua_State *L)
{
	lua_getglobal(L, className);

	NCLLuaEventBinding *a = *(NCLLuaEventBinding **)lua_touserdata(L, -1);

	a->RegisterHandler(L);

	lua_pop(L, 1);

	return 0;
}

int NCLLuaEventBinding::unregister(lua_State *L)
{
	lua_getglobal(L, className);

	NCLLuaEventBinding *a = (NCLLuaEventBinding *)lua_touserdata(L, -1);

	a->UnregisterHandler(L);

	lua_pop(L, 1);

	return 0;
}

int NCLLuaEventBinding::post(lua_State *L)
{
	NCLLuaEventBinding *a = *((NCLLuaEventBinding **)NCLLuaObjectBinding::GetPointer(L, 1, className));
	const char *name = (char*)luaL_checkstring(L, 2);

	if (strcasecmp(name, "in") == 0) {
		for (std::vector<int>::iterator i=a->_handlers.begin(); i!=a->_handlers.end(); i++) {
			lua_rawgeti(L, LUA_REGISTRYINDEX, (*i));

			/*
			lua_newtable(L);

			lua_pushstring(L, "class");
			lua_pushstring(L, "user");
			lua_settable(L, -3);

			lua_pushstring(L, "type");
			lua_pushstring(L, "press");
			lua_settable(L, -3);
			*/

			lua_pushvalue(L, -2);

			lua_pcall(L, 1, 0, 0);
		}
	} else if (strcasecmp(name, "out") == 0) {
	}

	return 0;
}

int NCLLuaEventBinding::uptime(lua_State *L)
{
	lua_pushnumber(L, jcommon::Date::CurrentTimeMillis());

	return 1;
}

int NCLLuaEventBinding::timer(lua_State *L)
{
	return 0;
}

int NCLLuaEventBinding::delay(lua_State *L)
{
	int x = (int)luaL_checknumber(L, 2)*1000;

	usleep(x);

	return 0;
}

const char NCLLuaEventBinding::className[] = "event";

#define method(class, name) {#name, class::name}

const luaL_Reg NCLLuaEventBinding::methods[] = {
	{"register", NCLLuaEventBinding::aregister},
	{"unregister", NCLLuaEventBinding::unregister},
	{"post", NCLLuaEventBinding::post},
	{"uptime", NCLLuaEventBinding::uptime},
	{"timer", NCLLuaEventBinding::timer},
	{"delay", NCLLuaEventBinding::delay},
	{0,0}
};

