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
#ifndef MIDIAPLAYER_H
#define MIDIAPLAYER_H

#include "mediaplayerlistener.h"

namespace media {

class Player{

	private:
		std::vector<MediaPlayerListener *> _listeners;

	protected:
		Player();

	public:
		virtual ~Player();

		virtual void Play();
		virtual void Pause();
		virtual void Stop();
		virtual void Resume();
		virtual void Dispose();

		virtual void SetMediaTime(int i);
		virtual int GetMediaTime();

		virtual void SetVolume(int i);
		virtual int GetVolume();

		virtual void SetMute(bool b);
		virtual void SetLoop(bool b);
		virtual bool IsMute(bool b);
		virtual bool IsLoop(bool b);

		void RegisterListener(MediaPlayerListener *l);
		void RemoveListener(MediaPlayerListener *l);
		void DispatchEvent(MediaPlayerEvent *event);
};

}

#endif
