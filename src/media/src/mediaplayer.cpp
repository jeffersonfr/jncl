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
#include "mediaplayer.h"

namespace media {

Player::Player()
{
}

Player::~Player()
{
}

void Player::Play()
{
}

void Player::Pause()
{
}

void Player::Stop()
{
}

void Player::Resume()
{
}

void Player::Dispose()
{
}

void Player::SetMediaTime(int i)
{
}

int Player::GetMediaTime()
{
	return 0;
}

void Player::SetVolume(int i)
{
}

int Player::GetVolume()
{
	return 0;
}

void Player::SetMute(bool b)
{
}

void Player::SetLoop(bool b)
{
}

bool Player::IsMute(bool b)
{
	return false;
}

bool Player::IsLoop(bool b)
{
	return false;
}

void Player::RegisterListener(MediaPlayerListener *l)
{
	for (std::vector<MediaPlayerListener *>::iterator i=_listeners.begin(); i!=_listeners.end(); i++) {
		if (l == (*i)) {
			return;
		}
	}

	_listeners.push_back(l);
}

void Player::RemoveListener(MediaPlayerListener *l)
{
	for (std::vector<MediaPlayerListener *>::iterator i=_listeners.begin(); i!=_listeners.end(); i++) {
		if (l == (*i)) {
			_listeners.erase(i);

			break;
		}
	}
}

void Player::DispatchEvent(MediaPlayerEvent *event)
{
	for (std::vector<MediaPlayerListener *>::iterator i=_listeners.begin(); i!=_listeners.end(); i++) {
		if (event->GetType() == PAUSED_EVENT) {
			(*i)->MediaPaused(event);
		} else if (event->GetType() == STARTED_EVENT) {
			(*i)->MediaStarted(event);
		} else if (event->GetType() == RESUMED_EVENT) {
			(*i)->MediaResumed(event);
		} else if (event->GetType() == STOPPED_EVENT) {
			(*i)->MediaStopped(event);
		} else if (event->GetType() == FINISHED_EVENT) {
			(*i)->MediaFinished(event);
		}
	}

	delete event;
}

}

