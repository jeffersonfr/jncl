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
#ifndef FILEMIDIABANK_H
#define FILEMIDIABANK_H

#include "mediabank.h"
#include "channel.h"

#include "jio/jfileinputstream.h"
#include "jshared/jindexedbuffer.h"

#include <string>
#include <thread>
#include <mutex>

#include <stdint.h>

namespace tuner {

/**
 * \brief
 * 
 * \author Jeff Ferr
 */
class FileMediaBank : public MediaBank {

	private:
		jio::FileInputStream 
      *_file;
		jshared::IndexedBuffer 
      *_buffer;
    std::thread
      _thread;
    std::mutex 
      _mutex;
		int _read_index,
		  _pass_index,
		  _packet_index,
		  _packet_max,
      pidPcr;
		bool 
			_running;
		char 
      *_packet;

	protected:

	public:
		/**
		* \brief Constritor default.
		*
		*/
		FileMediaBank();

		/**
		* \brief Destrutor virtual.
		*
		*/
		virtual ~FileMediaBank();

		/**
		 * \brief
		 *
		 */
		virtual void SetLocator(Locator *locator);

		/**
		 * \brief
		 *
		 */
		virtual long long Available();

		/**
		 * \brief
		 *
		 */
		virtual long long GetCapacity();

		/**
		 * \brief
		 *
		 */
		virtual void Open();

		/**
		 * \brief
		 *
		 */
		virtual void Reset();

		/**
		 * \brief
		 *
		 */
		virtual int AddData(char *data, int size);

		/**
		 * \brief
		 *
		 */
		virtual int GetData(char *data, int size);

		/**
		 * \brief
		 *
		 */
		virtual void Run();

};

};

#endif
