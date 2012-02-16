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
	sound = NULL;
	provider = NULL;
	stream = NULL;
	playback = NULL;

	_mute = false;
	_mrl = mrl->what();
	_volume = 1.0;
	_loop = false;
	
	if (_mrl == "") {
		return;
	}

	if (FusionSoundInit(0, NULL) != 0) {
		return;
	}

	if (FusionSoundCreate(&sound) != 0) {
		sound = NULL;
		return;
	}

	// if (sound->CreateMusicProvider(sound, _mrl.c_str(), &provider) != 0) {
	if (sound->CreateMusicProvider(sound, _mrl.c_str(), &provider) != 0) {
		sound->Release( sound );
		sound = NULL;
		return;
	}

	FSStreamDescription s_dsc;
	double len = 0;

	// Select current track in playlist. 
	if (provider->SelectTrack(provider, 0) != 0) {
		return;// DFENUM_OK;
	}

	provider->GetStreamDescription(provider, &s_dsc);
	if (stream) {
		FSStreamDescription dsc;
		// Check whether stream format changed. 
		stream->GetDescription(stream, &dsc);
		if (dsc.samplerate   != s_dsc.samplerate || dsc.channels != s_dsc.channels || dsc.sampleformat != s_dsc.sampleformat) {
			if (playback) {
				playback->Release(playback);
				playback = NULL;
			}
			stream->Release(stream);
			stream = NULL;
		}
	}

	if (!stream) {
		// Create the sound stream and feed it. 
		if (sound->CreateStream(sound, &s_dsc, &stream) != 0) {
			return;// DFENUM_CANCEL;
		}

		stream->GetDescription(stream, &s_dsc);
		stream->GetPlayback(stream, &playback);
	}

	// Reset volume level. 
	playback->SetVolume(playback, _volume);
	// Query provider for track length. 
	provider->GetLength(provider, &len);
	
	_alive = true;
}

AudioPlayer::~AudioPlayer()
{
	_alive = false;

	if (provider) {
		provider->Release(provider);
	}

	if (playback) {
		playback->Release(playback);
	}

	if (stream) {
		stream->Release(stream);
	}

	if (sound) {
		sound->Release(sound);
	}
}

void AudioPlayer::Play()
{
	FSMusicProviderStatus status = FMSTATE_UNKNOWN;

	// Let the provider play the music using our stream. 
	if (provider != NULL) {
		provider->GetStatus(provider, &status);

		if (status != FMSTATE_PLAY) {
			provider->PlayToStream(provider, stream);
		
			DispatchEvent(new MediaPlayerEvent(this, media::STARTED_EVENT));
		}
	}
}

void AudioPlayer::Pause()
{
	if (provider != NULL) {
		provider->Stop(provider);
		
		DispatchEvent(new MediaPlayerEvent(this, media::PAUSED_EVENT));
	}
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
	double len = 0;

	if (provider != NULL) {
		provider->GetLength(provider, &len);

		if (provider != NULL) {
			double pos;

			provider->GetPos(provider, &pos);
			provider->SeekTo(provider, pos+15.0);
		}
	}
}

void AudioPlayer::Rewind()
{
	if (provider != NULL) {
		double pos;

		provider->GetPos(provider, &pos);
		provider->SeekTo(provider, pos-15.0);
	}
}

void AudioPlayer::SetLoop(bool b)
{
	if (provider != NULL) {
		FSMusicProviderPlaybackFlags flags = (FSMusicProviderPlaybackFlags)(FMPLAY_NOFX);

		_loop = b;

		if (_loop == true) {
			flags = (FSMusicProviderPlaybackFlags)(flags | FMPLAY_LOOPING);
		} else {
			flags = (FSMusicProviderPlaybackFlags)(flags | 0);
		}

		provider->SetPlaybackFlags(provider, flags);
	}
}

void AudioPlayer::SetMute(bool b)
{
	if (provider != NULL) {
		_mute = b;

		if (_mute == false) {
			playback->SetVolume(playback, _mute);
		} else {
			playback->SetVolume(playback, 0);
		}
	}
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
	if (provider != NULL) {
		provider->Stop(provider);
		provider->SeekTo(provider, 0.0);
		
		DispatchEvent(new MediaPlayerEvent(this, media::STOPPED_EVENT));
	}
}

int AudioPlayer::GetLength()
{
	if (provider != NULL) {
		double len = 0;

		if (provider != NULL) {
			provider->GetLength(provider, &len);
		}

		return (int)len;
	}

	return 0;
}

void AudioPlayer::SetMediaTime(int i)
{
		double len = 0,
			   pos = i;

		if (provider != NULL) {
			provider->GetLength(provider, &len);
		}

		if (pos < 0.0) {
			pos = 0.0;
		} else if (pos > len) {
			pos = len;
		}

		if (provider != NULL) {
			provider->SeekTo(provider, pos);
		}
}

int AudioPlayer::GetMediaTime()
{
		double pos;

		if (provider != NULL) {
			provider->GetPos(provider, &pos);
		} else {
			pos = 0;
		}

		return (int)pos;
}

void AudioPlayer::SetVolume(int i)
{
		double volume = i;

		if (volume < 0.0) {
			volume = 0.0;
		} else if (volume > 2.0) {
			volume = 2.0;
		}

		_volume = volume;

		if (provider != NULL) {
			playback->SetVolume(playback, volume);
		}
}

int AudioPlayer::GetVolume()
{
	return (int)_volume;
}

void AudioPlayer::GetTrackDescription(struct audio_track_t *t)
{
	if (t == NULL) {
		return;
	}

	FSTrackDescription desc;
	 
  	provider->GetTrackDescription(provider, &desc);
	   
	t->artist = desc.artist;
	t->title = desc.title;
	t->album = desc.album;
	t->genre = desc.genre;
	t->year = desc.year;
}

}
