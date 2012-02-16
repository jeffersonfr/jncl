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
#include "ipmediabank.h"
#include "jinputstream.h"
#include "jdatagramsocket.h"
#include "jsocketoptions.h"
#include "jthread.h"

namespace tuner {

unsigned char null_packet [188]= {
	0x47, 0x1F, 0xff, 0x1F, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

IPMediaBank::IPMediaBank():
	MediaBank(),
	jthread::Thread()
{
	_socket = NULL;
	_read_index = 0;
	_pass_index = 0;
	_flag = false;
	_packet_index = 0;
	_packet_max = 0;
	_input_stream = NULL;

	_buffer = new jthread::IndexedBuffer(1024, 4096);

	_packet = new char[4096];
}

IPMediaBank::~IPMediaBank()
{
}

void IPMediaBank::SetLocator(Locator *locator)
{
	if (locator == NULL) {
		return;
	}

	try {
		if (_socket != NULL) {
			_flag = false;

			if (IsRunning() == true) {
				WaitThread();
			}

			_socket->Close();
			delete _socket;
			_socket = NULL;
		}

		if (locator->GetProtocol() == "udp") {
			_socket = dynamic_cast<jsocket::Connection *>(new jsocket::DatagramSocket(locator->GetPort()));

			{
				jsocket::SocketOptions *o = ((jsocket::DatagramSocket *)_socket)->GetSocketOptions(); 

				o->SetReceiveTimeout(2*500);

				delete o;
			}

			_input_stream = _socket->GetInputStream();

			_flag = true;

			Start();
		}
	} catch (...) {
		_socket = NULL;
	}
}

long long IPMediaBank::Available()
{
	return 0LL;
}

long long IPMediaBank::GetCapacity()
{
	return 0LL;
}

void IPMediaBank::Open()
{
}

void IPMediaBank::Reset()
{
	_buffer->Reset();
}

int IPMediaBank::AddData(char *data, int size)
{
	return _buffer->Write((uint8_t *)data, size);
}

int IPMediaBank::GetData(char *data, int size)
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

void IPMediaBank::Run()
{
	int r;
	char receive[1500];

	do {
		r = -1;

		if (_input_stream != NULL) {
			r = _input_stream->Read(receive, 1500);
		}

		if (r <= 0) {
			AddData((char *)null_packet, 188);

			usleep(100000);
		} else {
			AddData(receive, r);
		}
	} while (_flag == true);
}

};

