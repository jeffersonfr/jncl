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
#ifndef TUNEREVENT_H
#define TUNEREVENT_H

#include "channel.h"

#include "jevent/jeventobject.h"

#include <stdint.h>

namespace tuner {

enum tuner_event_t {
	NEW_CHANNEL,
	TUNE_COMPLETE,
	TUNE_FAILED
};

/**
 * \brief
 * 
 * \author Jeff Ferr
 */
class TunerEvent : public jevent::EventObject {

	private:
		/** \brief */
		Channel *_channel;
		/** \brief */
		tuner_event_t _type;

	protected:

	public:
		/**
		* \brief Constritor default.
		*
		*/
		TunerEvent(void *source, Channel *channel, tuner_event_t type);

		/**
		* \brief Destrutor virtual.
		*
		*/
		virtual ~TunerEvent();

		/**
		* \brief
		*
		*/
		Channel * GetChannel();

		/**
		* \brief
		*
		*/
		tuner_event_t GetType();

};

};

#endif
