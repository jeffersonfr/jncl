/*
 * DVBScan.h
 *
 *  Created on: 27/11/2009
 *      Author: marcovas
 */

#ifndef DVBSCAN_H
#define DVBSCAN_H

#include "jthread.h"

#include <string>
#include <vector>
#include <deque>

#include <linux/dvb/frontend.h>
#include <linux/dvb/dmx.h>

// Frequency, channel
unsigned const freqs[][2] = {
 {473142857, 14},
 {479142857, 15},
 {485142857, 16},
 {491142857, 17},
 {497142857, 18},
 {503142857, 19},
 {509142857, 20},
 {515142857, 21},
 {521142857, 22},
 {527142857, 23},
 {533142857, 24},
 {539142857, 25},
 {545142857, 26},
 {551142857, 27},
 {557142857, 28},
 {563142857, 29},
 {569142857, 30},
 {575142857, 31},
 {581142857, 32},
 {587142857, 33},
 {593142857, 34},
 {599142857, 35},
 {605142857, 36},
 // Channel 37 not used
 {617142857, 38},
 {623142857, 39},
 {629142857, 40},
 {635142857, 41},
 {641142857, 42},
 {647142857, 43},
 {653142857, 44},
 {659142857, 45},
 {665142857, 46},
 {671142857, 47},
 {677142857, 48},
 {683142857, 49},
 {689142857, 50},
 {695142857, 51},
 {701142857, 52},
 {707142857, 53},
 {713142857, 54},
 {719142857, 55},
 {725142857, 56},
 {731142857, 57},
 {737142857, 58},
 {743142857, 59},
 {749142857, 60},
 {755142857, 61},
 {761142857, 62},
 {767142857, 63},
 {773142857, 64},
 {779142857, 65},
 {785142857, 66},
 {791142857, 67},
 {797142857, 68},
 {803142857, 69}
};

enum table_type {
   PAT, PMT, SDT, NIT
};

const int AUDIO_CHAN_MAX = 32;
const int CA_SYSTEM_ID_MAX = 16;

class Service {
public:
   Service();
   virtual ~Service();
   int getTransportStreamID();
   void setTransportStreamID(int);
   int getServiceID();
   void setServiceID(int);
   std::string getProviderName();
   void setProviderName(std::string);
   std::string getServiceName();
   void setServiceName(std::string);
   uint16_t getpmtPID();
   void setPmtPID(uint16_t);
   uint16_t getPcrPID();
   void setPcrPID(uint16_t);
   std::vector<uint16_t> getAudioPIDs();
   void addAudioPID(uint16_t);
   void removeAudioPID(uint16_t);
   uint16_t getVideoPID();
   void setVideoPID(uint16_t);
   void setType(int);
   int getType();
   bool hasPrivateData();
   void setPrivateData(bool);
private:
   int transportStreamID;
   int serviceID;
   std::string providerName;
   std::string serviceName;
   uint16_t pmtPID;
   uint16_t pcrPID;
   uint16_t videoPID;
   std::vector<uint16_t> audioPIDs;
   std::vector<std::string> audioLang;
   int audio_num;
   uint16_t caID[CA_SYSTEM_ID_MAX];
   int caNUM;
   uint16_t teletextPID;
   uint16_t subtitlingPID;
   uint16_t ac3PID;
   int type;
   unsigned int scrambled;
   bool priv;
   int channelNUM;
};

class SBTVDNetwork {
public:
   SBTVDNetwork(unsigned);
   std::string getNetworkName();
   void setNetworkName(std::string);
   void addService(Service&);
   void removeService(Service&);
   std::vector<Service>getServices();
   void setNetworkID(int);
   int getNetworkID();
   unsigned getFrequency();
   void setFrequency(unsigned);
   virtual ~SBTVDNetwork();
   void setChannelNumber(unsigned short);
   unsigned short getChannelNumber();
private:
   std::string networkName;
   std::vector<Service> services;
   int networkID;
   unsigned frequency;
   unsigned short channelNumber;
};

void dumpNetwork(SBTVDNetwork *network);

enum polarisation {
   POLARISATION_HORIZONTAL = 0x00,
   POLARISATION_VERTICAL = 0x01,
   POLARISATION_CIRCULAR_LEFT = 0x02,
   POLARISATION_CIRCULAR_RIGHT = 0x03
};

enum running_mode {
   RM_NOT_RUNNING = 0x01,
   RM_STARTS_SOON = 0x02,
   RM_PAUSING = 0x03,
   RM_RUNNING = 0x04
};

struct section_buf {
   //struct list_head list;
   const char *dmx_devname;
   unsigned int run_once :1;
   unsigned int segmented :1; /* segmented by table_id_ext */
   int fd;
   int pid;
   int table_id;
   int table_id_ext;
   int section_version_number;
   uint8_t section_done[32];
   int sectionfilter_done;
   unsigned char buf[1024];
   time_t timeout;
   time_t start_time;
   time_t running_time;
   struct section_buf *next_seg; // Segmented tables
   bool to_delete;
};

struct service {
   //struct list_head list;
   int transport_stream_id;
   int service_id;
   char *provider_name;
   char *service_name;
   uint16_t pmt_pid;
   uint16_t pcr_pid;
   uint16_t video_pid;
   uint16_t audio_pid[AUDIO_CHAN_MAX];
   char audio_lang[AUDIO_CHAN_MAX][4];
   int audio_num;
   uint16_t ca_id[CA_SYSTEM_ID_MAX];
   int ca_num;
   uint16_t teletext_pid;
   uint16_t subtitling_pid;
   uint16_t ac3_pid;
   unsigned int type :8;
   unsigned int scrambled :1;
   enum running_mode running;
   void *priv;
   int channel_num;
};

struct transponder {
   //struct list_head list;
   //struct list_head services;
   std::deque<service> services;
   int network_id;
   int original_network_id;
   int transport_stream_id;
   enum fe_type type;
   struct dvb_frontend_parameters param;
   //enum polarisation polarisation;     /* only for DVB-S */
   //int orbital_pos;        /* only for DVB-S */
   //unsigned int we_flag      : 1;   /* West/East Flag - only for DVB-S */
   unsigned int scan_done :1;
   unsigned int last_tuning_failed :1;
   unsigned int other_frequency_flag :1; /* DVB-T */
   unsigned int wrong_frequency :1; /* DVB-T with other_frequency_flag */
   int n_other_f;
   uint32_t *other_f; /* DVB-T freqeuency-list descriptor */
   unsigned short channel_number;
};

class ScanListener {
public:
   virtual void ScanStarted() = 0;
   virtual void ScanFinished() = 0;
   virtual void StartedScanChannel(unsigned short) = 0;
   virtual void FinishedScanChannel(unsigned short) = 0;
   virtual void ScanCancelled(SBTVDNetwork *) = 0;
   virtual void NetworkReached(SBTVDNetwork *) = 0;
   virtual void ScanError() = 0;
};

class DVBScan: public jthread::Thread {
public:
   DVBScan();
   virtual void Run();
   virtual ~DVBScan();
   void SetAdapter(unsigned short);
   void SetFrontEnd(unsigned short);
   void SetDemux(unsigned short);
   unsigned short GetAdapter() const;
   unsigned short GetFrontEnd() const;
   unsigned short GetDemux() const;
   void StartScan();
   void AddScanListener(ScanListener&);
   void RemoveScanListener(ScanListener&);
private:
   unsigned short adapter;
   unsigned short frontend;
   unsigned short demux;
   std::vector<ScanListener*> listeners;
   int frontendFD;
   int demuxFD;
   std::vector<transponder*> transponders;
   void ScanNetwork();
   void ScanTransponders();
   bool ScanTransponder(transponder *t);
   bool TuneInitial();
   bool TuneTransponder(transponder *t);
   void CreateFilter(int, int, int, struct section_buf*);
   bool ReadSections(struct section_buf*, unsigned long wait = 1000);
   bool ParseSection(struct section_buf*);
   bool ParseNit(const unsigned char *, int, int);
   bool ParsePat(const unsigned char *, int, int);
   bool ParseSdt(const unsigned char *, int, int);
   bool ParseDescriptors(table_type, const unsigned char *, int, Service&);
   bool ParseServiceDescriptor(const unsigned char*, Service&);
   bool ParseNetworkNameDescriptor(const unsigned char*, Service&);
   bool GetBit(uint8_t *num, int bit);
   void SetBit(uint8_t *num, bool bitValue);
   bool StartFilter(struct section_buf*);
   void StopFilter(struct section_buf*);
   std::deque<struct section_buf*> filters;
   SBTVDNetwork *network;
   void notifyStart();
   void notifyEnd();
   void notifyCancel();
   void notifyNetwork();
   void notifyError();
   void notifyScanChannelStart(unsigned short);
   void notifyScanChannelEnd(unsigned short);
};

#endif

