/*
 * DVBTuner.h
 *
 *  Created on: 27/11/2009
 *      Author: marcovas
 */

#ifndef DVBTUNER_H
#define DVBTUNER_H

#include "tuner.h"
#include "jthread.h"
#include "dvbscan.h"

#include <string>

#include <linux/dvb/frontend.h>
#include <linux/dvb/dmx.h>
#include <libdvbapi/dvbdemux.h>

namespace tuner {

int GetFrequencyByName(std::string frequency);
fe_spectral_inversion_t GetInversionByName(std::string inversion);
fe_bandwidth_t GetBandwidthByName(std::string bandwidth);
fe_code_rate_t GetCodeRateHPByName(std::string coderate);
fe_code_rate_t GetCodeRateLPByName(std::string coderate);
fe_modulation_t GetConstellationByName(std::string constellation);
fe_transmit_mode_t GetTransmissionModeByName(std::string transmission_mode);
fe_guard_interval_t GetGuardIntervalByName(std::string guard_interval);
fe_hierarchy_t GetHierarchyInformationByName(std::string hierarchy_information);
std::string GetFrequencyByID(int frequency);
std::string GetInversionByID(fe_spectral_inversion_t inversion);
std::string GetBandwidthByID(fe_bandwidth_t bandwidth);
std::string GetCodeRateHPByID(fe_code_rate_t coderate);
std::string GetCodeRateLPByID(fe_code_rate_t coderate);
std::string GetConstellationByID(fe_modulation_t constellation);
std::string GetTransmissionModeByID(fe_transmit_mode_t transmission_mode);
std::string GetGuardIntervalByID(fe_guard_interval_t guard_interval);
std::string GetHierarchyInformationByID(fe_hierarchy_t hierarchy_information);

class DVBTuner : public Tuner, ScanListener {
	
	private:
		int frontend,
				adapter,
				demux,
				dvr,
				frontendFD;
		std::string configFile,
			channel;
		bool running;

		/**
		 * \brief
		 *
		 */
		void Load();
		
		/**
		 * \brief
		 *
		 */
		void Save();
		
		/**
		 * \brief
		 *
		 */
		virtual void Update();

		/**
		 * \brief
		 *
		 */
		void ScanStarted();
		
		/**
		 * \brief
		 *
		 */
		void ScanFinished();
		
		/**
		 * \brief
		 *
		 */
		void ScanCancelled(SBTVDNetwork *n);
		
		/**
		 * \brief
		 *
		 */
		void NetworkReached(SBTVDNetwork *n);
		
		/**
		 * \brief
		 *
		 */
		void ScanError();
		
		/**
		 * \brief
		 *
		 */
		void StartedScanChannel(unsigned short c);
		
		/**
		 * \brief
		 *
		 */
		void FinishedScanChannel(unsigned short c);

	public:
		/**
		 * \brief
		 *
		 */
		DVBTuner(int frontend, int adapter = 0, int demux = 0, int dvr = 0);
		
		/**
		 * \brief
		 *
		 */
		virtual ~DVBTuner();

		/**
		 * \brief
		 *
		 */
		virtual int GetSignalStrength();
		
		/**
		 * \brief
		 *
		 */
		virtual void Scan();
		
		/**
		 * \brief
		 *
		 */
		virtual void InterruptScan();

};

}

#endif /* DVBZAP_H_ */
