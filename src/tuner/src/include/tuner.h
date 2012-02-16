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
#ifndef TUNER_H
#define TUNER_H

#include "mediabank.h"
#include "tunerlistener.h"
#include "tunerstatuslistener.h"
#include "channel.h"

#include <vector>

#include <stdint.h>

namespace tuner {

/**
 * \brief
 * 
 * \author Jeff Ferr
 */
class Tuner{

	protected:
		/** \brief */
		std::vector<TunerListener *> _tuner_listeners;
		/** \brief */
		std::vector<Channel *> _channels;
		/** \brief */
		MediaBank *_source;
		/** \brief */
		int _index;

		/**
		 * \brief
		 *
		 */
		virtual void Update();

	public:
		/**
		* \brief Constritor default.
		*
		*/
		Tuner();

		/**
		* \brief Destrutor virtual.
		*
		*/
		virtual ~Tuner();

		/**
		 * \brief
		 *
		 */
		MediaBank * GetMediaBank();

		/**
		 * \brief
		 *
		 */
		virtual std::vector<Channel *> & ListChannels();

		/**
		 * \brief
		 *
		 */
		virtual void Scan();

		/**
		 * \brief
		 *
		 */
		virtual void InterruptScan();

		/**
		 * \brief
		 *
		 */
		virtual int GetSignalStrength();

		/**
		 * \brief
		 *
		 */
		virtual void SetChannel(Channel *ch);

		/**
		 * \brief
		 *
		 */
		virtual Channel * GetChannel();

		/**
		 * \brief
		 *
		 */
		virtual Channel * NextChannel();

		/**
		 * \brief
		 *
		 */
		virtual Channel * PreviousChannel();

		/**
		 * \brief
		 *
		 */
		virtual int GetNumberOfChannels();

		/**
		 * \brief
		 *
		 */
		virtual void ReleaseChannels();

		/**
		 * \brief
		 *
		 */
		void RegisterTunerListener(TunerListener *listener);

		/**
		 * \brief
		 *
		 */
		void RemoveTunerListener(TunerListener *listener);

		/**
		 * \brief
		 *
		 */
		void RegisterStatusListener(TunerStatusListener *listener);

		/**
		 * \brief
		 *
		 */
		void RemoveStatusListener(TunerStatusListener *listener);

		/**
		 * \brief
		 *
		 */
		void DispatchEvent(TunerEvent *event);

		/**
		 * \brief
		 *
		 */
		void DispatchEvent(TunerStatusEvent *event);

};

};

#endif
