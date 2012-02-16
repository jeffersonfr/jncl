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
#ifndef J_NCLTIMER_H
#define J_NCLTIMER_H

#include "ncltimerlistener.h"
#include "jmutex.h"
#include "jthread.h"
#include "jsemaphore.h"

#include <string>
#include <list>
#include <vector>
#include <map>

namespace jncl {

/**
 * \brief
 * 
 * \author Jeff Ferr
 */
class NCLTimer : public jthread::Thread{

	private:
		static NCLTimer *_instance;

		struct ncltimer_t {
			NCLTimerListener *listener;
			std::string name;
			time_t delay;
			time_t loop;
		};

	private:
		std::vector<struct ncltimer_t *> _listeners;
		jthread::Mutex _mutex;
		jthread::Semaphore _sem;
		bool _running;

	public:
		NCLTimer();
		virtual ~NCLTimer();

		static NCLTimer * GetInstance();

		void Init();
		void Release();

		void RemoveAllListeners();

		void RegisterTimerListener(NCLTimerListener *listener, std::string name, time_t delay, time_t loop = 0);
		void RemoveTimerListener(NCLTimerListener *listener);

		virtual void Run();

};

}

#endif

