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
#include "nclluacanvas.h"
#include "nclluaevent.h"
#include "nclhelper.h"
#include "nclluasettings.h"
#include "jgfxhandler.h"

#include <directfb.h>

NCLLuaCanvasBinding::NCLLuaCanvasBinding(int xp, int yp, int wp, int hp)
{
	_img = NULL;
	_font = NULL;
	
	_size.width = wp;
	_size.height = hp;
	
	_window = new jgui::Window(xp, yp, wp, hp);

	_window->SetBorder(jgui::JCB_EMPTY);
	_window->SetBackgroundVisible(false);
	_window->Show(false);

	_graphics = _window->GetGraphics();
}

NCLLuaCanvasBinding::NCLLuaCanvasBinding(std::string image, int wp, int hp)
{
	image = NCLLuaSettings::GetInstance()->GetProperty("lua.basedirectory") + image;

	_img = NULL;
	_graphics = NULL;
	_font = NULL;

	_size.width = 0;
	_size.height = 0;

	IDirectFB *engine = (IDirectFB *)jgui::GFXHandler::GetInstance()->GetGraphicEngine();
	IDirectFBImageProvider *provider = NULL;
	DFBSurfaceDescription desc;

	if (engine->CreateImageProvider(engine, image.c_str(), &provider) != DFB_OK) {
		return;
	}

	if (provider->GetSurfaceDescription (provider, &desc) != DFB_OK) {
		return;
	}

	_size.width = wp;

	if (wp <= 0) {
		wp = SCREEN_TO_SCALE(desc.width, jgui::GFXHandler::GetInstance()->GetScreenWidth(), DEFAULT_SCALE_WIDTH);
	}

	_size.height = hp;

	if (hp <= 0) {
		hp = SCREEN_TO_SCALE(desc.height, jgui::GFXHandler::GetInstance()->GetScreenHeight(), DEFAULT_SCALE_HEIGHT);
	}

	_img = jgui::Image::CreateImage(wp, hp);

	_graphics = _img->GetGraphics();

	provider->Release(provider);

	_graphics->SetPorterDuffFlags(jgui::JPF_NONE);
	_graphics->SetBlittingFlags(jgui::JBF_ALPHACHANNEL);

	_graphics->DrawImage(image, 0, 0, wp, hp);
}

NCLLuaCanvasBinding::NCLLuaCanvasBinding(int wp, int hp)
{
	_img = NULL;
	_font = NULL;

	if (wp <= 0) {
		wp = 1;
	}

	if (hp <= 0) {
		hp = 1;
	}

	_img = jgui::Image::CreateImage(wp, hp);

	_graphics = _img->GetGraphics();
	
	_size.width = wp;
	_size.height = hp;
}

NCLLuaCanvasBinding::~NCLLuaCanvasBinding()
{
	if (_img != NULL) {
		delete _img;
	}

	if (_window != NULL) {
		delete _window;
	}
}

jgui::jsize_t NCLLuaCanvasBinding::GetSize()
{
	return _size;
}

jgui::Graphics * NCLLuaCanvasBinding::GetGraphics()
{
	return _graphics;
}

jgui::Image * NCLLuaCanvasBinding::GetOffScreenImage()
{
	return _img;
}

jgui::Font * NCLLuaCanvasBinding::GetFont()
{
	if (_font != NULL) {
		return _font;
	}

	return jgui::Font::GetDefaultFont();
}

void NCLLuaCanvasBinding::Register(lua_State* L, int x, int y, int w, int h) 
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
	
	NCLLuaCanvasBinding *c = new NCLLuaCanvasBinding(x, y, w, h);

	lua_boxpointer(L, c);
	luaL_getmetatable(L, className);
	lua_setmetatable(L, -2);
	lua_setglobal(L, className);
}

int NCLLuaCanvasBinding::_lua_create(lua_State *L)
{
	NCLLuaCanvasBinding *c = NULL;

	if (lua_type(L, 1) == LUA_TNUMBER) {
		int w = (int)luaL_checknumber(L, 1),
			h = (int)luaL_checknumber(L, 2);
		
		c = new NCLLuaCanvasBinding(w, h);
	} else if (lua_type(L, 1) == LUA_TSTRING) {
		if (lua_type(L, 2) == LUA_TNUMBER && lua_type(L, 3) == LUA_TNUMBER) {
			const char *img = (const char *)luaL_checkstring(L, 1);
			int w = (int)luaL_checknumber(L, 2),
				h = (int)luaL_checknumber(L, 3);
		
			c = new NCLLuaCanvasBinding(img, w, h);
		} else {
			const char *img = (const char *)luaL_checkstring(L, 1);
		
			c = new NCLLuaCanvasBinding(img);
		}
	} else {
		return 0;
	}

	lua_boxpointer(L, c);
	luaL_getmetatable(L, className);
	lua_setmetatable(L, -2);

	return 1;
}

int NCLLuaCanvasBinding::_lua_gc(lua_State *L)
{
	NCLLuaCanvasBinding *a = (NCLLuaCanvasBinding *)lua_unboxpointer(L, 1);

	// delete a;

	return 0;
}

int NCLLuaCanvasBinding::size(lua_State *L)
{
	//jthread::AutoLock lock(&NCLLuaEventBinding::event_mutex);

	NCLLuaCanvasBinding *a = *((NCLLuaCanvasBinding **)NCLLuaObjectBinding::GetPointer(L, 1, className));

	jgui::jsize_t size = a->GetSize();

	lua_pushnumber(L, size.width);
	lua_pushnumber(L, size.height);

	return 2;
}

int NCLLuaCanvasBinding::font(lua_State *L)
{
	//jthread::AutoLock lock(&NCLLuaEventBinding::event_mutex);

	NCLLuaCanvasBinding *a = *((NCLLuaCanvasBinding **)NCLLuaObjectBinding::GetPointer(L, 1, className));

	if (lua_type(L, 2) == LUA_TSTRING) {
		/* TODO::
		const char *name = (const char *)luaL_checkstring(L, 2);
		int size = (int)luaL_checknumber(L, 3);

		NCLLuaFontBinding *f = new NCLLuaFontBinding();

		f->SetFont(new jgui::Font(name, 0, size));

		lua_boxpointer(L, f);
		luaL_getmetatable(L, "NCLLuaFont");
		lua_setmetatable(L, -2);

		if (f != NULL) {
			a->SetFontBinding(f);
		}
		*/
	} else {
		jgui::Font *font = a->GetFont();

		lua_pushstring(L, font->GetName().c_str());
		lua_pushnumber(L, font->GetHeight());

		return 2;
	}

	return 0;
}

int NCLLuaCanvasBinding::clip(lua_State *L)
{
	//jthread::AutoLock lock(&NCLLuaEventBinding::event_mutex);

	NCLLuaCanvasBinding *a = *((NCLLuaCanvasBinding **)NCLLuaObjectBinding::GetPointer(L, 1, className));

	if (a->GetGraphics() == NULL) {
		return 0;
	}

	if (lua_type(L, 2) == LUA_TNUMBER) {
		int x = (int)luaL_checknumber(L, 2),
			y = (int)luaL_checknumber(L, 3),
			w = (int)luaL_checknumber(L, 4),
			h = (int)luaL_checknumber(L, 5);

		a->GetGraphics()->SetClip(x, y, w, h);
	} else {
		jgui::jregion_t t = a->GetGraphics()->GetClip();

		lua_pushnumber(L, t.x);
		lua_pushnumber(L, t.y);
		lua_pushnumber(L, t.width);
		lua_pushnumber(L, t.height);

		return 4;
	}

	return 0;
}

int NCLLuaCanvasBinding::color(lua_State *L)
{
	//jthread::AutoLock lock(&NCLLuaEventBinding::event_mutex);

	NCLLuaCanvasBinding *a = *((NCLLuaCanvasBinding **)NCLLuaObjectBinding::GetPointer(L, 1, className));

	if (a->GetGraphics() == NULL) {
		return 0;
	}

	if (lua_type(L, 2) == LUA_TNUMBER) {
		int r1 = (int)luaL_checknumber(L, 2),
			g1 = (int)luaL_checknumber(L, 3),
			b1 = (int)luaL_checknumber(L, 4),
			a1 = (int)luaL_checknumber(L, 5);

			a->GetGraphics()->SetColor(r1, g1, b1, a1);
	} else if (lua_type(L, 2) == LUA_TSTRING) {
		const char *name = (char*)luaL_checkstring(L, 2);
		uint32_t color = jncl::NCLHelper::ParseColor(name);
		int r1 = (color>>16)&0xff,
			g1 = (color>>8)&0xff,
			b1 = (color>>0)&0xff,
			a1 = (color>>24)&0xff;

		a->GetGraphics()->SetColor(r1, g1, b1, a1);
	} else {
		jgui::Color color = a->GetGraphics()->GetColor();

		lua_pushnumber(L, color.GetRed());
		lua_pushnumber(L, color.GetGreen());
		lua_pushnumber(L, color.GetBlue());
		lua_pushnumber(L, color.GetAlpha());

		return 4;
	}

	return 0;
}

int NCLLuaCanvasBinding::drawing(lua_State *L)
{
	//jthread::AutoLock lock(&NCLLuaEventBinding::event_mutex);

	NCLLuaCanvasBinding *a = *((NCLLuaCanvasBinding **)NCLLuaObjectBinding::GetPointer(L, 1, className));

	if (a->GetGraphics() == NULL) {
		return 0;
	}

	const char *type = (const char *)luaL_checkstring(L, 2);

	if (strcmp(type, "blend") == 0) {
		a->GetGraphics()->SetDrawingFlags(jgui::JDF_BLEND);
		a->GetGraphics()->SetBlittingFlags(jgui::JBF_ALPHACHANNEL);
	} else if (strcmp(type, "none") == 0) {
		a->GetGraphics()->SetDrawingFlags(jgui::JDF_NOFX);
		a->GetGraphics()->SetBlittingFlags(jgui::JBF_NOFX);
	}

	return 0;
}

int NCLLuaCanvasBinding::measuretext(lua_State *L)
{
	//jthread::AutoLock lock(&NCLLuaEventBinding::event_mutex);

	NCLLuaCanvasBinding *a = *((NCLLuaCanvasBinding **)NCLLuaObjectBinding::GetPointer(L, 1, className));

	if (a->GetGraphics() == NULL) {
		return 0;
	}

	IDirectFBFont *font = (IDirectFBFont *)a->GetFont()->GetNativeFont();
	DFBRectangle rect;

	font->GetStringExtents(font, (const char *)luaL_checkstring(L, 2), -1, NULL, &rect);

	int x = rect.x,
		y = rect.y,
		w = rect.w,
		h = rect.h;

	lua_pushnumber(L, x);
	lua_pushnumber(L, y);
	lua_pushnumber(L, w);
	lua_pushnumber(L, h);

	return 4;
}

int NCLLuaCanvasBinding::pixel(lua_State *L)
{
	//jthread::AutoLock lock(&NCLLuaEventBinding::event_mutex);

	NCLLuaCanvasBinding *a = *((NCLLuaCanvasBinding **)NCLLuaObjectBinding::GetPointer(L, 1, className));

	if (a->GetGraphics() == NULL) {
		return 0;
	}

	if (lua_gettop(L) > 2) {
		jgui::Color color = a->GetGraphics()->GetColor();

		int x = (int)luaL_checknumber(L, 2),
				y = (int)luaL_checknumber(L, 3);
		int r1 = (int)luaL_checknumber(L, 4),
				g1 = (int)luaL_checknumber(L, 5),
				b1 = (int)luaL_checknumber(L, 6),
				a1 = (int)luaL_checknumber(L, 7);

		a->GetGraphics()->SetColor(r1, g1, b1, a1);
		a->GetGraphics()->DrawLine(x, y, x, y);
		a->GetGraphics()->SetColor(color);
	} else {
		IDirectFBSurface *surface = NULL;
		DFBSurfacePixelFormat format;
		unsigned char *matrix;
		void *aux;
		int x,
			y,
			sx,
			sy,
			xp,
			yp,
			bytes,
			pitch;

		surface = (IDirectFBSurface *)a->GetGraphics()->GetNativeSurface();

		surface->GetSize(surface, &sx, &sy);
		surface->GetPixelFormat(surface, &format);

		bytes = DFB_BYTES_PER_PIXEL(format);

		xp = (int)luaL_checknumber(L, 2);
		yp = (int)luaL_checknumber(L, 3);

		x = (int)(((double)xp*(double)1280)/(double)jgui::GFXHandler::GetInstance()->GetScreenWidth());
		y = (int)(((double)yp*(double)800)/(double)jgui::GFXHandler::GetInstance()->GetScreenHeight());

		if (x > sx) {
			x = sx-1;
		}

		if (y > sy) {
			y = sy-1;
		}

		surface->Lock(surface, (DFBSurfaceLockFlags)(DSLF_READ), &aux, &pitch);

		matrix = (unsigned char *)aux;
		unsigned char *pos = matrix+y*pitch;
		unsigned int *pixel = (unsigned int *)(pos+x);

		surface->Unlock(surface);

		if (bytes == DSPF_RGB32 || bytes == DSPF_ARGB) {
			lua_pushnumber(L, (unsigned int)(((*pixel)>>0x18)&0xff));
			lua_pushnumber(L, (unsigned int)(((*pixel)>>0x10)&0xff));
			lua_pushnumber(L, (unsigned int)(((*pixel)>>0x08)&0xff));
			lua_pushnumber(L, (unsigned int)(((*pixel)>>0x00)&0xff));

			return 4;
		}
	}

	return 0;
}

int NCLLuaCanvasBinding::line(lua_State *L)
{
	//jthread::AutoLock lock(&NCLLuaEventBinding::event_mutex);

	NCLLuaCanvasBinding *a = *((NCLLuaCanvasBinding **)NCLLuaObjectBinding::GetPointer(L, 1, className));

	if (a->GetGraphics() == NULL) {
		return 0;
	}

	int x1 = (int)luaL_checknumber(L, 2),
		y1 = (int)luaL_checknumber(L, 3),
		x2 = (int)luaL_checknumber(L, 4),
		y2 = (int)luaL_checknumber(L, 5);

	a->GetGraphics()->DrawLine(x1, y1, x2, y2);

	return 0;
}

int NCLLuaCanvasBinding::arc(lua_State *L)
{
	//jthread::AutoLock lock(&NCLLuaEventBinding::event_mutex);

	NCLLuaCanvasBinding *a = *((NCLLuaCanvasBinding **)NCLLuaObjectBinding::GetPointer(L, 1, className));

	if (a->GetGraphics() == NULL) {
		return 0;
	}

	const char *type = (const char *)luaL_checkstring(L, 2);
	int x = (int)luaL_checknumber(L, 3),
		y = (int)luaL_checknumber(L, 4),
		rx = (int)luaL_checknumber(L, 5),
		ry = (int)luaL_checknumber(L, 6);
	double start = (double)luaL_checknumber(L, 7),
		   end = (double)luaL_checknumber(L, 8);

	if (strcasecmp(type, "fill") == 0) {
		a->GetGraphics()->FillArc(x, y, rx, ry, start, end);
	} else {
		a->GetGraphics()->DrawArc(x, y, rx, ry, start, end);
	}

	return 0;
}

int NCLLuaCanvasBinding::circle(lua_State *L)
{
	//jthread::AutoLock lock(&NCLLuaEventBinding::event_mutex);

	NCLLuaCanvasBinding *a = *((NCLLuaCanvasBinding **)NCLLuaObjectBinding::GetPointer(L, 1, className));

	if (a->GetGraphics() == NULL) {
		return 0;
	}

	const char *type = (const char *)luaL_checkstring(L, 2);
	int x = (int)luaL_checknumber(L, 3),
		y = (int)luaL_checknumber(L, 4),
		r = (int)luaL_checknumber(L, 5);

	if (strcasecmp(type, "fill") == 0) {
		a->GetGraphics()->FillCircle(x, y, r);
	} else {
		a->GetGraphics()->DrawCircle(x, y, r);
	}

	return 0;
}

int NCLLuaCanvasBinding::ellipse(lua_State *L)
{
	//jthread::AutoLock lock(&NCLLuaEventBinding::event_mutex);

	NCLLuaCanvasBinding *a = *((NCLLuaCanvasBinding **)NCLLuaObjectBinding::GetPointer(L, 1, className));

	if (a->GetGraphics() == NULL) {
		return 0;
	}

	const char *type = (const char *)luaL_checkstring(L, 2);
	int x = (int)luaL_checknumber(L, 3),
		y = (int)luaL_checknumber(L, 4),
		rx = (int)luaL_checknumber(L, 5),
		ry = (int)luaL_checknumber(L, 6);

	if (strcasecmp(type, "fill") == 0) {
		a->GetGraphics()->FillArc(x, y, rx, ry, 0, 360);
	} else {
		a->GetGraphics()->DrawArc(x, y, rx, ry, 0, 360);
	}

	return 0;
}

int NCLLuaCanvasBinding::drawRect(lua_State *L)
{
	//jthread::AutoLock lock(&NCLLuaEventBinding::event_mutex);

	NCLLuaCanvasBinding *a = *((NCLLuaCanvasBinding **)NCLLuaObjectBinding::GetPointer(L, 1, className));

	if (a->GetGraphics() == NULL) {
		return 0;
	}

	const char *type = (const char *)luaL_checkstring(L, 2);
	int x = (int)luaL_checknumber(L, 3),
		y = (int)luaL_checknumber(L, 4),
		w = (int)luaL_checknumber(L, 5),
		h = (int)luaL_checknumber(L, 6);

	if (strcasecmp(type, "fill") == 0) {
		a->GetGraphics()->FillRectangle(x, y, w, h);
	} else {
		a->GetGraphics()->DrawRectangle(x, y, w, h);
	}

	return 0;
}

int NCLLuaCanvasBinding::triangle(lua_State *L)
{
	//jthread::AutoLock lock(&NCLLuaEventBinding::event_mutex);

	NCLLuaCanvasBinding *a = *((NCLLuaCanvasBinding **)NCLLuaObjectBinding::GetPointer(L, 1, className));

	if (a->GetGraphics() == NULL) {
		return 0;
	}

	const char *type = (const char *)luaL_checkstring(L, 2);
	int p1x = (int)luaL_checknumber(L, 3),
		p1y = (int)luaL_checknumber(L, 4),
		p2x = (int)luaL_checknumber(L, 5),
		p2y = (int)luaL_checknumber(L, 6),
		p3x = (int)luaL_checknumber(L, 7),
		p3y = (int)luaL_checknumber(L, 8);


	if (strcasecmp(type, "fill") == 0) {
		a->GetGraphics()->FillTriangle(p1x, p1y, p2x, p2y, p3x, p3y);
	} else {
		a->GetGraphics()->DrawTriangle(p1x, p1y, p2x, p2y, p3x, p3y);
	}

	return 0;
}

int NCLLuaCanvasBinding::polygon(lua_State *L)
{
	//jthread::AutoLock lock(&NCLLuaEventBinding::event_mutex);

	NCLLuaCanvasBinding *a = *((NCLLuaCanvasBinding **)NCLLuaObjectBinding::GetPointer(L, 1, className));

	int length = lua_gettop(L);

	if (length > 2) {
		const char *type = (const char *)luaL_checkstring(L, 2);

		length = ((length-4)/2);

		jgui::jpoint_t p[length];
		int x = (int)luaL_checknumber(L, 3),
			y = (int)luaL_checknumber(L, 4);

		for (int i=0; i<length; i++) {
			p[i].x = (int)luaL_checknumber(L, 5+i*2+0);
			p[i].y = (int)luaL_checknumber(L, 5+i*2+1);
		}

		if (strcasecmp(type, "fill") == 0) {
			a->GetGraphics()->FillPolygon(x, y, p, length);
		} else {
			a->GetGraphics()->DrawPolygon(x, y, p, length, true);
		}
	}
	

	return 1;
}

int NCLLuaCanvasBinding::drawimage(lua_State *L)
{
	//jthread::AutoLock lock(&NCLLuaEventBinding::event_mutex);

	NCLLuaCanvasBinding *a = *((NCLLuaCanvasBinding **)NCLLuaObjectBinding::GetPointer(L, 1, className));

	if (a->GetGraphics() == NULL) {
		return 0;
	}

	a->GetGraphics()->SetPorterDuffFlags(jgui::JPF_NONE);
	a->GetGraphics()->SetBlittingFlags(jgui::JBF_ALPHACHANNEL);

	if (lua_gettop(L) == 4) { 
		const char *img = (const char *)luaL_checkstring(L, 2);
		int x = (int)luaL_checknumber(L, 3),
			y = (int)luaL_checknumber(L, 4);

		if (img != NULL) {
			a->GetGraphics()->DrawImage(img, x, y);
		}
	} else if (lua_gettop(L) == 6) {
		const char *img = (const char *)luaL_checkstring(L, 2);
		int x = (int)luaL_checknumber(L, 3),
			y = (int)luaL_checknumber(L, 4),
			w = (int)luaL_checknumber(L, 5),
			h = (int)luaL_checknumber(L, 6);

		if (img != NULL) {
			a->GetGraphics()->DrawImage(img, x, y, w, h);
		}
	} else if (lua_gettop(L) == 10) {
		const char *img = (const char *)luaL_checkstring(L, 2);
		int x = (int)luaL_checknumber(L, 3),
			y = (int)luaL_checknumber(L, 4),
			w = (int)luaL_checknumber(L, 5),
			h = (int)luaL_checknumber(L, 6);
		int sx = (int)luaL_checknumber(L, 7),
			sy = (int)luaL_checknumber(L, 8),
			sw = (int)luaL_checknumber(L, 9),
			sh = (int)luaL_checknumber(L, 10);

		if (img != NULL) {
			a->GetGraphics()->DrawImage(img, sx, sy, sw, sh, x, y, w, h);
		}
	}

	return 0;
}

int NCLLuaCanvasBinding::clear(lua_State *L)
{
	//jthread::AutoLock lock(&NCLLuaEventBinding::event_mutex);

	NCLLuaCanvasBinding *a = *((NCLLuaCanvasBinding **)NCLLuaObjectBinding::GetPointer(L, 1, className));

	if (lua_gettop(L) == 1) {
		if (a->GetGraphics() != NULL) {
			a->GetGraphics()->Clear();
		}
	} else if (lua_gettop(L) == 5) {
		int x = (int)luaL_checknumber(L, 2),
			y = (int)luaL_checknumber(L, 3),
			w = (int)luaL_checknumber(L, 4),
			h = (int)luaL_checknumber(L, 5);

		if (a->GetGraphics() != NULL) {
			jgui::Color color = a->GetGraphics()->GetColor();

			a->GetGraphics()->SetDrawingFlags(jgui::JDF_NOFX);
			a->GetGraphics()->SetPorterDuffFlags(jgui::JPF_NONE);
			a->GetGraphics()->SetBlittingFlags(jgui::JBF_ALPHACHANNEL);
			a->GetGraphics()->SetColor(0x00, 0x00, 0x00, 0x00);
			a->GetGraphics()->FillRectangle(x, y, w, h);
			a->GetGraphics()->SetColor(color);
		}
	}

	return 0;
}

int NCLLuaCanvasBinding::flush(lua_State *L)
{
	//jthread::AutoLock lock(&NCLLuaEventBinding::event_mutex);

	NCLLuaCanvasBinding *a = *((NCLLuaCanvasBinding **)NCLLuaObjectBinding::GetPointer(L, 1, className));

	if (a->GetGraphics() != NULL) {
		a->GetGraphics()->Flip();
	}

	return 0;
}

int NCLLuaCanvasBinding::compose(lua_State *L)
{
	//jthread::AutoLock lock(&NCLLuaEventBinding::event_mutex);

	NCLLuaCanvasBinding *c1 = *((NCLLuaCanvasBinding **)NCLLuaObjectBinding::GetPointer(L, 1, className));

	if (NCLLuaObjectBinding::GetPointer(L, 4, className) == NULL) {
		return luaL_argerror(L, 4, "invalid canvas parameter (nil)");
	}

	NCLLuaCanvasBinding *c2 = *((NCLLuaCanvasBinding **)NCLLuaObjectBinding::GetPointer(L, 4, className));
	jgui::Graphics *g1 = NULL,
		*g2 = NULL;
	int x = (int)luaL_checknumber(L, 2),
		y = (int)luaL_checknumber(L, 3);

	if (c1 != NULL) {
		g1 = c1->GetGraphics();
	}

	if (c2 != NULL) {
		g2 = c2->GetGraphics();
	}

	if (g1 == NULL || g2 == NULL) {
		return 0;
	}

	jgui::Image *img = c2->GetOffScreenImage();

	if (lua_type(L, 5) == LUA_TNUMBER && lua_type(L, 6) == LUA_TNUMBER && 
			lua_type(L, 7) == LUA_TNUMBER && lua_type(L, 8) == LUA_TNUMBER) {
		int sx = (int)luaL_checknumber(L, 5),
				sy = (int)luaL_checknumber(L, 6),
				sw = (int)luaL_checknumber(L, 7),
				sh = (int)luaL_checknumber(L, 8);

		g1->DrawImage(img, sx, sy, sw, sh, x, y, img->GetWidth(), img->GetHeight()); 
	} else if (lua_type(L, 5) == LUA_TNUMBER && lua_type(L, 6) == LUA_TNUMBER) {
		int sx = (int)luaL_checknumber(L, 5),
				sy = (int)luaL_checknumber(L, 6);

		g1->DrawImage(img, sx, sy, img->GetWidth(), img->GetHeight(), x, y, img->GetWidth(), img->GetHeight()); 
	} else if (lua_type(L, 5) == LUA_TNONE) {
		g1->DrawImage(img, x, y); 
	}

	return 0;
}

int NCLLuaCanvasBinding::drawstring(lua_State *L)
{
	//jthread::AutoLock lock(&NCLLuaEventBinding::event_mutex);

	NCLLuaCanvasBinding *a = *((NCLLuaCanvasBinding **)NCLLuaObjectBinding::GetPointer(L, 1, className));

	const char *text = (const char *)luaL_checkstring(L, 2);
	int x = (int)luaL_checknumber(L, 3),
		y = (int)luaL_checknumber(L, 4);

	if (a->GetGraphics() != NULL) {
		if (lua_gettop(L) == 4) {
			a->GetGraphics()->DrawString(std::string(text), x, y);
		} else if (lua_gettop(L) == 6) {
			int w = (int)luaL_checknumber(L, 5),
				h = (int)luaL_checknumber(L, 6);

			a->GetGraphics()->DrawString(std::string(text), x, y, w, h);//, 0);
		} else if (lua_gettop(L) == 7) {
			int w = (int)luaL_checknumber(L, 5),
				h = (int)luaL_checknumber(L, 6);
			jgui::jhorizontal_align_t align = jgui::JHA_LEFT;
			
			const char *salign = (const char *)luaL_checkstring(L, 7);

			if (strcasecmp(salign, "left") == 0) {
				align = jgui::JHA_LEFT;
			} else if (strcasecmp(salign, "center") == 0) {
				align = jgui::JHA_CENTER;
			} else if (strcasecmp(salign, "right") == 0) {
				align = jgui::JHA_RIGHT;
			} else if (strcasecmp(salign, "justified") == 0) {
				align = jgui::JHA_JUSTIFY;
			}

			a->GetGraphics()->DrawString(std::string(text), x, y, w, h, align);
		}
	}

	return 0;
}

const char NCLLuaCanvasBinding::className[] = "canvas";

#define method(class, name) {#name, class::name}

const luaL_Reg NCLLuaCanvasBinding::methods[] = {
	{"new", NCLLuaCanvasBinding::_lua_create},
	{"drawing", NCLLuaCanvasBinding::drawing},
	{"attrClip", NCLLuaCanvasBinding::clip},
	{"attrColor", NCLLuaCanvasBinding::color},
	{"pixel", NCLLuaCanvasBinding::pixel},
	{"attrFont", NCLLuaCanvasBinding::font},
	{"measureText", NCLLuaCanvasBinding::measuretext},
	{"drawLine", NCLLuaCanvasBinding::line},
	{"drawArc", NCLLuaCanvasBinding::arc},
	{"drawCircle", NCLLuaCanvasBinding::circle},
	{"drawEllipse", NCLLuaCanvasBinding::ellipse},
	{"drawRect", NCLLuaCanvasBinding::drawRect},
	{"drawTriangule", NCLLuaCanvasBinding::triangle},
	{"drawPolygon", NCLLuaCanvasBinding::polygon},
	{"drawImage", NCLLuaCanvasBinding::drawimage},
	{"drawClear", NCLLuaCanvasBinding::clear},
	{"flush", NCLLuaCanvasBinding::flush},
	{"compose", NCLLuaCanvasBinding::compose},
	{"drawText", NCLLuaCanvasBinding::drawstring},
	{"attrSize", NCLLuaCanvasBinding::size},
	{0, 0}
};

