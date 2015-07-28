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

#define LUA_REENTRANT

extern "C" {
	#include "lua.h"
	#include "lauxlib.h"
	#include "lualib.h"
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
	
	_luaL = luaL_newstate();

	if (_luaL == NULL) {
		return;
	}

	luaL_openlibs(_luaL);

	NCLLuaSettings::GetInstance()->SetProperty("lua.basedirectory", _enviroment->GetBaseDirectory() + "/");

	NCLLuaEventBinding::Register(_luaL);
	NCLLuaCanvasBinding::Register(_luaL, region->left+border, region->top+border, region->width+border, region->height+border);

	// stop collector during initialization 
	lua_gc(_luaL, LUA_GCSTOP, 0);	
	lua_gc(_luaL, LUA_GCRESTART, 0);

	std::string path = _enviroment->GetBaseDirectory() + "/";

	// initialize random seed
	char tmp[255];

	sprintf(tmp, "\nmath.randomseed(%d)\n", (int)time(NULL));

	luaL_dostring(_luaL, tmp);

	// setting package.path
	std::string package_path = "package.path = package.path .. ;" + path + "?.lua";

	if (luaL_dostring(_luaL, package_path.c_str()) != 0) {
		lua_pop(_luaL, 1);
	}

	// INFO:: setting io redirect
	// CHANGE :: usar chdir() em vez de mudar o codigo de lua
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
	
	if (luaL_dostring(_luaL, o.str().c_str()) != 0) {
		lua_pop(_luaL, 1);
	}
}

NCLLuaPlayer::~NCLLuaPlayer()
{
}

void NCLLuaPlayer::Play()
{
	printf("NCLPlayer id=image src=%s play\n", _media->src.c_str());

	printf(":: %p\n", _luaL);
	if (luaL_loadfile(_luaL, _filename.c_str()) == 0) {
		if (lua_pcall(_luaL, 0, 0, 0) != 0) {
			printf("NCLPlayer id=image src=%s play error\n", _media->src.c_str());
		}
	}

	_is_alive = true;
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

void NCLLuaPlayer::Stop()
{
	printf("NCLPlayer id=image src=%s stop\n", _media->src.c_str());

	_component->Hide();

	lua_gc(_luaL, LUA_GCCOLLECT, 0);
	lua_close(_luaL);

	/* TODO:: fix
	media::VideoPlayer *player = controller::Controller::GetInstance()->GetVideoPlayer();

	if (player != NULL) {
		player->GetComponent()->SetBounds(0, 0, 1920, 1080);
	}
	*/

	_is_alive = false;
}

void NCLLuaPlayer::Close()
{
	puts("NCLPlayer id=image abort\n");

	_is_alive = false;
}

}

