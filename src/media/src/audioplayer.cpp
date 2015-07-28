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
#include "audioplayer.h"
#include "directfb.h"

namespace media {

AudioPlayer::AudioPlayer(tuner::Locator *mrl)
{
	_mute = false;
	_mrl = mrl->what();
	_volume = 1.0;
	_loop = false;
	
	if (_mrl == "") {
		return;
	}

	_alive = true;
}

AudioPlayer::~AudioPlayer()
{
	_alive = false;
}

void AudioPlayer::Play()
{
	// DispatchEvent(new MediaPlayerEvent(this, media::STARTED_EVENT));
}

void AudioPlayer::Pause()
{
	// DispatchEvent(new MediaPlayerEvent(this, media::PAUSED_EVENT));
}

void AudioPlayer::Resume()
{
	// DispatchEvent(new MediaPlayerEvent(this, media::RESUMED_EVENT));
}

void AudioPlayer::Dispose()
{
}

void AudioPlayer::Forward()
{
}

void AudioPlayer::Rewind()
{
}

void AudioPlayer::SetLoop(bool b)
{
}

void AudioPlayer::SetMute(bool b)
{
}

bool AudioPlayer::IsLoop()
{
	return _loop;
}

bool AudioPlayer::IsMute()
{
	return _mute;
}

void AudioPlayer::Stop()
{
	// DispatchEvent(new MediaPlayerEvent(this, media::STOPPED_EVENT));
}

int AudioPlayer::GetLength()
{
	return 0;
}

void AudioPlayer::SetMediaTime(int i)
{
}

int AudioPlayer::GetMediaTime()
{
	return 0;
}

void AudioPlayer::SetVolume(int i)
{
}

int AudioPlayer::GetVolume()
{
	return (int)_volume;
}

void AudioPlayer::GetTrackDescription(struct audio_track_t *t)
{
}

}
