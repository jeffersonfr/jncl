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
#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include "mediaplayer.h"
#include "jthread.h"
#include "tuner.h"

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include <fusionsound.h>
#include <stdio.h>
#include <stdlib.h>

namespace media {

struct audio_track_t {
	std::string artist,
		title,
		album,
		genre;
	int year;
};

class PlayerManager;

class AudioPlayer : public Player{

	friend class PlayerManager;

	private:
		std::vector<int> _handlers;
		std::map<int, int> _timed_handlers;

		AudioPlayer(tuner::Locator *mrl);

	public:
		IFusionSound *sound;
		IFusionSoundMusicProvider *provider;
		IFusionSoundStream *stream;
		IFusionSoundPlayback *playback;
		std::string _mrl;
		double _volume;
		bool _loop,
			 _mute,
			 _alive;

	public:
		virtual ~AudioPlayer();

		void Play();
		void Pause();
		void Stop();
		void Resume();
		void Dispose();
		void Forward();
		void Rewind();
		
		int GetLength();
		
		void SetMediaTime(int i);
		int GetMediaTime();
		
		void SetVolume(int i);
		int GetVolume();
		
		void SetLoop(bool b);
		void SetMute(bool b);
		
		bool IsLoop();
		bool IsMute();
		
  		void GetTrackDescription(struct audio_track_t *t);

};

}

#endif
