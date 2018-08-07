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
#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include "mediaplayer.h"
#include "tuner.h"

#include "jgui/jcontainer.h"
#include "jgui/jimage.h"
#include "jgui/jcomponent.h"

#include <iostream>
#include <string>
#include <map>
#include <thread>

#include <stdio.h>
#include <stdlib.h>

namespace media {

class PlayerManager;

class VideoComponent : public jgui::Component {

		private:
			jgui::Image *_image;
			bool _clear,
				 _invalid;

		public:
			VideoComponent(int x, int y, int width, int height);
			virtual ~VideoComponent();

			void SetSignal(bool b);
			void SetOffScreenImage(jgui::Image *image);
			void Release();
			
			virtual void Paint(jgui::Graphics *g);

};

class VideoPlayer : public Player {

	friend class PlayerManager;

	private:
		std::vector<int> _handlers;
		std::map<int, int> _timed_handlers;
		struct frame_data *data;
		jgui::Component *_component;
    std::thread _thread;

		VideoPlayer(tuner::Locator *mrl);

	public:
		jgui::Image 
      *_offscreen;
		std::string 
      _mrl;
		int 
      _screen_width,
			_screen_height,
			_scale_width,
			_scale_height;
		int 
      cx,
			cy,
			cw,
			ch;
		int 
      _volume;
		bool
      paused,
			 _mute,
			 _alive,
			 _flag,
			 _loop;

	public:
		virtual ~VideoPlayer();

		virtual jgui::Component * GetComponent();

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
		virtual bool IsMute();
		virtual bool IsLoop();
		
		virtual void SetBrightness(int i);
		virtual void SetContrast(int i);
		virtual void SetHue(int i);
		virtual void SetSaturation(int i);
		virtual void SetNitidez(int i);
		
		virtual int GetBrightness();
		virtual int GetContrast();
		virtual int GetHue();
		virtual int GetSaturation();
		virtual int GetNitidez();

		virtual void Run();

};

}

#endif
