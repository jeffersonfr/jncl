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
#include "nclluaplayer.h"
#include "nclluacanvas.h"
#include "nclluaevent.h"
#include "nclluasettings.h"
#include "nclenviroment.h"
#include "videoplayer.h"

#include <sstream>

#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

#define LUA_GLOBALSINDEX	100

#define lua_boxpointer(L,u) \
	        (*(void **)(lua_newuserdata(L, sizeof(void *))) = (u))

#define lua_unboxpointer(L,i)   (*(void **)(lua_touserdata(L, i)))

#define method(class, name) {#name, class::name}

#define LUA_REENTRANT

struct Smain {
	int argc;
	char **argv;
	int status;
};

lua_State *globalL = NULL;

static int traceback (lua_State *L) {
	if (!lua_isstring(L, 1))  /* 'message' not a string? */
		return 1;  /* keep it intact */
	lua_getfield(L, LUA_GLOBALSINDEX, "debug");
	if (!lua_istable(L, -1)) {
		lua_pop(L, 1);
		return 1;
	}
	lua_getfield(L, -1, "traceback");
	if (!lua_isfunction(L, -1)) {
		lua_pop(L, 2);
		return 1;
	}
	lua_pushvalue(L, 1);  /* pass error message */
	lua_pushinteger(L, 2);  /* skip this function and traceback */
	lua_call(L, 2, 1);  /* call debug.traceback */
	return 1;
}

static void lstop (lua_State *L, lua_Debug *ar) {
	(void)ar;  /* unused arg. */
	lua_sethook(L, NULL, 0, 0);
	luaL_error(L, "interrupted!");
}

static void laction (int i) 
{
	signal(i, SIG_DFL); // if another SIGINT happens before lstop, terminate process (default action) 
	lua_sethook(globalL, lstop, LUA_MASKCALL | LUA_MASKRET | LUA_MASKCOUNT, 1);
}

static int docall (lua_State *L, int narg, int clear) 
{
	int status = -1;
	int base = lua_gettop(L) - narg;  /* function index */
	lua_pushcfunction(L, traceback);  /* push traceback function */
	lua_insert(L, base);  /* put it under chunk and args */
	signal(SIGINT, laction);
	lua_pcall(L, narg, (clear ? 0 : LUA_MULTRET), base);
	// lua_call(L, narg, (clear ? 0 : LUA_MULTRET));
	signal(SIGINT, SIG_DFL);
	lua_remove(L, base);  /* remove traceback function */
	/* force a complete garbage collection in case of errors */
	if (status != 0) lua_gc(L, LUA_GCCOLLECT, 0);
	return status;
}


namespace jncl {

NCLLuaPlayer::NCLLuaPlayer(NCLEnviroment *env, struct nclmedia_t *media):
	NCLPlayer(media, NCL_PLAYER_AUDIO)
{
	puts("NCLPlayer id=image create\n");

	_enviroment = env;

	struct ncldescriptor_t *descriptor = _media->descriptor;
	struct nclregion_t *region = descriptor->region;

	int border = descriptor->focusborderwidth;

	if (border < 0) {
		border = 0;
	}

	_filename = _enviroment->GetBaseDirectory() + "/" + _media->src;
	
	lua_State *L = NULL;

	L = luaL_newstate();

	if (L == NULL) {
		return;
	}

	globalL = L;

	luaL_openlibs(L);

	NCLLuaSettings::GetInstance()->SetProperty("lua.basedirectory", _enviroment->GetBaseDirectory() + "/");

	NCLLuaEventBinding::Register(L);
	NCLLuaCanvasBinding::Register(L, region->left+border, region->top+border, region->width+border, region->height+border);

	lua_gc(L, LUA_GCSTOP, 0);	// stop collector during initialization 
	lua_gc(L, LUA_GCRESTART, 0);

	std::string path = _enviroment->GetBaseDirectory() + "/";

	// initialize random seed
	char tmp[255];

	sprintf(tmp, "\nmath.randomseed(%d)\n", (int)time(NULL));

	luaL_dostring(L, tmp);

	// setting package.path
	std::string package_path = "package.path = package.path .. ;" + path + "?.lua";

	if (luaL_dostring(L, package_path.c_str()) != 0) {
		lua_pop(L,1);
	}

	// TODO:: usar chdir() em vez de mudar o codigo de lua

	// setting io redirect
	std::ostringstream o;
   
	o << "require 'io'" << "\r\n" 
	  << "function set_basedirectory(dir)" << "\r\n"
	  << "dir = dir .. \"/\"" << "\r\n"
	  << "original_open = io.open function new_open(filename, mode) return original_open(dir .. filename, mode ) end io.open = new_open" << "\r\n"
	  << "original_lines = io.lines function new_lines(filename, mode) return original_lines(dir .. filename, mode ) end io.lines = new_lines" << "\r\n"
	  << "original_dofile = dofile function new_dofile(filename) return original_dofile(dir .. filename ) end dofile = new_dofile" << "\r\n"
	  << "original_input = io.input function new_input(filename)" << "\r\n"
	  << "if (filename) then return original_input(dir .. filename) else return original_input() end" << "\r\n"
	  << "end io.input = new_input" << "\r\n"
	  << "end" << "\r\n"
	  << "set_basedirectory(\"" << path << "\")" << std::endl;
	
	if (luaL_dostring(L, o.str().c_str()) != 0) {
		lua_pop(L,1);
	}
}

NCLLuaPlayer::~NCLLuaPlayer()
{
}

void NCLLuaPlayer::Hide()
{
	puts("NCLPlayer id=image hide\n");
}

void NCLLuaPlayer::Play()
{
	printf("NCLPlayer id=image src=%s play\n", _media->src.c_str());

	if (luaL_loadfile(globalL, _filename.c_str()) == 0) {
		// call method
		if (docall(globalL, 0, 1) != 0) {
			printf("NCLPlayer id=image src=%s play error\n", _media->src.c_str());
		}
	}

	_is_alive = true;
}

void NCLLuaPlayer::Stop()
{
	printf("NCLPlayer id=image src=%s stop\n", _media->src.c_str());

	_component->Hide();

	lua_gc(globalL, LUA_GCCOLLECT, 0);
	lua_close(globalL);

	/* TODO:: resolver
	media::VideoPlayer *player = controller::Controller::GetInstance()->GetVideoPlayer();

	if (player != NULL) {
		player->GetComponent()->SetBounds(0, 0, 1920, 1080);
	}
	*/

	_is_alive = false;
}

void NCLLuaPlayer::Pause()
{
	puts("NCLPlayer id=image pause\n");

	_is_alive = false;
}

void NCLLuaPlayer::Resume()
{
	puts("NCLPlayer id=image resume\n");

	Play();
}

void NCLLuaPlayer::Abort()
{
	puts("NCLPlayer id=image abort\n");

	_is_alive = false;
}

void NCLLuaPlayer::Release()
{
	puts("NCLPlayer id=image release\n");

	_is_alive = false;
}

}

