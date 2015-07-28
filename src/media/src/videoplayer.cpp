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
#include "jsocketexception.h"
#include "jsockettimeoutexception.h"
#include "jdatagramsocket.h"
#include "demuxmanager.h"
#include "filterraw.h"

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

DFBSurfacePixelFormat format = DSPF_YUY2; //UNKNOWN;

void media_event_listener(void *user_data, const xine_event_t *event);
int update_visual(IDirectFBSurface *destination, xine_video_port_t *vo_driver, dfb_visual_t *visual, int width, int height);
void output_cb(void *cdata, int width, int height, double ratio, DFBSurfacePixelFormat format, DFBRectangle *dst);
void frame_cb(void *cdata);

VideoComponent::VideoComponent(int x, int y, int width, int height):
	jgui::Component(x, y, width, height)
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
	
	if (_clear == false) {
		g->DrawImage(_image, 0, 0, GetWidth(), GetHeight());
	} else {
		g->SetColor(0x00, 0x00, 0x00, 0xff);
		g->FillRectangle(0, 0, GetWidth(), GetHeight());
	}
}

VideoPlayer::VideoPlayer(tuner::Locator *mrl)
{
	_volume = 1;
	_mute = false;

	_component = new VideoComponent(0, 0, VIDEO_WIDTH, VIDEO_HEIGHT);
	_offscreen = jgui::Image::CreateImage(jgui::JPF_ARGB, VIDEO_WIDTH, VIDEO_HEIGHT);
	
	((VideoComponent *)_component)->SetOffScreenImage(_offscreen);

	_screen_width = jgui::GFXHandler::GetInstance()->GetScreenWidth();
	_screen_height = jgui::GFXHandler::GetInstance()->GetScreenHeight();
	_scale_width = 1920;
	_scale_height = 1080;
	
	xine = NULL;
	vo = NULL;
	ao = NULL;
	stream = NULL;
	osd = NULL;
	data = NULL;
	event = NULL;

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

	cx = 0;
	cy = 0;
	cw = 320;
	ch = 240;

	IDirectFBSurface *surface = NULL;
	dfb_visual_t visual;
	
	data = new struct frame_data;

	data->window       = NULL;
	data->frame        = NULL;
	data->osd          = NULL;
	data->dest         = NULL;
	data->progress     = 0;
	data->component    = _component;
	data->source       = (uint8_t *)this;

	if (_mrl == "") {
		throw MediaException("MRL cannot be empty");
		return;
	}

	xine = xine_new();
	if (xine == NULL) {
		throw MediaException("Problem at the xine create");
	}

	xine_init(xine);
	
	xine_engine_set_param(xine, XINE_ENGINE_PARAM_VERBOSITY, XINE_VERBOSITY_NONE);

	visual.destination  = NULL;
	visual.subpicture   = NULL;
	visual.output_cb    = output_cb;
	visual.output_cdata = (void *)data;
	visual.frame_cb     = frame_cb;
	visual.frame_cdata  = (void *)data;
	
	vo = xine_open_video_driver(xine, "DFB", XINE_VISUAL_TYPE_DFB, (void*) &visual);
	if (vo == NULL) {
		throw MediaException("Inexistent DFB driver for xine");
	}

	ao = xine_open_audio_driver(xine, "alsa", NULL);
	if (!ao) {
		ao = xine_open_audio_driver(xine, "oss", NULL);
		if (!ao) {
			ao = xine_open_audio_driver(xine, "null", NULL);
			xine_engine_set_param(xine, XINE_PARAM_IGNORE_AUDIO, 1);
		}
	}

	stream = xine_stream_new(xine, ao, vo);
	if (stream == NULL) {
		throw MediaException("Problem at the stream creation");
	}

	// be verbose 
	xine_set_param(stream, XINE_PARAM_VERBOSITY, XINE_VERBOSITY_LOG);

	if (!xine_open(stream, _mrl.c_str())) {
		((VideoComponent *)data->component)->Release();
		throw MediaException("Xine cannot open mrl");
	}

	// query video size 
	data->width  = xine_get_stream_info(stream, XINE_STREAM_INFO_VIDEO_WIDTH);
	data->height = xine_get_stream_info(stream, XINE_STREAM_INFO_VIDEO_HEIGHT);
	data->width  = (data->width < 1)?320:data->width;
	data->height = (data->height < 1)?240:data->height;
	
	data->screen_width  = jgui::GFXHandler::GetInstance()->GetScreenWidth();
	data->screen_height = jgui::GFXHandler::GetInstance()->GetScreenHeight();

	surface = (IDirectFBSurface *)_offscreen->GetGraphics()->GetNativeSurface();

	// setup buffers and update video driver's visual 
	if (!update_visual(surface, vo, &visual, data->width, data->height)) {
		// throw MediaException(""):
	}

	pthread_mutex_init( &data->mutex, NULL );

	// create the osd renderer 
	osd = xine_osd_new(stream, 0, 0, 50, 50);
	if (osd == NULL) {
		throw MediaException("Problems at the osd creation");
	}

	event = xine_event_new_queue(stream);
	if (event == NULL) {
		// throw MediaException(""):
	}

	xine_event_create_listener_thread(event, media_event_listener, data);

	xine_osd_set_font( osd, "cetus", 20 );
	xine_osd_set_text_palette( osd, XINE_TEXTPALETTE_WHITE_BLACK_TRANSPARENT, XINE_OSD_TEXT1 );
	xine_osd_set_position( osd, 0, 0 );
}

VideoPlayer::~VideoPlayer()
{
	_alive = false;

	WaitThread();

	Stop();

	if (osd) {
		xine_osd_hide( osd, 0 );
		xine_osd_free( osd );
	}

	if (stream) {
		xine_stop( stream );
		xine_close( stream );
		xine_dispose( stream );
	}

	if (ao) {
		xine_close_audio_driver( xine, ao );
	}
	
	if (vo) {
		xine_close_video_driver( xine, vo );
	}
	
	/*
	if (config) {
		xine_config_save( xine, config );
		free( config );
	}
	*/

	if (xine)
		xine_exit( xine );

	/*
	if (buffer)
		buffer->Release( buffer );
	*/
	
	if (event) {
		xine_event_dispose_queue(event);
	}

	if (data->frame != NULL) {
		data->frame->ReleaseSource(data->frame);
	}

	if (data->osd != NULL) {
		data->osd->ReleaseSource(data->osd);
	}

	if (_offscreen != NULL) {
		delete _offscreen;
	}

	pthread_mutex_destroy(&data->mutex);
	
	if (_component != NULL) {
		// ((VideoComponent *)_component)->SetSignal();
		((VideoComponent *)_component)->Repaint();
	}
}

void VideoPlayer::Run()
{
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

	/*
	close(fd);
	unlink("/tmp/video.ts");
	*/
}

jgui::Component * VideoPlayer::GetComponent()
{
	return _component;
}

void VideoPlayer::Play()
{
	if (stream == NULL) {
		return;
	}

	int status = xine_get_status(stream),
		speed  = xine_get_param(stream, XINE_PARAM_SPEED);

	if (status != XINE_STATUS_PLAY) {
		xine_play(stream, 0, 0);
	
		DispatchEvent(new MediaPlayerEvent(this, media::STARTED_EVENT));
	} else {
		if (speed != XINE_SPEED_NORMAL) {
			if (paused == true) {
				xine_set_param(stream, XINE_PARAM_SPEED, XINE_SPEED_NORMAL);
		
				DispatchEvent(new MediaPlayerEvent(this, media::RESUMED_EVENT));
			} else {
				xine_play(stream, 0, 0);
		
				DispatchEvent(new MediaPlayerEvent(this, media::STARTED_EVENT));
			}
		}
	}
	
	((VideoComponent *)_component)->SetSignal(false);
}

void VideoPlayer::Pause()
{
	if (stream == NULL) {
		return;
	}

	int speed = xine_get_param(stream, XINE_PARAM_SPEED);

	if (speed != XINE_SPEED_PAUSE) {
		paused = true;

		xine_set_param(stream, XINE_PARAM_SPEED, XINE_SPEED_PAUSE); 
	
		DispatchEvent(new MediaPlayerEvent(this, media::PAUSED_EVENT));
	} else {
		// xine_set_param(a->stream, XINE_PARAM_SPEED, XINE_SPEED_NORMAL);
	}

	((VideoComponent *)_component)->SetSignal(false);
}

void VideoPlayer::Stop()
{
	if (stream == NULL) {
		return;
	}

	/* INFO:: hide osd
	xine_osd_t *osd
	xine_osd_show( osd, 0 );
	// hide in 5 seconds 
	xine_osd_hide( osd, xine_get_current_vpts( stream ) + 5*90000 );
	*/

	int speed = xine_get_param(stream, XINE_PARAM_SPEED);

	if (speed != XINE_SPEED_PAUSE) {
		paused = false;
		xine_set_param(stream, XINE_PARAM_SPEED, XINE_SPEED_PAUSE); 
		
		DispatchEvent(new MediaPlayerEvent(this, media::STOPPED_EVENT));
	}

	SetMediaTime(0);

	((VideoComponent *)_component)->SetSignal(true);
}

void VideoPlayer::Resume()
{
	if (stream == NULL) {
		return;
	}

	int status = xine_get_status(stream),
		speed  = xine_get_param(stream, XINE_PARAM_SPEED);

	if (status == XINE_STATUS_PLAY) {
		if (speed != XINE_SPEED_NORMAL) {
			if (paused == true) {
				xine_set_param(stream, XINE_PARAM_SPEED, XINE_SPEED_NORMAL);
		
				DispatchEvent(new MediaPlayerEvent(this, media::RESUMED_EVENT));
			}
		}
	}
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
	if (stream == NULL) {
		return;
	}

	int pos = -1;

	xine_stop(stream);

	if (!xine_get_stream_info(stream, XINE_STREAM_INFO_SEEKABLE)) {
		return;
	}

	for (int trys = 5; trys--; ) {
		if (xine_get_pos_length(stream, NULL, &pos, NULL)) {
			break;
		}

		usleep( 1000 );
	}

		if (pos >= 0) {
			pos = max(pos, 0);
			xine_play(stream, 0, pos);
			usleep(10);
		}
}

int VideoPlayer::GetMediaTime()
{
	if (stream == NULL) {
		return -1;
	}

	int pos = -1;

	xine_stop(stream);

	if (!xine_get_stream_info(stream, XINE_STREAM_INFO_SEEKABLE)) {
		return -1;
	}

	for (int trys = 5; trys--; ) {
		if (xine_get_pos_length(stream, NULL, &pos, NULL)) {
			break;
		}

		usleep( 1000 );
	}

	if (pos >= 0) {
		pos = max(pos, 0);
	}

	return pos;
}

void VideoPlayer::SetMute(bool b)
{
	if (stream == NULL) {
		return;
	}

	_mute = b;

	if (_mute == false) {
		xine_set_param(stream, XINE_PARAM_AUDIO_VOLUME, CLAMP( _volume, 0, 0xffff ));
	} else {
		xine_set_param(stream, XINE_PARAM_AUDIO_VOLUME, CLAMP( 0, 0, 0xffff ));
	}
}

bool VideoPlayer::IsMute()
{
	return _mute;
}

void VideoPlayer::SetVolume(int i)
{
	if (stream == NULL) {
		return;
	}

	int volume = i;

	if (volume < 0.0) {
		volume = 0;
	} else if (volume > 100) {
		volume = 100;
	}

	_volume = volume;

	// value = xine_get_param(stream, XINE_PARAM_AUDIO_VOLUME);
	xine_set_param(stream, XINE_PARAM_AUDIO_VOLUME, CLAMP( volume, 0, 0xffff ));
}

int VideoPlayer::GetVolume()
{
	return _volume;
}

void VideoPlayer::SetBrightness(int i)
{
	if (stream == NULL) {
		return;
	}

	// 0..65535
	int value = i;

	if (value < 0) {
		value = 0;
	} else if (value > 100) {
		value = 100;
	}

	value = value*655;

	value = xine_get_param(stream, XINE_PARAM_VO_BRIGHTNESS);
	xine_set_param(stream, XINE_PARAM_VO_BRIGHTNESS, CLAMP( value, 0, 0xffff ));
}

void VideoPlayer::SetContrast(int i)
{
	if (stream == NULL) {
		return;
	}

	// 0..65535
	int value = i;

	if (value < 0) {
		value = 0;
	} else if (value > 100) {
		value = 100;
	}

	value = value*655;

	value = xine_get_param(stream, XINE_PARAM_VO_CONTRAST);
	xine_set_param(stream, XINE_PARAM_VO_CONTRAST, CLAMP( value, 0, 0xffff ));
}

void VideoPlayer::SetHue(int i)
{
	if (stream == NULL) {
		return;
	}

	// 0..65535
	int value = i;

	if (value < 0) {
		value = 0;
	} else if (value > 100) {
		value = 100;
	}

	value = value*655;

	value = xine_get_param(stream, XINE_PARAM_VO_HUE);
	xine_set_param(stream, XINE_PARAM_VO_HUE, CLAMP( value, 0, 0xffff ));
}

void VideoPlayer::SetSaturation(int i)
{
	if (stream == NULL) {
		return;
	}

	// 0..65535
	int value = i;

	if (value < 0) {
		value = 0;
	} else if (value > 100) {
		value = 100;
	}

	value = value*655;

	value = xine_get_param(stream, XINE_PARAM_VO_SATURATION);
	xine_set_param(stream, XINE_PARAM_VO_SATURATION, CLAMP( value, 0, 0xffff ));
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

void media_event_listener(void *user_data, const xine_event_t *event)
{
	if (user_data == NULL) {
		return;
	}

	struct frame_data *data = (struct frame_data *)user_data;

	VideoPlayer *player = (VideoPlayer *)data->source;

	if (player->IsLoop() == true) {
		if (event->type == XINE_EVENT_UI_PLAYBACK_FINISHED) {
			struct frame_data *t = (struct frame_data*)user_data;
			VideoPlayer *player = (VideoPlayer *)t->source;

			if (player != NULL) {
				player->Stop();
				player->Play();
			}
		}
	}
	
	if (event->type == XINE_EVENT_UI_PLAYBACK_FINISHED) {
		struct frame_data *t = (struct frame_data*)user_data;
		VideoPlayer *player = (VideoPlayer *)t->source;

		if (player != NULL) {
			player->DispatchEvent(new MediaPlayerEvent(player, media::FINISHED_EVENT));
		}
	}
}

int update_visual( IDirectFBSurface *destination, xine_video_port_t *vo_driver, dfb_visual_t *visual, int width, int height) 
{
	IDirectFBSurface             *frame      = visual->destination;
	IDirectFBSurface             *osd        = visual->subpicture;
	DFBSurfaceDescription         dsc;
	DFBGraphicsDeviceDescription  caps;
	DFBAccelerationMask           mask;
	DFBResult                     err;

	if (width < 1 || height < 1) {
		return 0;
	}

	if (frame) {
		int w, h;

		frame->GetSize(frame, &w, &h);

		if (w == width && h == height) {
			return 1;
		}

		frame->Release(frame);

		frame = NULL;
	}

	((IDirectFB *)jgui::GFXHandler::GetInstance()->GetGraphicEngine())->GetDeviceDescription(((IDirectFB *)jgui::GFXHandler::GetInstance()->GetGraphicEngine()) , &caps );

	if (caps.acceleration_mask & DFXL_STRETCHBLIT) {
		dsc.flags  = (DFBSurfaceDescriptionFlags)(DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_CAPS);
		dsc.caps   = DSCAPS_VIDEOONLY;
		dsc.width  = width;
		dsc.height = height;

		if (format) {
			dsc.flags       = (DFBSurfaceDescriptionFlags)(dsc.flags | DSDESC_PIXELFORMAT);
			dsc.pixelformat = format;
		}

		err = ((IDirectFB *)jgui::GFXHandler::GetInstance()->GetGraphicEngine())->CreateSurface(((IDirectFB *)jgui::GFXHandler::GetInstance()->GetGraphicEngine()) , &dsc, &frame );
		if (err != DFB_OK) { 
			DirectFBError( "couldn't create frame surface", err );
			if (err == DFB_NOVIDEOMEMORY) {
				frame = NULL;
				mask  = DFXL_NONE;
			} else {
				return 0;
			}
		} else {
			/* check if hardware StechBlit is supported for the current format */
			destination->SetBlittingFlags( destination, DSBLIT_NOFX );
			destination->GetAccelerationMask( destination, frame, &mask );
		}

		if (!(mask & DFXL_STRETCHBLIT)) {
			/* fall back to software scaling */
			if (frame) {
				frame->Release( frame );
			}
			destination->AddRef( destination );
			frame = destination;
		} else {
		}
	} else {
		/* fall back to software scaling */
		destination->AddRef( destination );
		frame = destination;
	}

	/* update visual */
	visual->destination = frame;
	visual->subpicture  = osd;
	if (!xine_port_send_gui_data( vo_driver, XINE_GUI_SEND_SELECT_VISUAL, (void*) visual )) { 
		return 0;
	}

	return 1;
}

void output_cb( void *cdata, int width, int height, double ratio, DFBSurfacePixelFormat format, DFBRectangle *dst )
{
	struct frame_data *data = (struct frame_data*) cdata;

	data->width  = width;
	data->height = height;

	dst->x = 0;
	dst->y = 0;
	dst->w = VIDEO_WIDTH;
	dst->h = VIDEO_HEIGHT;
}

void frame_cb( void *cdata )
{
	struct frame_data *data = (struct frame_data*) cdata;

	if (data->component != NULL) {
		data->component->Repaint();
	}
}

}

