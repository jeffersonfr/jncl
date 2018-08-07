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
#include "ncltimer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

namespace jncl {

NCLTimer *NCLTimer::_instance = NULL;

NCLTimer::NCLTimer()
{
	_running = false;
}

NCLTimer::~NCLTimer()
{
	RemoveAllListeners();
}

NCLTimer * NCLTimer::GetInstance()
{
	if (_instance == NULL) {
		_instance = new NCLTimer();

		_instance->Init();
	}

	return _instance;
}

void NCLTimer::Init()
{
	_thread = std::thread(&NCLTimer::Run, this);
}

void NCLTimer::Release()
{
	_running = false;

  _thread.join();
}

void NCLTimer::RemoveAllListeners()
{
	_listeners.clear();
}

void NCLTimer::RegisterTimerListener(NCLTimerListener *listener, std::string name, time_t delay, time_t loop)
{
  std::unique_lock<std::mutex> lock(_mutex);

	for (std::vector<struct ncltimer_t *>::iterator i=_listeners.begin(); i!=_listeners.end(); i++) {
		if ((*i)->listener == listener) {
			return;
		}
	}

	struct ncltimer_t *t = new struct ncltimer_t;

	t->listener = listener;
	t->name = name;
	t->delay = delay + std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
	t->loop = loop;

	_listeners.push_back(t);

	_sem.notify_all();
}

void NCLTimer::RemoveTimerListener(NCLTimerListener *listener)
{
  std::unique_lock<std::mutex> lock(_mutex);

	for (std::vector<struct ncltimer_t *>::iterator i=_listeners.begin(); i!=_listeners.end(); i++) {
		if ((*i)->listener == listener) {
			_listeners.erase(i);

			break;
		}
	}
}

void NCLTimer::Run()
{
  _running = true;

	while (_running) {
		if (_listeners.size() == 0) {
      std::unique_lock<std::mutex> lock(_mutex);

			_sem.wait(lock);
		}

    uint64_t current = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();

		for (int i=0; i<(int)_listeners.size(); i++) {
			struct ncltimer_t *t = _listeners[i];

			if (t->delay <= current) {
				NCLTimerEvent event(t->name);

				t->listener->TimeoutReceived(&event);
			
				if (t->loop > 0) {
					t->delay += t->loop;
				}
			}
		}

		usleep(100);
	}
}

}

