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
#ifndef J_NCLEVENT_H
#define J_NCLEVENT_H

#include <string>
#include <vector>
#include <list>
#include <map>

namespace jncl {

enum nclevent_type_t {
	NCL_EVENT_PRESENTATION,
	NCL_EVENT_SELECTION,
	NCL_EVENT_ATTRIBUTION,
	NCL_EVENT_POINTOVER,
	NCL_EVENT_PREFETCH,
	NCL_EVENT_FOCUS
};

/**
 * \brief
 * 
 * \author Jeff Ferr
 */
class NCLEvent{

	private:
		nclevent_type_t _type;

	public:
		NCLEvent(nclevent_type_t type);
		virtual ~NCLEvent();

		nclevent_type_t GetType();

};

}

#endif

