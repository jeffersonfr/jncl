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
#include "videoplayer.h"
#include "mediaexception.h"
#include "demuxmanager.h"
#include "filterraw.h"

#include "jnetwork/jdatagramsocket.h"
#include "jexception/jconnectionexception.h"
#include "jexception/jconnectiontimeoutexception.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define VIDEO_WIDTH		1920/2
#define VIDEO_HEIGHT	1080/2

#ifdef min
#undef min
#endif
#define min( a, b )  (((a)<(b)) ? (a) : (b))

#ifdef max
#undef max
#endif
#define max( a, b )  (((a)>(b)) ? (a) : (b))

#ifdef CLAMP
#undef CLAMP
#endif
#define CLAMP( a, min, max )  (((a)<(min)) ? (min) : (((a)>(max)) ? (max) : (a)))

namespace media {

VideoComponent::VideoComponent(int x, int y, int width, int height):
	jgui::Component({x, y, width, height})
{
	_clear = true;
	_invalid = false;
	_image = NULL;

	// SetSignal(true);
	// SetSignal(false);
}

VideoComponent::~VideoComponent()
{
}

void VideoComponent::Release() 
{
	_invalid = true;
	_clear = true;
}

void VideoComponent::SetSignal(bool b) 
{
	_clear = b;

	if (_invalid == true) {
		_clear = true;
	}

	Repaint();
}

void VideoComponent::SetOffScreenImage(jgui::Image *image)
{
	_image = image;
}

void VideoComponent::Paint(jgui::Graphics *g)
{
	// jgui::Component::Paint(g);
	
  /*
	if (_clear == false) {
		g->DrawImage(_image, 0, 0, GetWidth(), GetHeight());
	} else {
		g->SetColor(0x00, 0x00, 0x00, 0xff);
		g->FillRectangle(0, 0, GetWidth(), GetHeight());
	}
  */
}

VideoPlayer::VideoPlayer(tuner::Locator *mrl)
{
  /*
	_volume = 1;
	_mute = false;

	_component = new VideoComponent(0, 0, VIDEO_WIDTH, VIDEO_HEIGHT);
	_offscreen = jgui::Image::CreateImage(jgui::JPF_ARGB, VIDEO_WIDTH, VIDEO_HEIGHT);
	
	((VideoComponent *)_component)->SetOffScreenImage(_offscreen);

	_screen_width = jgui::GFXHandler::GetInstance()->GetScreenWidth();
	_screen_height = jgui::GFXHandler::GetInstance()->GetScreenHeight();
	_scale_width = 1920;
	_scale_height = 1080;
	
	if (mrl->GetProtocol() == "file") {
		_mrl = mrl->GetPath();
	} else { // if (mrl->GetProtocol() == "udp") {
		// _mrl = mrl->what();

		_mrl = "/tmp/video.ts";
		_alive = true;

		Start();

		// INFO:: tempo para bufferizar
		jthread::Thread::Sleep(1000);
	}
  */
}

VideoPlayer::~VideoPlayer()
{
}

void VideoPlayer::Run()
{
  /*
	demux::Demux *demux = demux::DemuxManager::getInstance()->getDemux(demux::RAW);
	demux::FilterRaw *filter = new demux::FilterRaw();

	int r,
			fd,
			index = 0,
			size = 7*188;
	char buf[size];

	demux->setFilter(filter);
	demux->start();
	
	unlink("/tmp/video.ts");
	mkfifo("/tmp/video.ts", 0755);

	fd = open("/tmp/video.ts", O_ASYNC | O_WRONLY | O_LARGEFILE);

	if (fd < 3) {
		return;
	}

	while (_alive == true) {
		r = demux->readStream(buf+index, size);

		if (r > 0) {
			index = index + r;
			
			if (index >= 7*188) {
				int w = write(fd, buf, index);
				
				if (w <= 0) {
					sleep(1);
				}

				index = 0;
			}
		}
	}
  */
}

jgui::Component * VideoPlayer::GetComponent()
{
	return _component;
}

void VideoPlayer::Play()
{
	DispatchEvent(new MediaPlayerEvent(this, media::STARTED_EVENT));
}

void VideoPlayer::Pause()
{
	DispatchEvent(new MediaPlayerEvent(this, media::PAUSED_EVENT));
}

void VideoPlayer::Stop()
{
	DispatchEvent(new MediaPlayerEvent(this, media::STOPPED_EVENT));
}

void VideoPlayer::Resume()
{
  DispatchEvent(new MediaPlayerEvent(this, media::RESUMED_EVENT));
}

void VideoPlayer::Dispose()
{
}

void VideoPlayer::SetLoop(bool b)
{
	_loop = b;
}

bool VideoPlayer::IsLoop()
{
	return _loop;
}

void VideoPlayer::SetMediaTime(int i)
{
}

int VideoPlayer::GetMediaTime()
{
	return 0;
}

void VideoPlayer::SetMute(bool b)
{
}

bool VideoPlayer::IsMute()
{
	return _mute;
}

void VideoPlayer::SetVolume(int i)
{
}

int VideoPlayer::GetVolume()
{
	return _volume;
}

void VideoPlayer::SetBrightness(int i)
{
}

void VideoPlayer::SetContrast(int i)
{
}

void VideoPlayer::SetHue(int i)
{
}

void VideoPlayer::SetSaturation(int i)
{
}

void VideoPlayer::SetNitidez(int i)
{
}

int VideoPlayer::GetBrightness()
{
	return 0;
}

int VideoPlayer::GetContrast()
{
	return 0;
}

int VideoPlayer::GetHue()
{
	return 0;
}

int VideoPlayer::GetSaturation()
{
	return 0;
}

int VideoPlayer::GetNitidez()
{
	return 0;
}

}

