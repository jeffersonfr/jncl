/*
 * DVBMediaBank.h
 *
 *  Created on: 24/11/2009
 *      Author: marcovas
 */

#ifndef DVBCAPTURE_H_
#define DVBCAPTURE_H_

#include "mediabank.h"
#include "jthread.h"
#include "jindexedbuffer.h"

#include <ostream>

namespace tuner {

class DVBMediaBank : public MediaBank, public jthread::Thread {
	
	private:
		jthread::IndexedBuffer *_buffer;
		std::ostream *out;
		int adapter,
				demux,
				frontend,
				fdFrontend,
				fdDVR,
				fdDemux;
		bool _flag;

		// buffer
		int _read_index,
				_pass_index,
				_packet_index,
				_packet_max;
		char *_packet;
	
	public:
		/**
		 * \brief
		 *
		 */
		DVBMediaBank(int frontend = 0, int adapter = 0, int demux = 0);

		/**
		 * \brief
		 *
		 */
		virtual ~DVBMediaBank();

		/**
		 * \brief
		 *
		 */
		virtual int GetSignalStrength();
		
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

}

#endif /* DVBCAPTURE_H_ */
