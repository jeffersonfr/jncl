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
#ifndef DVBCHANNEL_H
#define DVBCHANNEL_H

#include "channel.h"

#include <string>

#include <stdint.h>

#include <linux/dvb/frontend.h>
#include <linux/dvb/dmx.h>

namespace tuner {

/**
 * \brief
 * 
 * \author Jeff Ferr
 */
class DVBChannel : public Channel{

	private:
		struct dvb_frontend_parameters _frontend;

	public:
		/**
		 * \brief Constritor default.
		 *
		 */
		DVBChannel(std::string name, struct dvb_frontend_parameters *frontend);

		/**
		 * \brief Destrutor virtual.
		 *
		 */
		virtual ~DVBChannel();

		/**
		 * \brief
		 *
		 */
		std::string GetName();

		/**
		 * \brief
		 *
		 */
		int GetFrequency();

		/**
		 * \brief
		 *
		 */
		fe_spectral_inversion_t GetInversion();

		/**
		 * \brief
		 *
		 */
		fe_bandwidth_t GetBandwidth();

		/**
		 * \brief
		 *
		 */
		fe_code_rate_t GetCodeRateHP();

		/**
		 * \brief
		 *
		 */
		fe_code_rate_t GetCodeRateLP();

		/**
		 * \brief
		 *
		 */
		fe_modulation_t GetConstellation();

		/**
		 * \brief
		 *
		 */
		fe_transmit_mode_t GetTransmissionMode();

		/**
		 * \brief
		 *
		 */
		fe_guard_interval_t GetGuardInterval();

		/**
		 * \brief
		 *
		 */
		fe_hierarchy_t GetHierarchyInformation();

};

}

#endif
