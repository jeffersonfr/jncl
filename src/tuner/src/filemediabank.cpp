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

	jthread::jbuffer_chunk_t t;
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

	r = _buffer->Read(&t);

	if (r < 0) {
		_buffer->GetIndex(&t);
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
}

}
