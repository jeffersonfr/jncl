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
#ifndef J_NCLINPUT_H
#define J_NCLINPUT_H

#include "ncleventlistener.h"
#include "jmutex.h"
#include "jkeylistener.h"

#include <string>
#include <vector>
#include <list>
#include <map>

namespace jncl {

/**
 * \brief
 * 
 * \author Jeff Ferr
 */
class NCLInput : public jgui::KeyListener{

	private:
		static NCLInput *_instance;

		std::vector<NCLEventListener *> _listeners;
		jthread::Mutex _mutex;

	public:
		NCLInput();
		virtual ~NCLInput();

		static NCLInput * GetInstance();

		void Init();
		void Release();

		void RemoveAllListeners();

		void RegisterEventListener(NCLEventListener *listener);
		void RemoveEventListener(NCLEventListener *listener);
		void DispatchEvent(NCLEvent *event);

		virtual bool KeyPressed(jgui::KeyEvent *event);

};

}

#endif

