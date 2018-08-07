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
#include "nclvideoplayer.h"
#include "nclenviroment.h"
#include "videoplayer.h"
#include "playermanager.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

namespace jncl {

NCLVideoPlayer::NCLVideoPlayer(NCLEnviroment *env, struct nclmedia_t *media):
	NCLPlayer(media, NCL_PLAYER_VIDEO)
{
	puts("NCLPlayer id=video create\n");

	_enviroment = env;

	struct ncldescriptor_t *descriptor = _media->descriptor;

	int border = descriptor->focusborderwidth;

	if (border < 0) {
		border = 0;
	}

	_component = NULL;

	/* TODO:: capturar main stream
	if (_media->src.find("isdtv-ts://") != std::string::npos || _media->src.find("sbtvd-ts://") != std::string::npos) {
		_is_video_layer = true;

		_player = controller::Controller::GetInstance()->GetVideoPlayer();
	} else */ {
		struct nclregion_t *region = _media->descriptor->region;
		int left = region->left,
				top = region->top,
				width = region->width,
				height = region->height;

		_is_video_layer = false;

    /* TODO::
		tuner::Locator *locator = new tuner::Locator("file://" + _enviroment->GetBaseDirectory() + "/" + _media->src);

		_component = new jgui::Window(left, top, width, height);
	
		_theme.SetBorder("window", jgui::JCB_EMPTY);

		_component->SetTheme(&_theme);
		_component->SetBackgroundVisible(false);
		_component->SetUndecorated(true);

		_player = media::PlayerManager::GetInstance()->CreatePlayer(locator);
		((media::VideoPlayer *)_player)->GetComponent()->SetBounds(0, 0, width, height);
		_component->Add(((media::VideoPlayer *)_player)->GetComponent());

		delete locator;
    */
	}
	
	for (std::vector<struct nclparam_t>::iterator i=media->params.begin(); i!=media->params.end(); i++) {
		if (strcasecmp(i->name.c_str(), "") == 0) {
			// TODO::
		}
	}

	for (std::vector<struct nclarea_t>::iterator i=media->areas.begin(); i!=media->areas.end(); i++) {
		// TODO::
	}
}

NCLVideoPlayer::~NCLVideoPlayer()
{
}

void NCLVideoPlayer::Hide()
{
	puts("NCLPlayer id=video hide\n");
}

void NCLVideoPlayer::Play()
{
	printf("NCLPlayer id=video src=%s play\n", _media->src.c_str());

	if (_player == NULL) {
		return;
	}

	if (_is_video_layer == true) {
		struct nclregion_t *region = _media->descriptor->region;

		((media::VideoPlayer *)_player)->GetComponent()->SetBounds(region->left, region->top, region->width, region->height);
	} else {
		_player->Play();
	}
		
	if (_component != NULL) {
		// TODO:: _component->Show(false);
	}
	
	struct ncldescriptor_t *descriptor = _media->descriptor;

	if (descriptor != NULL && descriptor->explicitdur > 0) {
		NCLTimer::GetInstance()->RegisterTimerListener(this, "explicitDur", descriptor->explicitdur);
	}
}

void NCLVideoPlayer::Stop()
{
	puts("NCLPlayer id=video stop\n");
	
	if (_component != NULL) {
		_player->Stop();
		// TODO:: _component->Hide();
	}
	
	NCLTimer::GetInstance()->RemoveTimerListener(this);
}

void NCLVideoPlayer::Pause()
{
	puts("NCLPlayer id=video pause\n");
}

void NCLVideoPlayer::Resume()
{
	puts("NCLPlayer id=video resume\n");
}

void NCLVideoPlayer::Abort()
{
	puts("NCLPlayer id=video abort\n");
}

void NCLVideoPlayer::Release()
{
	puts("NCLPlayer id=video release\n");
}

}

