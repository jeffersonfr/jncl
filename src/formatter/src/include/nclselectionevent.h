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
#ifndef J_NCLSELECTIONEVENT_H
#define J_NCLSELECTIONEVENT_H

#include "nclevent.h"

#include <string>
#include <vector>
#include <list>
#include <map>

namespace jncl {

enum nclevent_symbol_t {
	NCL_SYMBOL_UNKNOWN, 
	NCL_SYMBOL_0, 
	NCL_SYMBOL_1,
	NCL_SYMBOL_2,
	NCL_SYMBOL_3,
	NCL_SYMBOL_4,
	NCL_SYMBOL_5,
	NCL_SYMBOL_6,
	NCL_SYMBOL_7,
	NCL_SYMBOL_8,
	NCL_SYMBOL_9,
	NCL_SYMBOL_A,
	NCL_SYMBOL_B,
	NCL_SYMBOL_C,
	NCL_SYMBOL_D,
	NCL_SYMBOL_E,
	NCL_SYMBOL_F,
	NCL_SYMBOL_G, 
	NCL_SYMBOL_H,
	NCL_SYMBOL_I,
	NCL_SYMBOL_J,
	NCL_SYMBOL_K,
	NCL_SYMBOL_L,
	NCL_SYMBOL_M,
	NCL_SYMBOL_N,
	NCL_SYMBOL_O,
	NCL_SYMBOL_P,
	NCL_SYMBOL_Q,
	NCL_SYMBOL_R,
	NCL_SYMBOL_S, 
	NCL_SYMBOL_T, 
	NCL_SYMBOL_U,
	NCL_SYMBOL_V,
	NCL_SYMBOL_W,
	NCL_SYMBOL_X,
	NCL_SYMBOL_Y,
	NCL_SYMBOL_Z,
	NCL_SYMBOL_ASTERISTIC,
	NCL_SYMBOL_SHARP,
	NCL_SYMBOL_MENU,
	NCL_SYMBOL_INFO,
	NCL_SYMBOL_GUIDE, 
	NCL_SYMBOL_CURSOR_DOWN,  
	NCL_SYMBOL_CURSOR_LEFT,
	NCL_SYMBOL_CURSOR_RIGHT, 
	NCL_SYMBOL_CURSOR_UP, 
	NCL_SYMBOL_CHANNEL_DOWN,
	NCL_SYMBOL_CHANNEL_UP,
	NCL_SYMBOL_VOLUME_DOWN,
	NCL_SYMBOL_VOLUME_UP,
	NCL_SYMBOL_ENTER,
	NCL_SYMBOL_RED, 
	NCL_SYMBOL_GREEN,
	NCL_SYMBOL_YELLOW,
	NCL_SYMBOL_BLUE,
	NCL_SYMBOL_BACK,
	NCL_SYMBOL_EXIT,
	NCL_SYMBOL_POWER,
	NCL_SYMBOL_REWIND, 
	NCL_SYMBOL_STOP, 
	NCL_SYMBOL_EJECT,
	NCL_SYMBOL_PLAY,
	NCL_SYMBOL_RECORD,
	NCL_SYMBOL_PAUSE
};

/**
 * \brief
 * 
 * \author Jeff Ferr
 */
class NCLSelectionEvent : public NCLEvent{

	private:
		nclevent_symbol_t _symbol;

	public:
		NCLSelectionEvent(nclevent_symbol_t symbol);
		virtual ~NCLSelectionEvent();

		nclevent_symbol_t GetSymbol();

};

}

#endif

