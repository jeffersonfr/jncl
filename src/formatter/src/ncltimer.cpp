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
#include "jautolock.h"
#include "jdate.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

namespace jncl {

NCLTimer *NCLTimer::_instance = NULL;

NCLTimer::NCLTimer():
	jthread::Thread()
{
	_running = true;
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
	Start();
}

void NCLTimer::Release()
{
	_running = false;

	WaitThread();
}

void NCLTimer::RemoveAllListeners()
{
	_listeners.clear();
}

void NCLTimer::RegisterTimerListener(NCLTimerListener *listener, std::string name, time_t delay, time_t loop)
{
	jthread::AutoLock lock(&_mutex);

	for (std::vector<struct ncltimer_t *>::iterator i=_listeners.begin(); i!=_listeners.end(); i++) {
		if ((*i)->listener == listener) {
			return;
		}
	}

	struct ncltimer_t *t = new struct ncltimer_t;

	time_t current = jcommon::Date::CurrentTimeMillis();

	t->listener = listener;
	t->name = name;
	t->delay = current + delay;
	t->loop = loop;

	_listeners.push_back(t);

	_sem.Notify();
}

void NCLTimer::RemoveTimerListener(NCLTimerListener *listener)
{
	jthread::AutoLock lock(&_mutex);

	for (std::vector<struct ncltimer_t *>::iterator i=_listeners.begin(); i!=_listeners.end(); i++) {
		if ((*i)->listener == listener) {
			_listeners.erase(i);

			break;
		}
	}
}

void NCLTimer::Run()
{
	while (_running) {
		if (_listeners.size() == 0) {
			_sem.Wait();
		}

		time_t current = jcommon::Date::CurrentTimeMillis();

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

