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
#include "tuner.h"
#include "filemediabank.h"

#include <stdlib.h>

namespace tuner {

Tuner::Tuner()
{
	_source = NULL;
	_index = -1;
}

Tuner::~Tuner()
{
}

MediaBank * Tuner::GetMediaBank()
{
	return _source;
}

void Tuner::InterruptScan()
{
}

void Tuner::Scan()
{
	// TODO::
}

int Tuner::GetSignalStrength()
{
	return 0;
}

std::vector<Channel *> & Tuner::ListChannels()
{
	return _channels;
}

void Tuner::SetChannel(Channel *ch)
{
	if (ch == NULL) {
		return;
	}

	_index = -1;

	for (int i=0; i<(int)_channels.size(); i++) {
		if (_channels[i] == ch) {
			_index = i;

			break;
		}
	}

	if (_index >= 0) {
		_source->Reset();
		_source->SetLocator(_channels[_index]->GetLocator());
	}

	Update();
}

Channel * Tuner::GetChannel()
{
	if (_index >= 0) {
		return _channels[_index];
	}

	return NULL;
}

Channel * Tuner::NextChannel()
{
	Channel *ch = NULL;

	if (_channels.size() > 0) {
		int old_index = _index;

		_index = (_index+1)%_channels.size();

		if (old_index != _index) {
			ch = _channels[_index];

			SetChannel(ch);
		}
	}

	Update();

	return ch;
}

Channel * Tuner::PreviousChannel()
{
	Channel *ch = NULL;

	if (_channels.size() > 0) {
		int old_index = _index;

		_index = _index-1;
		
		if (_index < 0) {
			_index = _channels.size()-1;
		}

		if (old_index != _index) {
			ch = _channels[_index];

			SetChannel(ch);
		}
	}

	Update();

	return ch;
}

int Tuner::GetNumberOfChannels()
{
	return _channels.size();
}

void Tuner::ReleaseChannels()
{
	Update();
}

void Tuner::Update()
{
}

void Tuner::RegisterTunerListener(TunerListener *listener)
{
	if (listener == NULL) {
		return;
	}

	for (std::vector<TunerListener *>::iterator i=_tuner_listeners.begin(); i!=_tuner_listeners.end(); i++) {
		if ((*i) == listener) {
			return;
		}
	}

	_tuner_listeners.push_back(listener);
}

void Tuner::RemoveTunerListener(TunerListener *listener)
{
	if (listener == NULL) {
		return;
	}

	for (std::vector<TunerListener *>::iterator i=_tuner_listeners.begin(); i!=_tuner_listeners.end(); i++) {
		if ((*i) == listener) {
			_tuner_listeners.erase(i);

			break;
		}
	}
}

void Tuner::DispatchEvent(TunerEvent *event)
{
	if (event == NULL) {
		return;
	}

	for (std::vector<TunerListener *>::iterator i=_tuner_listeners.begin(); i!=_tuner_listeners.end(); i++) {
		if (event->GetType() == NEW_CHANNEL) {
			(*i)->ChannelScanned(event);
		} else if (event->GetType() == TUNE_COMPLETE) {
			(*i)->TuneComplete(event);
		} else if (event->GetType() == TUNE_FAILED) {
			(*i)->TuneFailed(event);
		}
	}

	delete event;
}

};

