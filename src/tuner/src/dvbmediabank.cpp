/*
 * DVBMediaBank.cpp
 *
 *  Created on: 24/11/2009
 *      Author: marcovas
 */

#include "dvbmediabank.h"
#include "tunerexception.h"
#include "jstringtokenizer.h"
#include "dvbtuner.h"

#include <iostream>

#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/poll.h>
#include <signal.h>
#include <cstring>
#include <climits>
#include <sstream>

#include <linux/dvb/frontend.h>
#include <linux/dvb/dmx.h>
#include <libdvbapi/dvbdemux.h>

void SetupFrontend(int fe_fd, struct dvb_frontend_parameters *frontend) 
{
	struct dvb_frontend_info fe_info;

	if (ioctl(fe_fd, FE_GET_INFO, &fe_info) < 0) {
		throw tuner::TunerException("ioctl FE_GET_INFO failed");
	}

	if (fe_info.type != FE_OFDM) {
		throw tuner::TunerException("frontend device is not a OFDM (DVB-T) device");
	}

	std::cout << "tuning to " << frontend->frequency << "Hz" << std::endl;

	if (ioctl(fe_fd, FE_SET_FRONTEND, frontend) < 0) {
		throw tuner::TunerException("ioctl FE_SET_FRONTEND failed");
	}
}

namespace tuner {

DVBMediaBank::DVBMediaBank(int fe, int ad, int dmx)
{
	_read_index = 0;
	_pass_index = 0;
	_packet_index = 0;
	_packet_max = 0;

	_buffer = new jthread::IndexedBuffer(1024, 4096);

	_packet = new char[4096];

	adapter = ad;
	demux = dmx;
	frontend = fe;
	fdDVR = -1;
	fdDemux = -1;
	_flag = false;

	fdDVR = dvbdemux_open_dvr(adapter, demux, 1, 0);
	if (fdDVR < 0) {
		std::cerr << "Could not open dvr device on adapter " << adapter << std::endl;
		return;
	}
	
	dvbdemux_set_buffer(fdDVR, 1024 * 1024);
	fdDemux = dvbdemux_open_demux(adapter, demux, 0);
	if (fdDemux < 0) {
		std::cerr << "Could not open demux device on adapter " << adapter << std::endl;
		return;
	}

	// interromper a filtragem antes da troca de fluxo (ultimo parametro)
	// detalhes:: /usr/include/libdvbapi
	if (dvbdemux_set_pid_filter(fdDemux, 529, DVBDEMUX_INPUT_FRONTEND, DVBDEMUX_OUTPUT_DVR, 1)) {
		perror("dvbdemux_set_pid_filter");
		return;
	}
}

DVBMediaBank::~DVBMediaBank() 
{
   if (fdDVR >= 0) {
      close(fdDVR);
	 }
   
	 if (fdDemux >= 0) {
      close(fdDemux);
	 }
}

int DVBMediaBank::GetSignalStrength()
{
	if (_flag == false) {
		return 0;
	}

	uint16_t snr, 
					 _signal;
	uint32_t ber, 
					 uncorrected_blocks;
	fe_status_t status;

	ioctl(fdFrontend, FE_READ_STATUS, &status);
	ioctl(fdFrontend, FE_READ_SIGNAL_STRENGTH, &_signal);
	ioctl(fdFrontend, FE_READ_SNR, &snr);
	ioctl(fdFrontend, FE_READ_BER, &ber);
	ioctl(fdFrontend, FE_READ_UNCORRECTED_BLOCKS, &uncorrected_blocks);
	
	std::cout << "Signal: " << (_signal * 100) / 0xffff << '%' << '\r' << std::flush;

	return (int)((_signal*100)/0xffff);
}

void DVBMediaBank::SetLocator(Locator *locator)
{
	if (locator == NULL) {
		return;
	}

	try {
		_flag = false;

		if (IsRunning() == true) {
			WaitThread();
		}

		close(fdFrontend);

		if (locator->GetProtocol() == "sbtvd") {
			std::ostringstream dev;

			dev << "/dev/dvb/adapter" << adapter << "/frontend" << frontend;

			if ((fdFrontend = open(dev.str().c_str(), O_RDWR)) < 0) {
				throw TunerException(std::string("failed opening " + dev.str()));
			}
			
			struct dvb_frontend_parameters frontend_param;
			jcommon::StringTokenizer tokens(locator->GetPath(), ":", jcommon::SPLIT_FLAG, false);

			frontend_param.frequency = GetFrequencyByName(tokens.GetToken(1));
			frontend_param.inversion = GetInversionByName(tokens.GetToken(2));
			frontend_param.u.ofdm.bandwidth = GetBandwidthByName(tokens.GetToken(3));
			frontend_param.u.ofdm.code_rate_HP = GetCodeRateHPByName(tokens.GetToken(4));
			frontend_param.u.ofdm.code_rate_LP = GetCodeRateLPByName(tokens.GetToken(5));
			frontend_param.u.ofdm.constellation = GetConstellationByName(tokens.GetToken(6));
			frontend_param.u.ofdm.transmission_mode = GetTransmissionModeByName(tokens.GetToken(7));
			frontend_param.u.ofdm.guard_interval = GetGuardIntervalByName(tokens.GetToken(8));
			frontend_param.u.ofdm.hierarchy_information = GetHierarchyInformationByName(tokens.GetToken(9));

			SetupFrontend(fdFrontend, &frontend_param);

			// s << "/dev/dvb/adapter" << adapter << "/demux" << demux;

			_flag = true;

			Start();
		}
	} catch (...) {
		_flag = false;

		if (IsRunning() == true) {
			WaitThread();
		}

		close(fdFrontend);
	}
}

long long DVBMediaBank::Available()
{
	return 0LL;
}

long long DVBMediaBank::GetCapacity()
{
	return 0LL;
}

void DVBMediaBank::Open()
{
}

void DVBMediaBank::Reset()
{
	_buffer->Reset();
}

int DVBMediaBank::AddData(char *data, int size)
{
	return _buffer->Write((uint8_t *)data, size);
}

int DVBMediaBank::GetData(char *data, int size)
{
	jthread::jringbuffer_t t;
	int r,
			d = _packet_max - _packet_index;

	if (d > 0) {
		if (size <= d) {
			memcpy(data, (_packet + _packet_index), size);

			_packet_index = _packet_index + size;

			return size;
		}

		memcpy(data, (_packet + _packet_index), d);
	}

	_packet_index = 0;
	_packet_max = 0;

	r = _buffer->Read(&t, &_read_index, &_pass_index);

	if (r < 0) {
		_buffer->GetIndex(&_read_index, &_pass_index);
	} else {
		memcpy(_packet, t.data, t.size);

		_packet_max = t.size;

		memcpy((data + d), _packet, (size - d));

		_packet_index = size - d;

		return size;
	}

	return -1;
}

void DVBMediaBank::Run() 
{
	const int BUFFER_SIZE = 188;
	unsigned char buffer[7*BUFFER_SIZE];
	int pid,
			index = 0;
	bool ok = false;
	ssize_t readed;
	
	u_long opt = 1;

	ioctl(fdDVR, FIONBIO, &opt);

	// int fd = open("/tmp/video.ts", O_RDWR);

	while (_flag) {
		readed = read(fdDVR, (buffer+index), BUFFER_SIZE);
		
		if (readed <= 0) {
			printf("Error:: %d\n", readed);
	
			usleep(100000);

			continue;
		}
		
		ok = true;

		/*
		pid = ((((unsigned) buffer[1]) << 8) | ((unsigned) buffer[2])) & 0x1FFF;
	
		if (pid != 0x1FFF && out) {
			AddData((char*) buffer, readed); // BUFFER_SIZE);
		}
		*/

		index = index + 188;

		if (index == 7*188) {
			// write(fd, buffer, index);
			AddData((char*) buffer, readed); // BUFFER_SIZE);
			index = 0;
		}
	}

	std::cout << "Capture end" << std::endl;
}

}
