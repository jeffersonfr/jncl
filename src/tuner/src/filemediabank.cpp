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
#include "filemediabank.h"
#include "jtransportstreampacket.h"
#include "jprogramsysteminformationsection.h"
#include "jprogramassociationsection.h"
#include "jprogrammapsection.h"
#include "jautolock.h"

#include <iostream>

#include <fcntl.h>
#include <sys/time.h>

namespace tuner {

const double CLOCK_2_TIME_CONST = (double)1.0/0.09;

uint64_t timeval2int(const timeval *t) 
{
   return (uint64_t)((t->tv_sec)*1000000LL+t->tv_usec);
}

class PatInformation{

	protected:
		int programCount,
				catPID,
				*pmtPIDs;
		bool cat;

		void ParsePat(jmpeg::ProgramAssociationSection *patSection)
		{
			if (patSection->HasFailed() == true) {
				std::cout << "PAT section failed" << std::endl;

				return;
			}
			
			std::vector<jmpeg::ProgramAssociationSection::Program *> program_map;
			int k = 0;

			patSection->GetPrograms(program_map);

			programCount = program_map.size();
			pmtPIDs = new int[programCount];

			for (std::vector<jmpeg::ProgramAssociationSection::Program *>::iterator i=program_map.begin(); i!=program_map.end(); i++) {
				if ((*i)->GetProgramNumber() == 0) {
					cat = true;
					catPID = (*i)->GetProgramID();
				} else {
					pmtPIDs[k++] = (*i)->GetProgramID();
				}

				std::cout << "PMT:: program_number::[" << (*i)->GetProgramNumber() << "], pid::[" << std::hex << (*i)->GetProgramID() << "]" << std::endl;
			}
		}

	public:
		PatInformation()
		{
			programCount = 0;
			pmtPIDs = 0;
			catPID = -1; 
			cat = false;
		}

		PatInformation(jmpeg::ProgramAssociationSection *patSection)
		{
			programCount = 0;
			pmtPIDs = 0;
			catPID = -1; 
			cat = false;

			if (patSection->HasFailed() == true) {
				return;
			}

			SetPatSection(patSection);
		}

		virtual ~PatInformation()
		{
		}

		static jmpeg::ProgramAssociationSection * GetPatSection(uint8_t *packet)
		{
			if (jmpeg::TransportStreamPacket::GetProgramID(packet) == 0) {
				if (jmpeg::TransportStreamPacket::GetPayloadUnitStartIndicator(packet) == 1) {
					jmpeg::ProgramAssociationSection *psi = new jmpeg::ProgramAssociationSection();

					uint8_t pointer,
							payload[188];
					uint32_t size;

					jmpeg::TransportStreamPacket::GetPayload(packet, payload, &size);

					pointer = jmpeg::TransportStreamPacket::GetPointerField(packet);

					psi->Push((payload + pointer + 1), size - pointer - 1);

					return psi;
				}
			}

			std::cout << "PAT section failed" << std::endl;

			return NULL;
		}

		void SetPatSection(jmpeg::ProgramAssociationSection *p)
		{
			ParsePat(p);
		}

		bool HasCAT()
		{
			return cat;
		}

		int GetProgramCount()
		{
			return programCount;
		}

		int GetCatPid()
		{
			return catPID;
		}

		int * GetPmtPIDs()
		{
			return pmtPIDs;
		}

};

class PmtInformation{

	protected:
		jmpeg::ProgramMapSection *pmtSection;
		int audioPID;
		int videoPID;
	
		void ParsePmt ()
		{
			std::vector<jmpeg::ProgramMapSection::Program *> program_map;

			pmtSection->GetPrograms(program_map);

			for (std::vector<jmpeg::ProgramMapSection::Program *>::iterator i=program_map.begin(); i!=program_map.end(); i++) {
				std::cout << "Services:: pid::[" << (*i)->GetElementaryPID() << "], type::[" << (*i)->GetStreamType() << "]" << std::endl;

				int t = (*i)->GetStreamType();

				if (isAudioStream(t)) {
					audioPID = (*i)->GetElementaryPID();
				} else if (isVideoStream(t)) {
					videoPID = (*i)->GetElementaryPID();
				}
			}
		}

	public:
		PmtInformation ():
			pmtSection(0), 
			audioPID(-1), 
			videoPID(-1) 
		{
		}

		PmtInformation(jmpeg::ProgramMapSection *p):
			pmtSection(p), 
			audioPID(-1), 
			videoPID(-1)
		{
			SetData(pmtSection);
		}

		virtual ~PmtInformation()
		{
		}

		static bool isAudioStream(int t)
		{
			switch (t) {
				case 0x02:
				case 0x03:
				case 0x04:
				case 0x0F:
				case 0x11:
				case 0x81:
					return true;
				default:
					return false;
			}
		}

		static bool isVideoStream(int t)
		{
			switch (t) {
				case 0x00:
				case 0x01:
				case 0x02:
				case 0x1B:
					return true;
				default:
					return false;
			}
		}

		static jmpeg::ProgramMapSection * GetPmtSection(uint8_t *packet)
		{
			if (jmpeg::TransportStreamPacket::GetPayloadUnitStartIndicator(packet) == 1) {
				jmpeg::ProgramMapSection *psi = new jmpeg::ProgramMapSection();

				uint8_t pointer,
						payload[188];
				uint32_t size;

				jmpeg::TransportStreamPacket::GetPayload(packet, payload, &size);

				pointer = jmpeg::TransportStreamPacket::GetPointerField(packet);

				psi->Push((payload + pointer + 1), size - pointer - 1);

				return psi;
			}

			std::cout << "PMT section failed" << std::endl;

			return NULL;
		}

		int getAudioPID()
		{
			return audioPID;
		}

		int getVideoPID()
		{
			return videoPID;
		}

		void SetData(jmpeg::ProgramMapSection *s)
		{
			pmtSection = s;
			ParsePmt();
		}
};

FileMediaBank::FileMediaBank():
	MediaBank()
{
	_file = NULL;
	
	_read_index = 0;
	_pass_index = 0;
	_flag = true;
	_packet_index = 0;
	_packet_max = 0;

	_buffer = new jthread::IndexedBuffer(4096, 188*7);
	_packet = new char[188*7];
}

FileMediaBank::~FileMediaBank()
{
}

void FileMediaBank::SetLocator(Locator *locator)
{
	jthread::AutoLock lock(&_mutex);

	if (locator == NULL) {
		return;
	}

	try {
		_flag = false;

		if (IsRunning() == true) {
			WaitThread();
		}

		if (_file != NULL) {

			_file->Close();

			delete _file;
			_file = NULL;
		}

		_file = new jio::FileInputStream(locator->GetPath());
			
		_flag = true;

		Start();
	} catch (...) {
		_file = NULL;
	}
}

long long FileMediaBank::Available()
{
	jthread::AutoLock lock(&_mutex);

	if (_file != NULL) {
		return _file->Available();
	}

	return 0LL;
}

long long FileMediaBank::GetCapacity()
{
	jthread::AutoLock lock(&_mutex);

	if (_file != NULL) {
		return _file->GetSize();
	}

	return 0LL;
}

void FileMediaBank::Open()
{
}

void FileMediaBank::Reset()
{
	jthread::AutoLock lock(&_mutex);

	if (_file != NULL) {
		return _file->Reset();
	}
	
	_read_index = 0;
	_pass_index = 0;
	_flag = true;
	_packet_index = 0;
	_packet_max = 0;
	
	_buffer->Reset();
}

int FileMediaBank::AddData(char *data, int size)
{
	return _buffer->Write((uint8_t *)data, size);
}

int FileMediaBank::GetData(char *data, int size)
{
	/*
	jthread::AutoLock lock(&_mutex);

	if (_file != NULL) {
		int r = _file->Read(data, size);

		if (r <= 0) {
			_file->Reset();
			
			r = _file->Read(data, size);
		}

		return r;
	}

	return -1;
	*/

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

void FileMediaBank::Run() 
{
	return;

	/*
	int r,
			size = 7*188;
	char receive[size];

	do {
		r = -1;

		if (_file != NULL) {
			int r = _file->Read(receive, size);
		}
		
		if (r <= 0) {
			_file->Reset();

			r = _file->Read(receive, size);
		}

		addData(receive, r);

		usleep(100000);
	} while (_flag == true);
	*/

	unsigned pids [8191],
					 videoPackets = 0;

	for (int i = 0; i < 8191; i++) {
		pids [i] = 0;
	}

	const int MAX = 188;

	uint8_t *buffer = new uint8_t [MAX],
					*cursor = buffer;
	jmpeg::ProgramAssociationSection *pat = 0;
	jmpeg::ProgramMapSection *pmt = 0;
	PatInformation patInfo;
	uint64_t pcr = 0,
					 ipcr = 0,
					 factor = 0;
	int pid = -1,
			*pmtPids = 0,
			programCount = 0,
			catPid = -1,
			pmtLength = 0,
			videoPID = -1,
			audioPID = -1;

	PmtInformation *pmtInfo = new PmtInformation();
	// FDFile *f = new FDFile(fileName.c_str());
	unsigned payloadLength;
	int i;
	uint8_t * payload = new uint8_t[188];

	struct timeval t0, tf;
	gettimeofday(&t0, 0);
	uint64_t atual = 0,
					 primeiro = 0,
					 pcrExt = 0;
	int r,
			packetNo = 0;

	running = true;
	stop = false;

	while (!stop) {
		/*
		while (paused) {
			primeiro = 0;

			jthread::Thread::Sleep(100);
		}
		*/

		if ((r = _file->Read((char *)cursor, MAX)) < 188) {
			if (loop) {
				primeiro = 0;

				_file->Reset();
			} else {
				break;
			}
		}

		pid = jmpeg::TransportStreamPacket::GetProgramID(buffer);

		pids [pid]++;

		uint64_t tosleep;
		struct timeval t1, 
									 t2;

		if ((pid >= 0x0000 && pid <= 0x0001) || (pid >= 0x0010 && pid <= 0x1ffe)) {
			if (jmpeg::TransportStreamPacket::HasAdaptationField(buffer)) {
				unsigned plength = jmpeg::TransportStreamPacket::GetAdaptationFieldLength(buffer);
				// Tem adaptation field aqui. Tamanho = plength
				if (plength > 0) {
					jmpeg::AdaptationField *field = jmpeg::TransportStreamPacket::GetAdaptationField(buffer);

					if (field->GetPCRFlag()) {
						if (primeiro == 0) {
							// Primeiro PCR a passar por aqui
							if (pidPcr == 0) {
								pidPcr = pid;
							}

							pcr = field->GetPCRBase();
							pcrExt = field->GetPCRExtension();

							pcr = (pcr * 300 + pcrExt) / 300;
							gettimeofday(&t1, 0);
							primeiro = timeval2int(&t1);
						} else if (pid == pidPcr) {
							if (field->GetLength() > 0) {
								ipcr = field->GetPCRBase();
								pcrExt = field->GetPCRExtension();
								ipcr = (ipcr * 300LL + pcrExt) / 300LL;

								if (ipcr >= pcr) {
									gettimeofday(&t2, 0);
									atual = timeval2int(&t2);
									factor = (uint64_t)((double)(ipcr-pcr)*CLOCK_2_TIME_CONST);

									if (atual - primeiro < factor) {
										tosleep = factor - (atual - primeiro);
										usleep(tosleep);
									}
								} else {
									std::cout << "PCR restarted" << std::endl;

									primeiro = 0;
								}
							}
						}
					}
				}
			}
		}

		if (pid == 8191) {
			// NULL packet
		} else if (pid == 0 && pmtPids == 0) {
			std::cout << "Getting PAT information" << std::endl;
			pat = PatInformation::GetPatSection(buffer);
			if (pat == 0)
				continue;
			patInfo.SetPatSection(pat);
			programCount = patInfo.GetProgramCount();
			if (patInfo.HasCAT()) {
				catPid = patInfo.GetCatPid();
				std::cout << "CAT pid::[" << catPid << "]" << std::endl;
			}
			if (pmtPids == 0) {
				pmtPids = patInfo.GetPmtPIDs();
				pmtLength = patInfo.GetProgramCount();
			}
		} else if (pid == pmtInfo->getVideoPID()) {
			jmpeg::TransportStreamPacket::GetPayload(buffer, payload, &payloadLength);

			videoPackets++;
		} else if (pmtPids != 0) {
			for (i = 0; i < pmtLength; i++) {
				if (pid == pmtPids [i] && pmt == 0) {

					// TODO:: tem que mexer aqui
					if (jmpeg::TransportStreamPacket::GetPayloadUnitStartIndicator(buffer) == 1) {
						pmt = PmtInformation::GetPmtSection(buffer);
						pmtInfo->SetData(pmt);
						videoPID = pmtInfo->getVideoPID();
						audioPID = pmtInfo->getAudioPID();
					}
				}
			}
		}

		++packetNo;

		if (AddData((char *)buffer, 188) < 0) {
			std::cout << "Error writing output" << std::endl;

			break;
		}

		if ((packetNo % 6) == 0) {
			//pipe->Write((char*)nullPacket, 188);
		}
	}

	gettimeofday(&tf, 0);
	struct timeval tInterval;
	timersub(&tf, &t0, &tInterval);

	/*
	int hour = (int)tInterval.tv_sec / 3600;
	int min = (int)tInterval.tv_sec / 60, sec = (int) tInterval.tv_sec % 60;

	std::cout << std::dec << "Elapsed time: " << std::setfill('0') << std::setw(2) << hour << ':' << std::setw(2) << min << ':' << std::setw(2) << sec << std::endl;
	*/

	running = false;

	if(buffer)
		delete buffer;
}

}
