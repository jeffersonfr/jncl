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
#include "nclplayer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

namespace jncl {

NCLPlayer::NCLPlayer(struct nclmedia_t *media, nclplayer_type_t type)
{
	_media = media;
	_has_focus = false;
	_is_idle = true;
	_component = NULL;
	_is_alive = false;
	_transition_time = 0;
	_type = type;
	_transition = NCL_TRANSITION_NONE;
}

NCLPlayer::~NCLPlayer()
{
	if (_component != NULL) {
		delete _component;
	}
}

bool NCLPlayer::IsAlive()
{
	return _is_alive;
}

void NCLPlayer::SetFocus(bool b)
{
	if (_has_focus == b) {
		return;
	}

	_has_focus = b;

	if (_component != NULL) {
		_component->Repaint();
	}
}

bool NCLPlayer::HasFocus()
{
	return _has_focus;
}

bool NCLPlayer::IsIdle()
{
	return _is_idle;
}

void NCLPlayer::WaitIdle()
{
}

void NCLPlayer::SetTransition(ncltransition_type_t t)
{
	_transition = t;
}

void NCLPlayer::SetTransitionTime(int time)
{
	_transition_time = time;
}

void NCLPlayer::Hide()
{
}

void NCLPlayer::Play()
{
}

void NCLPlayer::Stop()
{
}

void NCLPlayer::Pause()
{
}

void NCLPlayer::Resume()
{
}

void NCLPlayer::Abort()
{
}

void NCLPlayer::Release()
{
}

struct nclmedia_t * NCLPlayer::GetMedia()
{
	return _media;
}

jgui::Window * NCLPlayer::GetComponent()
{
	return _component;
}

std::vector<struct ncllink_t *> * NCLPlayer::GetLinks()
{
	return &_links;
}

void NCLPlayer::TimeoutReceived(NCLTimerEvent *event)
{
	if (event->GetName().find("explicitDur") != std::string::npos) {
		Stop();
	}
}

}

