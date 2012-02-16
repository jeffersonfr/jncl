/*
 * DVBTuner.cpp
 *
 *  Created on: 27/11/2009
 *      Author: marcovas
 */

#include "dvbtuner.h"
#include "tunerexception.h"
#include "jfileinputstream.h"
#include "jbufferedreader.h"
#include "jstringtokenizer.h"
#include "dvbchannel.h"
#include "dvbmediabank.h"
#include "jxmlparser.h"

#include <iostream>
#include <sstream>
#include <iomanip>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
#include <errno.h>
#include <signal.h>

#include <linux/dvb/frontend.h>
#include <linux/dvb/dmx.h>

namespace tuner {

int GetFrequencyByName(std::string frequency)
{
	return atoi(frequency.c_str());
}

fe_spectral_inversion_t GetInversionByName(std::string inversion)
{
	if (inversion == "INVERSION_OFF") {
		return INVERSION_OFF;
	} else if (inversion == "INVERSION_ON") {
		return INVERSION_ON;
	} else if (inversion == "INVERSION_AUTO") {
		return INVERSION_AUTO;
	}

	return INVERSION_AUTO;
}

fe_bandwidth_t GetBandwidthByName(std::string bandwidth)
{
	if (bandwidth == "BANDWIDTH_8_MHZ") {
		return BANDWIDTH_8_MHZ;
	} else if (bandwidth == "BANDWIDTH_7_MHZ") {
		return BANDWIDTH_7_MHZ;
	} else if (bandwidth == "BANDWIDTH_6_MHZ") {
		return BANDWIDTH_6_MHZ;
	} else if (bandwidth == "BANDWIDTH_AUTO") {
		return BANDWIDTH_AUTO;
	}

	return BANDWIDTH_AUTO;
}

fe_code_rate_t GetCodeRateHPByName(std::string coderate)
{
	fe_code_rate_t code = FEC_NONE;

	if (coderate == "FEC_1_2") {
		code = FEC_1_2;
	} else if (coderate == "FEC_2_3") {
		code = FEC_2_3;
	} else if (coderate == "FEC_3_4") {
		code = FEC_3_4;
	} else if (coderate == "FEC_4_5") {
		code = FEC_4_5;
	} else if (coderate == "FEC_5_6") {
		code = FEC_5_6;
	} else if (coderate == "FEC_6_7") {
		code = FEC_6_7;
	} else if (coderate == "FEC_7_8") {
		code = FEC_7_8;
	} else if (coderate == "FEC_8_9") {
		code = FEC_8_9;
	} else if (coderate == "FEC_AUTO") {
		code = FEC_AUTO;
	} else if (coderate == "FEC_NONE") {
		code = FEC_NONE;
	}

	return code;
}

fe_code_rate_t GetCodeRateLPByName(std::string coderate)
{
	return GetCodeRateHPByName(coderate);
}
	
fe_modulation_t GetConstellationByName(std::string constellation)
{
	if (constellation == "QPSK") {
		return QPSK;
	} else if (constellation == "QAM_16") {
		return QAM_16;
	} else if (constellation == "QAM_32") {
		return QAM_32;
	} else if (constellation == "QAM_64") {
		return QAM_64;
	} else if (constellation == "QAM_128") {
		return QAM_128;
	} else if (constellation == "QAM_256") {
		return QAM_256;
	} else if (constellation == "QAM_AUTO") {
		return QAM_AUTO;
	} else if (constellation == "VSB_8") {
		return VSB_8;
	} else if (constellation == "VSB_16") {
		return VSB_16;
	} else if (constellation == "PSK_8") {
		// return PSK_8;
	} else if (constellation == "APSK_16") {
		// return APSK_16;
	} else if (constellation == "APSK_32") {
		// return APSK_32;
	} else if (constellation == "DQPSK") {
		// return DQPSK;
	}

	return QAM_AUTO;
}

fe_transmit_mode_t GetTransmissionModeByName(std::string transmission_mode)
{
	if (transmission_mode == "TRANSMISSION_MODE_2K") {
		return TRANSMISSION_MODE_2K;
	} else if (transmission_mode == "TRANSMISSION_MODE_8K") {
		return TRANSMISSION_MODE_8K;
	} else if (transmission_mode == "TRANSMISSION_MODE_AUTO") {
		return TRANSMISSION_MODE_AUTO;
	}

	return TRANSMISSION_MODE_AUTO;
}
	
fe_guard_interval_t GetGuardIntervalByName(std::string guard_interval)
{
	if (guard_interval == "GUARD_INTERVAL_1_32") {
		return GUARD_INTERVAL_1_32;
	} else if (guard_interval == "GUARD_INTERVAL_1_16") {
		return GUARD_INTERVAL_1_16;
	} else if (guard_interval == "GUARD_INTERVAL_1_8") {
		return GUARD_INTERVAL_1_8;
	} else if (guard_interval == "GUARD_INTERVAL_1_4") {
		return GUARD_INTERVAL_1_4;
	} else if (guard_interval == "GUARD_INTERVAL_AUTO") {
		return GUARD_INTERVAL_AUTO;
	}

	return GUARD_INTERVAL_AUTO;
}

fe_hierarchy_t GetHierarchyInformationByName(std::string hierarchy_information)
{
	if (hierarchy_information == "HIERARCHY_NONE") {
		return HIERARCHY_NONE;
	} else if (hierarchy_information == "HIERARCHY_1") {
		return HIERARCHY_1;
	} else if (hierarchy_information == "HIERARCHY_2") {
		return HIERARCHY_2;
	} else if (hierarchy_information == "HIERARCHY_4") {
		return HIERARCHY_4;
	} else if (hierarchy_information == "HIERARCHY_AUTO") {
		return HIERARCHY_AUTO;
	}

	return HIERARCHY_AUTO;
}

std::string GetFrequencyByID(int frequency)
{
	std::ostringstream o;

	o << frequency << std::flush;

	return o.str();
}

std::string GetInversionByID(fe_spectral_inversion_t inversion)
{
	if (inversion == INVERSION_OFF) {
		return "INVERSION_OFF";
	} else if (inversion == INVERSION_ON) {
		return "INVERSION_ON";
	} else if (inversion == INVERSION_AUTO) {
		return "INVERSION_AUTO";
	}

	return "INVERSION_AUTO";
}

std::string GetBandwidthByID(fe_bandwidth_t bandwidth)
{
	if (bandwidth == BANDWIDTH_8_MHZ) {
		return "BANDWIDTH_8_MHZ";
	} else if (bandwidth == BANDWIDTH_7_MHZ) {
		return "BANDWIDTH_7_MHZ";
	} else if (bandwidth == BANDWIDTH_6_MHZ) {
		return "BANDWIDTH_6_MHZ";
	} else if (bandwidth == BANDWIDTH_AUTO) {
		return "BANDWIDTH_AUTO";
	}

	return "BANDWIDTH_AUTO";
}

std::string GetCodeRateHPByID(fe_code_rate_t coderate)
{
	std::string code = "FEC_NONE";

	if (coderate == FEC_1_2) {
		code = "FEC_1_2";
	} else if (coderate == FEC_2_3) {
		code = "FEC_2_3";
	} else if (coderate == FEC_3_4) {
		code = "FEC_3_4";
	} else if (coderate == FEC_4_5) {
		code = "FEC_4_5";
	} else if (coderate == FEC_5_6) {
		code = "FEC_5_6";
	} else if (coderate == FEC_6_7) {
		code = "FEC_6_7";
	} else if (coderate == FEC_7_8) {
		code = "FEC_7_8";
	} else if (coderate == FEC_8_9) {
		code = "FEC_8_9";
	} else if (coderate == FEC_AUTO) {
		code = "FEC_AUTO";
	} else if (coderate == FEC_NONE) {
		code = "FEC_NONE";
	}

	return code;
}

std::string GetCodeRateLPByID(fe_code_rate_t coderate)
{
	return GetCodeRateHPByID(coderate);
}
	
std::string GetConstellationByID(fe_modulation_t constellation)
{
	if (constellation == QPSK) {
		return "QPSK";
	} else if (constellation == QAM_16) {
		return "QAM_16";
	} else if (constellation == QAM_32) {
		return "QAM_32";
	} else if (constellation == QAM_64) {
		return "QAM_64";
	} else if (constellation == QAM_128) {
		return "QAM_128";
	} else if (constellation == QAM_256) {
		return "QAM_256";
	} else if (constellation == QAM_AUTO) {
		return "QAM_AUTO";
	} else if (constellation == VSB_8) {
		return "VSB_8";
	} else if (constellation == VSB_16) {
		return "VSB_16";
	/*
	} else if (constellation == PSK_8) {
		return "PSK_8";
	} else if (constellation == APSK_16) {
		return "APSK_16";
	} else if (constellation == APSK_32) {
		return "APSK_32";
	} else if (constellation == DQPSK) {
		return "DQPSK";
	*/
	}

	return "QAM_AUTO";
}

std::string GetTransmissionModeByID(fe_transmit_mode_t transmission_mode)
{
	if (transmission_mode == TRANSMISSION_MODE_2K) {
		return "TRANSMISSION_MODE_2K";
	} else if (transmission_mode == TRANSMISSION_MODE_8K) {
		return "TRANSMISSION_MODE_8K";
	} else if (transmission_mode == TRANSMISSION_MODE_AUTO) {
		return "TRANSMISSION_MODE_AUTO";
	}

	return "TRANSMISSION_MODE_AUTO";
}
	
std::string GetGuardIntervalByID(fe_guard_interval_t guard_interval)
{
	if (guard_interval == GUARD_INTERVAL_1_32) {
		return "GUARD_INTERVAL_1_32";
	} else if (guard_interval == GUARD_INTERVAL_1_16) {
		return "GUARD_INTERVAL_1_16";
	} else if (guard_interval == GUARD_INTERVAL_1_8) {
		return "GUARD_INTERVAL_1_8";
	} else if (guard_interval == GUARD_INTERVAL_1_4) {
		return "GUARD_INTERVAL_1_4";
	} else if (guard_interval == GUARD_INTERVAL_AUTO) {
		return "GUARD_INTERVAL_AUTO";
	}

	return "GUARD_INTERVAL_AUTO";
}

std::string GetHierarchyInformationByID(fe_hierarchy_t hierarchy_information)
{
	if (hierarchy_information == HIERARCHY_NONE) {
		return "HIERARCHY_NONE";
	} else if (hierarchy_information == HIERARCHY_1) {
		return "HIERARCHY_1";
	} else if (hierarchy_information == HIERARCHY_2) {
		return "HIERARCHY_2";
	} else if (hierarchy_information == HIERARCHY_4) {
		return "HIERARCHY_4";
	} else if (hierarchy_information == HIERARCHY_AUTO) {
		return "HIERARCHY_AUTO";
	}

	return "HIERARCHY_AUTO";
}

DVBTuner::DVBTuner(int f, int a, int dm, int dv)
{
	configFile = "/etc/channels.conf"; 

	frontend = f;
	adapter = a;
	demux = dm;
	dvr = dv;
	frontendFD = 0;
	
	_source = new DVBMediaBank();

	Load();
}

DVBTuner::~DVBTuner() 
{
}

int DVBTuner::GetSignalStrength()
{
	return dynamic_cast<DVBMediaBank *>(_source)->GetSignalStrength();
}
		
void DVBTuner::Load()
{
	jcommon::XmlDocument doc("./config/channels.xml");

	if (!doc.LoadFile()) {
		return;
	}

	jcommon::XmlElement *root,
			   *psg;

	// parser servern node
	root = doc.RootElement();

	_index = -1;

	if (root != NULL) {
		if (strcmp(root->Value(), "channels") == 0) {
			std::string last,
				type,
				name,
				frequency,
				inversion,
				bandwidth,
				coderate_hp,
				coderate_lp,
				constellation,
				transmission_mode,
				guard_interval,
				hierarchy_information;

			if (root->Attribute("last") != NULL) {
				type = root->Attribute("last");
			}

			psg = root->FirstChildElement("channel");

			do {
				if (psg == NULL || strcmp(psg->Value(), "channel") != 0) {
					break;
				}

				if (psg->Attribute("type") != NULL) {
					type = psg->Attribute("type");
				}

				if (type == "dvb") {
					if (psg->Attribute("name") != NULL) {
						name = psg->Attribute("name");
					}

					if (psg->Attribute("frequency") != NULL) {
						frequency = psg->Attribute("frequency");
					}

					if (psg->Attribute("bandwidth") != NULL) {
						bandwidth = psg->Attribute("bandwidth");
					}

					if (psg->Attribute("coderatehp") != NULL) {
						coderate_hp = psg->Attribute("coderatehp");
					}

					if (psg->Attribute("coderatelp") != NULL) {
						coderate_lp = psg->Attribute("coderatelp");
					}

					if (psg->Attribute("constellation") != NULL) {
						constellation = psg->Attribute("constellation");
					}

					if (psg->Attribute("transmission-mode") != NULL) {
						transmission_mode = psg->Attribute("transmission-mode");
					}

					if (psg->Attribute("guard-interval") != NULL) {
						guard_interval = psg->Attribute("guard-interval");
					}

					if (psg->Attribute("hierarchy-information") != NULL) {
						hierarchy_information = psg->Attribute("hierarchy-information");
					}

					struct dvb_frontend_parameters frontend;

					frontend.frequency = GetFrequencyByName(frequency);
					frontend.inversion = GetInversionByName(inversion);
					frontend.u.ofdm.bandwidth = GetBandwidthByName(bandwidth);
					frontend.u.ofdm.code_rate_HP = GetCodeRateHPByName(coderate_hp);
					frontend.u.ofdm.code_rate_LP = GetCodeRateLPByName(coderate_lp);
					frontend.u.ofdm.constellation = GetConstellationByName(constellation);
					frontend.u.ofdm.transmission_mode = GetTransmissionModeByName(transmission_mode);
					frontend.u.ofdm.guard_interval = GetGuardIntervalByName(guard_interval);
					frontend.u.ofdm.hierarchy_information = GetHierarchyInformationByName(hierarchy_information);

					_channels.push_back(new DVBChannel(name, &frontend));

					if (psg->Attribute("last") != NULL) {
						_index = _channels.size()-1;
					}
				}
			} while ((psg = psg->NextSiblingElement("channel")) != NULL);
		}
	}
	
	if (_channels.size() > 0) {

		if (_index < 0) {
			_index = 0;
		}

		SetChannel(_channels[_index]);
	}
}

void DVBTuner::Save()
{
	std::ostringstream o;

	o << "<channels>\r\n";

	Channel *current = GetChannel();

	for (std::vector<Channel *>::iterator i=_channels.begin(); i!=_channels.end(); i++) {
		DVBChannel *ch = (DVBChannel *)(*i);

		o << "\t<channel type=\"dvb\" name=\"" << ch->GetName() 
			<< "\" frequency=\"" << GetFrequencyByID(ch->GetFrequency())
			<< "\" bandwidth=\"" << GetBandwidthByID(ch->GetBandwidth())
			<< "\" coderatehp=\"" << GetCodeRateHPByID(ch->GetCodeRateHP()) 
			<< "\" coderatelp=\"" << GetCodeRateLPByID(ch->GetCodeRateLP())
			<< "\" constellation=\"" << GetConstellationByID(ch->GetConstellation())
			<< "\" transmission-mode=\"" << GetTransmissionModeByID(ch->GetTransmissionMode())
			<< "\" guard-interval=\"" << GetGuardIntervalByID(ch->GetGuardInterval())
			<< "\" hierarchy-information =\"" << GetHierarchyInformationByID(ch->GetHierarchyInformation())
			<< "\" ";
	
		if (ch == current) {
			o << "last=\"true\" ";
		}
			
		o << "/>\r\n";
	}
	
	o << "</channels>\r\n";

	jio::File file("./config/channels.xml", jio::F_CREATE | jio::F_TRUNCATE | jio::F_READ_WRITE);

	file.Write(o.str().c_str(), o.str().size());
	file.Close();
}

void DVBTuner::Update()
{
	Save();
}

void DVBTuner::Scan()
{
	_channels.clear();

	DVBScan scan;

	scan.AddScanListener(*this);
	scan.StartScan();
	scan.WaitThread();
}

void DVBTuner::InterruptScan()
{
	// TODO::
}

void DVBTuner::ScanStarted()
{
	std::cout << "Scan iniciado" << std::endl;
}

void DVBTuner::ScanFinished()
{
	std::cout << "Scan finalizado" << std::endl;
	
	if (_channels.size() > 0) {
		_index = 0;

		// SetChannel(_channels[_index]);
	} else {
		_index = -1;
	}
	
	Update();
}

void DVBTuner::ScanCancelled(SBTVDNetwork *n)
{
	std::cout << "Scan cancelado" << std::endl;
}

void DVBTuner::NetworkReached(SBTVDNetwork *n)
{
	std::cout << "Rede encontrada" << std::endl;

	// fill channel
	struct dvb_frontend_parameters frontend;

	frontend.frequency = n->getFrequency();
	frontend.inversion = GetInversionByName("INVERSION_AUTO");
	frontend.u.ofdm.bandwidth = GetBandwidthByName("BANDWIDTH_6_MHZ");
	frontend.u.ofdm.code_rate_HP = GetCodeRateHPByName("FEC_3_4");
	frontend.u.ofdm.code_rate_LP = GetCodeRateLPByName("FEC_AUTO");
	frontend.u.ofdm.constellation = GetConstellationByName("QAM_AUTO");
	frontend.u.ofdm.transmission_mode = GetTransmissionModeByName("TRANSMISSION_MODE_AUTO");
	frontend.u.ofdm.guard_interval = GetGuardIntervalByName("GUARD_INTERVAL_AUTO");
	frontend.u.ofdm.hierarchy_information = GetHierarchyInformationByName("HIERARCHY_NONE");

	Channel *ch = new DVBChannel(n->getNetworkName(), &frontend);

	_channels.push_back(ch);

	DispatchEvent(new TunerEvent(this, ch, NEW_CHANNEL));
}

void DVBTuner::ScanError()
{
	std::cout << "Erro no scan" << std::endl;
}

void DVBTuner::StartedScanChannel(unsigned short c)
{
}

void DVBTuner::FinishedScanChannel(unsigned short c)
{
}

}

