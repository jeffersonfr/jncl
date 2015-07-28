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

const char NCLLuaCanvasBinding::className[] = "canvas";

const luaL_Reg NCLLuaCanvasBinding::methods[] = {
	{"create", NCLLuaCanvasBinding::create},
	{"drawing", NCLLuaCanvasBinding::drawing},
	{"clip", NCLLuaCanvasBinding::clip},
	{"color", NCLLuaCanvasBinding::color},
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
	{"clearRect", NCLLuaCanvasBinding::clear},
	{"flush", NCLLuaCanvasBinding::flush},
	{"compose", NCLLuaCanvasBinding::compose},
	{"drawText", NCLLuaCanvasBinding::drawstring},
	{"size", NCLLuaCanvasBinding::size},
	{0, 0}
};

NCLLuaCanvasBinding::NCLLuaCanvasBinding(int xp, int yp, int wp, int hp)
{
	_image = NULL;
	_font = NULL;
	
	_window = new jgui::Window(xp, yp, wp, hp);

	_size = _window->GetSize();

	_window->SetBorder(jgui::JCB_EMPTY);
	_window->SetBackgroundVisible(false);
	_window->Show(false);

	_graphics = _window->GetGraphics();
}

NCLLuaCanvasBinding::NCLLuaCanvasBinding(std::string image, int wp, int hp)
{
	image = NCLLuaSettings::GetInstance()->GetProperty("lua.basedirectory") + image;

	_image = NULL;
	_font = NULL;
	_window = NULL;
	_graphics = NULL;

	jgui::jsize_t isize = jgui::Image::GetImageSize(image);

	if (wp <= 0) {
		wp = isize.width;
	}

	if (hp <= 0) {
		hp = isize.height;
	}

	_size.width = wp;
	_size.height = hp;

	_image = jgui::Image::CreateImage(image);
}

NCLLuaCanvasBinding::NCLLuaCanvasBinding(int wp, int hp)
{
	_image = NULL;
	_font = NULL;
	_window = NULL;
	_graphics = NULL;

	if (wp < 0 || hp < 0) {
		return;
	}

	_size.width = wp;
	_size.height = hp;

	_image = jgui::Image::CreateImage(jgui::JPF_ARGB, wp, hp);
	_graphics = _image->GetGraphics();
}

NCLLuaCanvasBinding::~NCLLuaCanvasBinding()
{
	if (_image != NULL) {
		delete _image;
	}

	if (_window != NULL) {
		delete _window;
	}
}

void NCLLuaCanvasBinding::Register(lua_State* L, int x, int y, int w, int h) 
{
	luaL_requiref(L, className, _lua_create, 1);
	
	lua_boxpointer(L, new NCLLuaCanvasBinding(x, y, w, h));
	lua_setglobal(L, className);
	
	lua_pop(L, 1);
}

int NCLLuaCanvasBinding::_lua_create(lua_State *L)
{
	luaL_newlib(L, methods);

	return 1;
}

int NCLLuaCanvasBinding::_lua_gc(lua_State *L)
{
	// NCLLuaCanvasBinding *a = (NCLLuaCanvasBinding *)lua_unboxpointer(L, 1);

	return 0;
}

int NCLLuaCanvasBinding::create(lua_State *L)
{
	NCLLuaCanvasBinding *c = NULL;

	if (lua_gettop(L) == 1) {
			const char *image = (const char *)luaL_checkstring(L, 1);
		
		c = new NCLLuaCanvasBinding(image);
	} else if (lua_gettop(L) == 2) {
		int w = (int)luaL_checknumber(L, 1),
				h = (int)luaL_checknumber(L, 2);
		
		c = new NCLLuaCanvasBinding(w, h);
	} else if (lua_gettop(L) == 3) {
		const char *img = (const char *)luaL_checkstring(L, 1);
		int w = (int)luaL_checknumber(L, 2),
				h = (int)luaL_checknumber(L, 3);
		
		c = new NCLLuaCanvasBinding(img, w, h);
	}

	if (c == NULL) {
		return 0;
	}

	lua_boxpointer(L, c);
	luaL_getmetatable(L, className);
	lua_setmetatable(L, -2);

	return 1;
}

int NCLLuaCanvasBinding::size(lua_State *L)
{
	//jthread::AutoLock lock(&NCLLuaEventBinding::event_mutex);

	lua_getglobal(L, className);
	
	//lua_pushnumber(L, a->_size.width);
	//lua_pushnumber(L, a->_size.height);

	return 2;
}

int NCLLuaCanvasBinding::font(lua_State *L)
{
	//jthread::AutoLock lock(&NCLLuaEventBinding::event_mutex);

	NCLLuaCanvasBinding *a = *((NCLLuaCanvasBinding **)NCLLuaObjectBinding::GetPointer(L, 1, className));
	
	// return jgui::Font::GetDefaultFont();

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
		jgui::Font *font = a->_font;

		lua_pushstring(L, font->GetName().c_str());
		lua_pushnumber(L, font->GetSize());

		return 2;
	}

	return 0;
}

int NCLLuaCanvasBinding::clip(lua_State *L)
{
	//jthread::AutoLock lock(&NCLLuaEventBinding::event_mutex);

	NCLLuaCanvasBinding *a = *((NCLLuaCanvasBinding **)NCLLuaObjectBinding::GetPointer(L, 1, className));

	if (a->_graphics == NULL) {
		return 0;
	}

	if (lua_type(L, 2) == LUA_TNUMBER) {
		int x = (int)luaL_checknumber(L, 2),
			y = (int)luaL_checknumber(L, 3),
			w = (int)luaL_checknumber(L, 4),
			h = (int)luaL_checknumber(L, 5);

		a->_graphics->SetClip(x, y, w, h);
	} else {
		jgui::jregion_t t = a->_graphics->GetClip();

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

	if (a->_graphics == NULL) {
		return 0;
	}

	if (lua_type(L, 2) == LUA_TNUMBER) {
		int r1 = (int)luaL_checknumber(L, 2),
			g1 = (int)luaL_checknumber(L, 3),
			b1 = (int)luaL_checknumber(L, 4),
			a1 = (int)luaL_checknumber(L, 5);

			a->_graphics->SetColor(r1, g1, b1, a1);
	} else if (lua_type(L, 2) == LUA_TSTRING) {
		const char *name = (char*)luaL_checkstring(L, 2);
		uint32_t color = jncl::NCLHelper::ParseColor(name);
		int r1 = (color>>16)&0xff,
			g1 = (color>>8)&0xff,
			b1 = (color>>0)&0xff,
			a1 = (color>>24)&0xff;

		a->_graphics->SetColor(r1, g1, b1, a1);
	} else {
		jgui::Color color = a->_graphics->GetColor();

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

	if (a->_graphics == NULL) {
		return 0;
	}

	const char *type = (const char *)luaL_checkstring(L, 2);

	if (strcmp(type, "blend") == 0) {
		a->_graphics->SetCompositeFlags(jgui::JCF_SRC_OVER);
	} else if (strcmp(type, "none") == 0) {
		a->_graphics->SetCompositeFlags(jgui::JCF_SRC);
	}

	return 0;
}

int NCLLuaCanvasBinding::measuretext(lua_State *L)
{
	//jthread::AutoLock lock(&NCLLuaEventBinding::event_mutex);

	NCLLuaCanvasBinding *a = *((NCLLuaCanvasBinding **)NCLLuaObjectBinding::GetPointer(L, 1, className));

	if (a->_graphics == NULL) {
		return 0;
	}

	IDirectFBFont *font = (IDirectFBFont *)a->_font->GetNativeFont();
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

	if (a->_graphics == NULL) {
		return 0;
	}

	if (lua_gettop(L) > 2) {
		jgui::Color color = a->_graphics->GetColor();

		int x = (int)luaL_checknumber(L, 2),
				y = (int)luaL_checknumber(L, 3);
		int r1 = (int)luaL_checknumber(L, 4),
				g1 = (int)luaL_checknumber(L, 5),
				b1 = (int)luaL_checknumber(L, 6),
				a1 = (int)luaL_checknumber(L, 7);

		a->_graphics->SetColor(r1, g1, b1, a1);
		a->_graphics->DrawLine(x, y, x, y);
		a->_graphics->SetColor(color);
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

		surface = (IDirectFBSurface *)a->_graphics->GetNativeSurface();

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

	if (a->_graphics == NULL) {
		return 0;
	}

	int x1 = (int)luaL_checknumber(L, 2),
		y1 = (int)luaL_checknumber(L, 3),
		x2 = (int)luaL_checknumber(L, 4),
		y2 = (int)luaL_checknumber(L, 5);

	a->_graphics->DrawLine(x1, y1, x2, y2);

	return 0;
}

int NCLLuaCanvasBinding::arc(lua_State *L)
{
	//jthread::AutoLock lock(&NCLLuaEventBinding::event_mutex);

	NCLLuaCanvasBinding *a = *((NCLLuaCanvasBinding **)NCLLuaObjectBinding::GetPointer(L, 1, className));

	if (a->_graphics == NULL) {
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
		a->_graphics->FillArc(x, y, rx, ry, start, end);
	} else {
		a->_graphics->DrawArc(x, y, rx, ry, start, end);
	}

	return 0;
}

int NCLLuaCanvasBinding::circle(lua_State *L)
{
	//jthread::AutoLock lock(&NCLLuaEventBinding::event_mutex);

	NCLLuaCanvasBinding *a = *((NCLLuaCanvasBinding **)NCLLuaObjectBinding::GetPointer(L, 1, className));

	if (a->_graphics == NULL) {
		return 0;
	}

	const char *type = (const char *)luaL_checkstring(L, 2);
	int x = (int)luaL_checknumber(L, 3),
		y = (int)luaL_checknumber(L, 4),
		r = (int)luaL_checknumber(L, 5);

	if (strcasecmp(type, "fill") == 0) {
		a->_graphics->FillCircle(x, y, r);
	} else {
		a->_graphics->DrawCircle(x, y, r);
	}

	return 0;
}

int NCLLuaCanvasBinding::ellipse(lua_State *L)
{
	//jthread::AutoLock lock(&NCLLuaEventBinding::event_mutex);

	NCLLuaCanvasBinding *a = *((NCLLuaCanvasBinding **)NCLLuaObjectBinding::GetPointer(L, 1, className));

	if (a->_graphics == NULL) {
		return 0;
	}

	const char *type = (const char *)luaL_checkstring(L, 2);
	int x = (int)luaL_checknumber(L, 3),
		y = (int)luaL_checknumber(L, 4),
		rx = (int)luaL_checknumber(L, 5),
		ry = (int)luaL_checknumber(L, 6);

	if (strcasecmp(type, "fill") == 0) {
		a->_graphics->FillArc(x, y, rx, ry, 0, 360);
	} else {
		a->_graphics->DrawArc(x, y, rx, ry, 0, 360);
	}

	return 0;
}

int NCLLuaCanvasBinding::drawRect(lua_State *L)
{
	//jthread::AutoLock lock(&NCLLuaEventBinding::event_mutex);

	NCLLuaCanvasBinding *a = *((NCLLuaCanvasBinding **)NCLLuaObjectBinding::GetPointer(L, 1, className));

	if (a->_graphics == NULL) {
		return 0;
	}

	const char *type = (const char *)luaL_checkstring(L, 2);
	int x = (int)luaL_checknumber(L, 3),
		y = (int)luaL_checknumber(L, 4),
		w = (int)luaL_checknumber(L, 5),
		h = (int)luaL_checknumber(L, 6);

	if (strcasecmp(type, "fill") == 0) {
		a->_graphics->FillRectangle(x, y, w, h);
	} else {
		a->_graphics->DrawRectangle(x, y, w, h);
	}

	return 0;
}

int NCLLuaCanvasBinding::triangle(lua_State *L)
{
	//jthread::AutoLock lock(&NCLLuaEventBinding::event_mutex);

	NCLLuaCanvasBinding *a = *((NCLLuaCanvasBinding **)NCLLuaObjectBinding::GetPointer(L, 1, className));

	if (a->_graphics == NULL) {
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
		a->_graphics->FillTriangle(p1x, p1y, p2x, p2y, p3x, p3y);
	} else {
		a->_graphics->DrawTriangle(p1x, p1y, p2x, p2y, p3x, p3y);
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
			a->_graphics->FillPolygon(x, y, p, length);
		} else {
			a->_graphics->DrawPolygon(x, y, p, length, true);
		}
	}
	

	return 1;
}

int NCLLuaCanvasBinding::drawimage(lua_State *L)
{
	//jthread::AutoLock lock(&NCLLuaEventBinding::event_mutex);

	NCLLuaCanvasBinding *a = *((NCLLuaCanvasBinding **)NCLLuaObjectBinding::GetPointer(L, 1, className));

	if (a->_graphics == NULL) {
		return 0;
	}

	if (lua_gettop(L) == 4) { 
		const char *img = (const char *)luaL_checkstring(L, 2);
		int x = (int)luaL_checknumber(L, 3),
			y = (int)luaL_checknumber(L, 4);

		if (img != NULL) {
			jgui::Image *image = jgui::Image::CreateImage(img);

			a->_graphics->DrawImage(image, x, y);

			delete image;
		}
	} else if (lua_gettop(L) == 6) {
		const char *img = (const char *)luaL_checkstring(L, 2);
		int x = (int)luaL_checknumber(L, 3),
			y = (int)luaL_checknumber(L, 4),
			w = (int)luaL_checknumber(L, 5),
			h = (int)luaL_checknumber(L, 6);

		if (img != NULL) {
			jgui::Image *image = jgui::Image::CreateImage(img);

			a->_graphics->DrawImage(image, x, y, w, h);

			delete image;
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
			jgui::Image *image = jgui::Image::CreateImage(img);

			a->_graphics->DrawImage(image, sx, sy, sw, sh, x, y, w, h);

			delete image;
		}
	}

	return 0;
}

int NCLLuaCanvasBinding::clear(lua_State *L)
{
	//jthread::AutoLock lock(&NCLLuaEventBinding::event_mutex);

	NCLLuaCanvasBinding *a = *((NCLLuaCanvasBinding **)NCLLuaObjectBinding::GetPointer(L, 1, className));

	if (lua_gettop(L) == 1) {
		if (a->_graphics != NULL) {
			a->_graphics->Clear();
		}
	} else if (lua_gettop(L) == 5) {
		int x = (int)luaL_checknumber(L, 2),
				y = (int)luaL_checknumber(L, 3),
				w = (int)luaL_checknumber(L, 4),
				h = (int)luaL_checknumber(L, 5);

		if (a->_graphics != NULL) {
			a->_graphics->Clear(x, y, w, h);
		}
	}

	return 0;
}

int NCLLuaCanvasBinding::flush(lua_State *L)
{
	//jthread::AutoLock lock(&NCLLuaEventBinding::event_mutex);

	NCLLuaCanvasBinding *a = *((NCLLuaCanvasBinding **)NCLLuaObjectBinding::GetPointer(L, 1, className));

	if (a->_graphics != NULL) {
		a->_graphics->Flip();
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
		g1 = c1->_graphics;
	}

	if (c2 != NULL) {
		g2 = c2->_graphics;
	}

	if (g1 == NULL || g2 == NULL) {
		return 0;
	}

	jgui::Image *img = c2->_image;

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

	if (a->_graphics != NULL) {
		if (lua_gettop(L) == 4) {
			a->_graphics->DrawString(std::string(text), x, y);
		} else if (lua_gettop(L) == 6) {
			int w = (int)luaL_checknumber(L, 5),
				h = (int)luaL_checknumber(L, 6);

			a->_graphics->DrawString(std::string(text), x, y, w, h);//, 0);
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

			a->_graphics->DrawString(std::string(text), x, y, w, h, align);
		}
	}

	return 0;
}

