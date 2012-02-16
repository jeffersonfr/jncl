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
#ifndef J_NCLPRESENTATIONEVENT_H
#define J_NCLPRESENTATIONEVENT_H

#include "nclevent.h"
#include "nclplayer.h"

#include <string>
#include <vector>
#include <list>
#include <map>

namespace jncl {

enum nclevent_action_t {
	NCL_ACTION_START,
	NCL_ACTION_STOP,
	NCL_ACTION_PAUSE,
	NCL_ACTION_RESUME,
	NCL_ACTION_ABORT
};

enum nclevent_transition_t {
	NCL_TRANSITION_START,
	NCL_TRANSITION_STOP,
	NCL_TRANSITION_PAUSE,
	NCL_TRANSITION_RESUME,
	NCL_TRANSITION_ABORT,
	NCL_TRANSITION_ABORT_FROM_PAUSED,
	NCL_TRANSITION_END
};

/**
 * \brief
 * 
 * \author Jeff Ferr
 */
class NCLPresentationEvent : public NCLEvent{

	private:
		NCLPlayer *_player;
		nclevent_action_t _action;
		nclevent_transition_t _transition;

	public:
		NCLPresentationEvent(NCLPlayer *player, nclevent_action_t action, nclevent_transition_t transition);
		virtual ~NCLPresentationEvent();

		NCLPlayer * GetPlayer();
		nclevent_action_t GetAction();
		nclevent_transition_t GetTransition();

};

}

#endif

