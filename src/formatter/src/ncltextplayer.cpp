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
#include "ncltextplayer.h"
#include "nclformatter.h"
#include "nclenviroment.h"
#include "nclhelper.h"
#include "jfile.h"
#include "jstringutils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

namespace jncl {

NCLTextPlayer::NCLTextPlayer(NCLEnviroment *env, struct nclmedia_t *media):
	NCLPlayer(media, NCL_PLAYER_TEXT)
{
	puts("NCLPlayer id=text create\n");

	_enviroment = env;

	struct ncldescriptor_t *descriptor = _media->descriptor;
	struct nclregion_t *region = descriptor->region;

	int border = descriptor->focusborderwidth;

	if (border < 0) {
		border = 0;
	}

	// Uma lista priorizada de nomes de família de fonts e/ou nomes genéricos de famílias
	_font_family = "";
	// Estilo da fonte (normal or italic)
	_font_style = "";
	// Forma de exibição do texto: fonte em small-caps ou normal
	_font_variant = "";
	// Peso da fonte (normal or bold)
	_font_weight = "";
	// Tamanho da fonte
	_font_size = 16;
	// Cor da fonte
	_font_color = 0xff000000;
	// Cor da fundo
	_background = 0x00000000;

	for (std::vector<struct nclparam_t>::iterator i=media->params.begin(); i!=media->params.end(); i++) {
		if (strcasecmp(i->name.c_str(), "fontfamily") == 0) {
			_font_family = i->value;
		} else if (strcasecmp(i->name.c_str(), "fontstyle") == 0) {
			_font_style = i->value;
		} else if (strcasecmp(i->name.c_str(), "fontsize") == 0) {
			_font_size = NCLHelper::ParseNumber(i->value, 0, 4, 96);
		} else if (strcasecmp(i->name.c_str(), "fontvariant") == 0) {
			_font_variant = i->value;
		} else if (strcasecmp(i->name.c_str(), "fontweight") == 0) {
			_font_weight = i->value;
		} else if (strcasecmp(i->name.c_str(), "fontcolor") == 0) {
			_font_color = NCLHelper::ParseColor(i->value);
		} else if (strcasecmp(i->name.c_str(), "background") == 0) {
			_background = NCLHelper::ParseColor(i->value);
		}
	}

	_font = jgui::Font::CreateFont("fonts/font.ttf", jgui::JFA_NORMAL, _font_size);

	// _filename = _enviroment->GetBaseDirectory() + "/" + _media->src;

	_component = new jgui::Window(region->left+border, region->top+border, region->width+border, region->height+border);

	_theme.SetBorder("window", jgui::JCB_EMPTY);

	_component->SetTheme(&_theme);
	_component->SetBackgroundVisible(false);
	_component->SetUndecorated(true);
}

NCLTextPlayer::~NCLTextPlayer()
{
}

void NCLTextPlayer::Hide()
{
	puts("NCLPlayer id=text hide\n");
}

void NCLTextPlayer::Play()
{
	printf("NCLPlayer id=text src=%s play\n", _media->src.c_str());

	_component->Show(false);

	Render();

	struct ncldescriptor_t *descriptor = _media->descriptor;

	if (descriptor != NULL && descriptor->explicitdur > 0) {
		NCLTimer::GetInstance()->RegisterTimerListener(this, "explicitDur", descriptor->explicitdur);
	}

	_is_alive = true;
}

void NCLTextPlayer::Stop()
{
	printf("NCLPlayer id=text src=%s stop\n", _media->src.c_str());

	_component->Hide();

	NCLTimer::GetInstance()->RemoveTimerListener(this);

	_is_alive = false;
}

void NCLTextPlayer::Pause()
{
	puts("NCLPlayer id=text pause\n");

	_is_alive = false;
}

void NCLTextPlayer::Resume()
{
	puts("NCLPlayer id=text resume\n");

	Play();
}

void NCLTextPlayer::Abort()
{
	puts("NCLPlayer id=text abort\n");

	_is_alive = false;
}

void NCLTextPlayer::Release()
{
	puts("NCLPlayer id=text release\n");

	_is_alive = false;
}

void NCLTextPlayer::Render()
{
	jgui::Graphics *g = _component->GetGraphics();

	if (g == NULL) {
		return;
	}

	int width = _component->GetWidth(),
			height = _component->GetHeight();

	g->Reset();
	g->Clear();
	
	jio::File *file = NULL;
	char tmp[1024];

	try {
		int r, k = 0;

		file = jio::File::OpenFile(_filename);

		while ((r = file->Read((tmp+k), 255)) > 0 && k < (1024-1)) {
			k = k + r;
		}

		tmp[k] = '\0';
	} catch (...) {
		tmp[0] = '\0';
	}

	if (file != NULL) {
		delete file;
	}

	std::string text = tmp;

	if (strcasecmp(_font_variant.c_str(), "small-caps") == 0) {
		text = jcommon::StringUtils::ToLower(text);
	} else if (strcasecmp(_font_variant.c_str(), "large-caps") == 0) {
		text = jcommon::StringUtils::ToUpper(text);
	}

	// if (g->DrawImage(_filename, 0, 0, width, height) == false) {
		g->SetColor(_background);
		g->FillRectangle(0, 0, width, height);
		g->SetFont(_font);
		g->SetColor(_font_color);
		g->DrawString(text, 10, 10);
	// }

	g->Flip();
}

}

