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
#include "jfileinputstream.h"
#include "jindexedbuffer.h"
#include "jmutex.h"
#include "jthread.h"

#include <string>

#include <stdint.h>

namespace tuner {

/**
 * \brief
 * 
 * \author Jeff Ferr
 */
class FileMediaBank : public MediaBank, jthread::Thread{

	private:
		jio::FileInputStream *_file;
		jthread::Mutex _mutex;
		
		jthread::IndexedBuffer *_buffer;
		int _read_index;
		int _pass_index;
		int _packet_index;
		int _packet_max;
		char *_packet;
		bool _flag;

		int pidPcr;
		bool ok,
				 paused,
				 loop,
				 stop,
				 running;


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
