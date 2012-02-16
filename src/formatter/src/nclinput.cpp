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
#include "nclinput.h"
#include "nclhelper.h"
#include "jstringutils.h"
#include "jinputmanager.h"

#include <algorithm>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

namespace jncl {

NCLInput *NCLInput::_instance = NULL;

NCLInput::NCLInput()
{
}

NCLInput::~NCLInput()
{
}

NCLInput * NCLInput::GetInstance()
{
	if (_instance == NULL) {
		_instance = new NCLInput();

		_instance->Init();
	}

	return _instance;
}

void NCLInput::Init()
{
	jgui::InputManager::GetInstance()->RegisterKeyListener(this);
}

void NCLInput::Release()
{
	RemoveAllListeners();

	jgui::InputManager::GetInstance()->RemoveKeyListener(this);
}

void NCLInput::RemoveAllListeners()
{
	_mutex.Lock();

	_listeners.clear();
	
	_mutex.Unlock();
}

void NCLInput::RegisterEventListener(NCLEventListener *listener)
{
	_mutex.Lock();

	std::vector<NCLEventListener *>::iterator i = std::find(_listeners.begin(), _listeners.end(), listener);

	if (i == _listeners.end()) {
		_listeners.push_back(listener);
	}

	_mutex.Unlock();
}

void NCLInput::RemoveEventListener(NCLEventListener *listener)
{
	_mutex.Lock();

	std::vector<NCLEventListener *>::iterator i = std::find(_listeners.begin(), _listeners.end(), listener);

	if (i != _listeners.end()) {
		_listeners.erase(i);
	}

	_mutex.Unlock();
}

void NCLInput::DispatchEvent(NCLEvent *event)
{
	int k = 0;

	while (k < (int)_listeners.size()) {
		_listeners[k++]->ActionPerformed(event);
	}

	delete event;
}

void NCLInput::KeyPressed(jgui::KeyEvent *event)
{
	if (event->GetType() != jgui::JKT_PRESSED) {
		return;
	}

	DispatchEvent(new NCLSelectionEvent(NCLHelper::TranslateKey(event->GetSymbol())));
}

}

