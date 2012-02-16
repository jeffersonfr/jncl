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
#include "dvbchannel.h"
#include "dvbtuner.h"

#include <sstream>

namespace tuner {

DVBChannel::DVBChannel(std::string name, struct dvb_frontend_parameters *frontend):
	Channel(name)
{
	_name = name;

	_frontend.frequency = frontend->frequency;
	_frontend.inversion = frontend->inversion;
	_frontend.u.ofdm.bandwidth = frontend->u.ofdm.bandwidth;
	_frontend.u.ofdm.code_rate_HP = frontend->u.ofdm.code_rate_HP;
	_frontend.u.ofdm.code_rate_LP = frontend->u.ofdm.code_rate_LP;
	_frontend.u.ofdm.constellation = frontend->u.ofdm.constellation;
	_frontend.u.ofdm.transmission_mode = frontend->u.ofdm.transmission_mode;
	_frontend.u.ofdm.guard_interval = frontend->u.ofdm.guard_interval;
	_frontend.u.ofdm.hierarchy_information = frontend->u.ofdm.hierarchy_information;
	
	std::ostringstream o;

	o << "sbtvd://" 
		<< _name << ":"
		<< GetFrequencyByID(_frontend.frequency) << ":"
		<< GetInversionByID(_frontend.inversion) << ":"
		<< GetBandwidthByID(_frontend.u.ofdm.bandwidth) << ":"
		<< GetCodeRateHPByID(_frontend.u.ofdm.code_rate_HP) << ":"
		<< GetCodeRateLPByID(_frontend.u.ofdm.code_rate_LP) << ":"
		<< GetConstellationByID(_frontend.u.ofdm.constellation) << ":"
		<< GetTransmissionModeByID(_frontend.u.ofdm.transmission_mode) << ":"
		<< GetGuardIntervalByID(_frontend.u.ofdm.guard_interval) << ":"
		<< GetHierarchyInformationByID(_frontend.u.ofdm.hierarchy_information) << std::flush;

	_locator = new Locator(o.str());
}

DVBChannel::~DVBChannel()
{
}

std::string DVBChannel::GetName()
{
	return _name;
}

int DVBChannel::GetFrequency()
{
	return _frontend.frequency;
}
	
fe_spectral_inversion_t DVBChannel::GetInversion()
{
	return _frontend.inversion;
}
	
fe_bandwidth_t DVBChannel::GetBandwidth()
{
	return _frontend.u.ofdm.bandwidth;
}

fe_code_rate_t DVBChannel::GetCodeRateHP()
{
	return _frontend.u.ofdm.code_rate_HP;
}
	
fe_code_rate_t DVBChannel::GetCodeRateLP()
{
	return _frontend.u.ofdm.code_rate_LP;
}

fe_modulation_t DVBChannel::GetConstellation()
{
	return _frontend.u.ofdm.constellation;
}

fe_transmit_mode_t DVBChannel::GetTransmissionMode()
{
	return _frontend.u.ofdm.transmission_mode;
}

fe_guard_interval_t DVBChannel::GetGuardInterval()
{
	return _frontend.u.ofdm.guard_interval;
}

fe_hierarchy_t DVBChannel::GetHierarchyInformation()
{
	return _frontend.u.ofdm.hierarchy_information;
}

}

