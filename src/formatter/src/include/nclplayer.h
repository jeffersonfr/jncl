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
#ifndef J_NCLPLAYER_H
#define J_NCLPLAYER_H

#include "nclloader.h"
#include "ncltimerlistener.h"
#include "ncltimer.h"

#include "jgui/jwindow.h"

#include <string>
#include <vector>
#include <list>
#include <map>
#include <mutex>

namespace jncl {

enum nclplayer_type_t {
	NCL_PLAYER_IMAGE,
	NCL_PLAYER_VIDEO,
	NCL_PLAYER_AUDIO,
	NCL_PLAYER_TEXT,
	NCL_PLAYER_XHTML
};

enum ncltransition_type_t {
	NCL_TRANSITION_NONE
};

class NCLFormatter;

/**
 * \brief
 * 
 * \author Jeff Ferr
 */
class NCLPlayer : public NCLTimerListener{

	protected:
		std::vector<struct ncllink_t *> _links;
		std::mutex _mutex;
		jgui::Window *_component;
		struct nclmedia_t *_media;
		int _transition_time;
		bool _is_alive,
				 _is_idle,
				 _has_focus;
		ncltransition_type_t _transition;
		nclplayer_type_t _type;

	public:
		NCLPlayer(struct nclmedia_t *media, nclplayer_type_t type);
		virtual ~NCLPlayer();

		virtual bool IsAlive();

		virtual void SetFocus(bool b);
		virtual bool HasFocus();

		virtual bool IsIdle();
		virtual void WaitIdle();

		virtual void SetTransition(ncltransition_type_t transition);
		virtual void SetTransitionTime(int time);

		virtual void Hide();

		virtual void Play();
		virtual void Stop();
		virtual void Pause();
		virtual void Resume();
		virtual void Abort();

		virtual void Release();

		virtual nclmedia_t * GetMedia();

		virtual jgui::Window * GetComponent();

		virtual std::vector<ncllink_t *> * GetLinks();
		
		virtual void TimeoutReceived(NCLTimerEvent *event);

};

}

#endif

