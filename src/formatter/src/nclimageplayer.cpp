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
#include "nclimageplayer.h"
#include "nclformatter.h"
#include "nclenviroment.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

namespace jncl {

NCLImagePlayer::NCLImagePlayer(NCLEnviroment *env, struct nclmedia_t *media):
	NCLPlayer(media, NCL_PLAYER_IMAGE)
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

	_image = jgui::Image::CreateImage(_filename);

	_component = new jgui::Window(region->left+border, region->top+border, region->width+border, region->height+border);

	_theme.SetBorder("window", jgui::JCB_EMPTY);

	_component->SetTheme(&_theme);
	_component->SetBackgroundVisible(false);
	_component->SetUndecorated(true);
	
	for (std::vector<struct nclparam_t>::iterator i=media->params.begin(); i!=media->params.end(); i++) {
		if (strcasecmp(i->name.c_str(), "") == 0) {
			// TODO::
		}
	}
}

NCLImagePlayer::~NCLImagePlayer()
{
	delete _image;
}

void NCLImagePlayer::Hide()
{
	puts("NCLPlayer id=image hide\n");
}

void NCLImagePlayer::Play()
{
	printf("NCLPlayer id=image src=%s play\n", _media->src.c_str());

	_component->Show(false);

	Render();

	struct ncldescriptor_t *descriptor = _media->descriptor;

	if (descriptor != NULL && descriptor->explicitdur > 0) {
		NCLTimer::GetInstance()->RegisterTimerListener(this, "explicitDur", descriptor->explicitdur);
	}

	_is_alive = true;
}

void NCLImagePlayer::Stop()
{
	printf("NCLPlayer id=image src=%s stop\n", _media->src.c_str());

	NCLTimer::GetInstance()->RemoveTimerListener(this);

	_component->Hide();

	_is_alive = false;
}

void NCLImagePlayer::Pause()
{
	puts("NCLPlayer id=image pause\n");

	_is_alive = false;
}

void NCLImagePlayer::Resume()
{
	puts("NCLPlayer id=image resume\n");

	Play();
}

void NCLImagePlayer::Abort()
{
	puts("NCLPlayer id=image abort\n");

	_is_alive = false;
}

void NCLImagePlayer::Release()
{
	puts("NCLPlayer id=image release\n");

	_is_alive = false;
}

void NCLImagePlayer::Render()
{
	jgui::Graphics *g = _component->GetGraphics();

	if (g == NULL) {
		return;
	}

	int width = _component->GetWidth(),
			height = _component->GetHeight();

	g->Reset();
	g->Clear();
	
	g->SetCompositeFlags(jgui::JCF_SRC);

	if (g->DrawImage(_image, 0, 0, width, height) == false) {
		g->SetColor(0x00, 0x00, 0x00, 0xff);
		g->FillRectangle(0, 0, width, height);
	}

	g->Flip();
}

}

