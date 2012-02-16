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
#ifndef NCLLUACANVAS_H
#define NCLLUACANVAS_H

#include "nclluaobjectbinding.h"
#include "jimage.h"
#include "jfont.h"
#include "jframe.h"

#include <iostream>
#include <string>

#include <stdio.h>
#include <stdlib.h>

class NCLLuaCanvasBinding {

	private:
		jgui::Window *_window;
		jgui::Image *_img;
		jgui::Graphics *_graphics;
		jgui::Font *_font;
		jgui::jsize_t _size;

	public:
		static const char className[];
		static const luaL_Reg methods[];

	public:
		NCLLuaCanvasBinding(int xp, int yp, int wp, int hp);
		NCLLuaCanvasBinding(std::string image, int width = -1, int height = -1);
		NCLLuaCanvasBinding(int width, int height);
		virtual ~NCLLuaCanvasBinding();

		void SetFont(jgui::Font *font);

		jgui::jsize_t GetSize();
		jgui::Font * GetFont();
		jgui::Graphics * GetGraphics();
		jgui::Image * GetOffScreenImage();

		static void Register(lua_State* L, int x, int y, int w, int h);
	
		static int _lua_create(lua_State *L);
		static int _lua_gc(lua_State *L);

		static int drawing(lua_State *L);
		static int size(lua_State *L);
		static int font(lua_State *L);
		static int clip(lua_State *L);
		static int color(lua_State *L);
		static int pixel(lua_State *L);
		static int measuretext(lua_State *L);
		static int arc(lua_State *L);
		static int circle(lua_State *L);
		static int ellipse(lua_State *L);
		static int line(lua_State *L);
		static int drawRect(lua_State *L);
		static int triangle(lua_State *L);
		static int polygon(lua_State *L);
		static int drawimage(lua_State *L);
		static int clear(lua_State *L);
		static int compose(lua_State *L);
		static int drawstring(lua_State *L);
		static int flush(lua_State *L);

};

#endif
