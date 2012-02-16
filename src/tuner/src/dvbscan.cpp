/*
 * DVBScan.cpp
 *
 *  Created on: 27/11/2009
 *      Author: marcovas
 */

#include "dvbscan.h"

#include <sstream>
#include <iostream>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>

#include <string>
#include <iomanip>

Service::Service() :
   pmtPID(0), type(8), scrambled(1) {
   videoPID = 0;
   audioPIDs.push_back(0);
}

int Service::getTransportStreamID() {
   return transportStreamID;
}
void Service::setTransportStreamID(int id) {
   transportStreamID = id;
}

int Service::getServiceID() {
   return serviceID;
}

void Service::setServiceID(int id) {
   serviceID = id;
}
std::string Service::getProviderName() {
   return providerName;
}
void Service::setProviderName(std::string n) {
   providerName = n;
}
std::string Service::getServiceName() {
   return serviceName;
}

void Service::setServiceName(std::string n) {
   serviceName = n;
}

uint16_t Service::getpmtPID() {
   return pmtPID;
}

void Service::setPmtPID(uint16_t pid) {
   pmtPID = pid;
}

uint16_t Service::getPcrPID() {
   return pcrPID;
}

void Service::setPcrPID(uint16_t pid) {
   pcrPID = pid;
}

std::vector<uint16_t> Service::getAudioPIDs() {
   return audioPIDs;
}

void Service::addAudioPID(uint16_t pid) {
   audioPIDs.push_back(pid);
}

void Service::removeAudioPID(uint16_t pid) {
   std::vector<uint16_t>::iterator it;
   int pos = 0;
   for (it = audioPIDs.begin(); it < audioPIDs.end(); it++) {
      pos++;
      if (*it == pid) {
         audioPIDs.erase(audioPIDs.begin() + pos);
      }
   }
}

uint16_t Service::getVideoPID() {
   return videoPID;
}

void Service::setVideoPID(uint16_t pid) {
   videoPID = pid;
}

int Service::getType() {
   return type;
}

void Service::setType(int t) {
   type = t;
}

bool Service::hasPrivateData() {
   return priv;
}

void Service::setPrivateData(bool p) {
   priv = p;
}

Service::~Service() {
   // TODO Auto-generated destructor stub
}

SBTVDNetwork::SBTVDNetwork(unsigned f) :
   networkName(""), networkID(0), frequency(f) {
}

int SBTVDNetwork::getNetworkID() {
   return networkID;
}

unsigned SBTVDNetwork::getFrequency() {
   return frequency;
}

void SBTVDNetwork::setFrequency(unsigned f) {
   frequency = f;
}

void SBTVDNetwork::setNetworkID(int id) {
   networkID = id;
}

void SBTVDNetwork::setChannelNumber(unsigned short n) {
   channelNumber = n;
}

unsigned short SBTVDNetwork::getChannelNumber() {
   return channelNumber;
}

std::string SBTVDNetwork::getNetworkName() {
   return networkName;
}

void SBTVDNetwork::setNetworkName(std::string n) {
   networkName = n;
}

void SBTVDNetwork::addService(Service &s) {
   std::vector<Service>::iterator it;
   bool found = false;
   int pos = 0;
   for (it = services.begin(); it < services.end(); it++, pos++) {
      Service sv = *it;
      if (sv.getServiceID() == s.getServiceID()) {
         found = true;
         break;
      }
   }
   if (!found)
      services.push_back(s);
   else {
      services[pos] = s;
   }
}
void SBTVDNetwork::removeService(Service&) {
   // TODO
}
std::vector<Service> SBTVDNetwork::getServices() {
   return services;
}

SBTVDNetwork::~SBTVDNetwork() {
   // TODO Auto-generated destructor stub
}

void dumpNetwork(SBTVDNetwork *network) {
   std::vector<Service> services = network->getServices();
   std::cout << "Network: " << network->getNetworkName() << " (ID = 0x" << std::hex
         << network->getNetworkID() << ')' << std::dec << std::endl << "Services:"
         << std::endl;
   std::vector<Service>::iterator it;
   Service s;
   for (it = services.begin(); it < services.end(); it++) {
      s = *it;
      std::cout << '\t' << s.getServiceName() <<
      '\t' << s.getServiceID() << std::endl;
   }
}

DVBScan::DVBScan() :
   adapter(0), frontend(0), demux(0), frontendFD(0), demuxFD(0), network(0) {

}

void DVBScan::Run() {
   notifyStart();
   ScanNetwork();
   notifyEnd();
}

void DVBScan::StartScan() {
   std::stringstream ss;
   ss << "/dev/dvb/adapter" << adapter;
   std::string adapterName = ss.str();
   ss.str("");
   ss << adapterName << "/frontend" << frontend;
   std::string frontendName = ss.str();
   ss.str("");
   ss << frontendName << "/demux" << demux;
   std::string demuxName = ss.str();
   ss.str("");
   frontendFD = open(frontendName.c_str(), O_RDWR);
   if (frontendFD < 0) {
      std::cerr << "Error opening " << frontendName << std::endl;
      perror("StartScan");
      notifyError();
   } else
      Start();
}

DVBScan::~DVBScan() {
   if (frontendFD > 0)
      close(frontendFD);
   std::vector<transponder*>::iterator it;
   for (it = transponders.begin(); it < transponders.end(); it++) {
      transponder *t = *it;
      delete t;
   }
   //close(demuxFD);
}

void DVBScan::SetAdapter(unsigned short a) {
   adapter = a;
}

void DVBScan::SetDemux(unsigned short d) {
   demux = d;
}

void DVBScan::SetFrontEnd(unsigned short f) {
   frontend = f;
}

unsigned short DVBScan::GetFrontEnd() const {
   return frontend;
}

unsigned short DVBScan::GetAdapter() const {
   return adapter;
}

unsigned short DVBScan::GetDemux() const {
   return demux;
}

void DVBScan::AddScanListener(ScanListener& l) {
   listeners.push_back(&l);
}

void DVBScan::RemoveScanListener(ScanListener& l) {
   for (unsigned i = 0; i < listeners.size(); i++) {
      if (listeners[i] == &l) {
         listeners.erase(listeners.begin() + i);
         return;
      }
   }
}

void DVBScan::ScanNetwork() {
   if (!TuneInitial()) {
      std::cerr << "TuneInitial error" << std::endl;
      return;
   }
   ScanTransponders();
}

bool DVBScan::GetBit(uint8_t *num, int bit) {
   return (((num[bit / 8] >> (bit % 8)) & 1) == 1);
}

void DVBScan::SetBit(uint8_t *num, bool bitValue) {
   int bit = bitValue ? 1 : 0;
   num[bit / 8] |= 1 << (bit % 8);
}

bool DVBScan::ParseSection(struct section_buf *s) {
   const unsigned char *buf = s->buf;
   int table_id;
   int section_length;
   int table_id_ext;
   int section_version_number;
   int section_number;
   int last_section_number;
   int i;

   table_id = buf[0];

   if (s->table_id != table_id)
      return -1;

   section_length = ((buf[1] & 0x0f) << 8) | buf[2];

   table_id_ext = (buf[3] << 8) | buf[4];
   section_version_number = (buf[5] >> 1) & 0x1f;
   section_number = buf[6];
   last_section_number = buf[7];

   if (s->segmented && s->table_id_ext != -1 && s->table_id_ext != table_id_ext) {
      /* find or allocate actual section_buf matching table_id_ext */
      while (s->next_seg) {
         s = s->next_seg;
         if (s->table_id_ext == table_id_ext)
            break;
      }
      if (s->table_id_ext != table_id_ext) {
         if (s->next_seg == 0) {
            std::cerr << "s->next_seg == NULL" << std::endl;
            throw ;
         }
         s->next_seg = new (struct section_buf);
         s->next_seg->segmented = s->segmented;
         s->next_seg->run_once = s->run_once;
         s->next_seg->timeout = s->timeout;
         s = s->next_seg;
         s->table_id = table_id;
         s->table_id_ext = table_id_ext;
         s->section_version_number = section_version_number;
      }
   }

   if (s->section_version_number != section_version_number ||
         s->table_id_ext != table_id_ext) {
      struct section_buf *next_seg = s->next_seg;

      if (s->section_version_number != -1 && s->table_id_ext != -1)
      std::cout << "section version_number or table_id_ext changed " <<
      s->section_version_number << "->" << section_version_number <<
      '/' << std::hex << "0x" << s->table_id << "->" << s->table_id_ext << std::endl;
      s->table_id_ext = table_id_ext;
      s->section_version_number = section_version_number;
      s->sectionfilter_done = 0;
      memset (s->section_done, 0, sizeof(s->section_done));
      s->next_seg = next_seg;
   }

   buf += 8; /* past generic table header */
   section_length -= 5 + 4; /* header + crc */
   if (section_length < 0) {
      std::cerr << "truncated section (PID 0x" << std::hex << s->pid << "lenght " << std::dec
      << (section_length + 9) << ")";
      return 0;
   }

   if (!GetBit(s->section_done, section_number)) {
      SetBit (s->section_done, section_number);

      std::cout << "pid 0x" << std::hex << s->pid << " tid 0x" << table_id <<
      " table_id_ext 0x" << table_id_ext << std::dec << std::endl;
      //"%i/%i (version %i)\n",
      //s->pid, table_id, table_id_ext, section_number,
      //last_section_number, section_version_number);

      switch (table_id) {
         case 0x00:
         //std::cout << "PAT" << std::endl;
         //parse_pat (buf, section_length, table_id_ext);
         //ParsePat(buf, section_length, table_id_ext);
         break;

         case 0x02:
         //std::cout << "PMT 0x" << std::hex << s->pid << " for service 0x" << table_id_ext <<
         //std::dec << std::endl;
         //parse_pmt (buf, section_length, table_id_ext);
         break;

         case 0x41:
         case 0x40:
         //std::cout << "NIT" << std::endl;
         //parse_nit (buf, section_length, table_id_ext);
         ParseNit(buf, section_length, table_id_ext);
         break;

         case 0x42:
         case 0x46:
         //std::cout << "SDT" << std::endl;
         ParseSdt(buf, section_length, table_id_ext);
         break;
         default:
         ;
      };

      for (i = 0; i <= last_section_number; i++)
      if (!GetBit (s->section_done, i))
      break;

      if (i > last_section_number)
      s->sectionfilter_done = 1;
   }

   if (s->segmented) {
      /* always wait for timeout; this is because we don't now how
       * many segments there are
       */
      return 0;
   }
   else if (s->sectionfilter_done)
   return 1;

   return 0;
}

bool DVBScan::ParseNit(const unsigned char *buf, int section_length,
      int network_id) {
   int descriptors_loop_len = ((buf[0] & 0x0f) << 8) | buf[1];

   if (section_length < descriptors_loop_len + 4) {
      //warning("section too short: network_id == 0x%04x, section_length == %i, "
      //   "descriptors_loop_len == %i\n", network_id, section_length,
      //      descriptors_loop_len);
      return false;
   }
   Service s;
   ParseDescriptors(NIT, buf + 2, descriptors_loop_len, s);
   //network->addService(s);
   section_length -= descriptors_loop_len + 4;
   buf += descriptors_loop_len + 4;

   while (section_length > 6) {
      int transport_stream_id = (buf[0] << 8) | buf[1];
      struct transponder *t, tn;

      descriptors_loop_len = ((buf[4] & 0x0f) << 8) | buf[5];

      if (section_length < descriptors_loop_len + 4) {
         //warning("section too short: transport_stream_id == 0x%04x, "
         //   "section_length == %i, descriptors_loop_len == %i\n",
         //      transport_stream_id, section_length, descriptors_loop_len);
         break;
      }

      std::cout << "transport_stream_id 0x" << std::hex << transport_stream_id << std::dec
            << "\n";

      memset(&tn, 0, sizeof(tn));
      //tn.type = -1;
      network->setNetworkID(network_id);
      //tn.network_id = network_id;
      //tn.original_network_id = (buf[2] << 8) | buf[3];
      //tn.transport_stream_id = transport_stream_id;

      ParseDescriptors(NIT, buf + 6, descriptors_loop_len, s);
      //network->addService(s);
      /*if (tn.type == fe_info.type) {
       // only add if develivery_descriptor matches FE type
       t = find_transponder(tn.param.frequency);
       if (!t)
       t = alloc_transponder(tn.param.frequency);
       copy_transponder(t, &tn);
       }*/

      section_length -= descriptors_loop_len + 6;
      buf += descriptors_loop_len + 6;
   }

	 return false;
}

bool DVBScan::ParsePat(const unsigned char *buf, int sectionLength,
      int transportStreamID) {
   (void) transportStreamID;
   while (sectionLength > 0) {
      struct service s;
      section_buf *filter;
      Service srv;
      int service_id = (buf[0] << 8) | buf[1];

      if (service_id == 0) {
         /* nit pid entry */
         buf += 4;
         sectionLength -= 4;
         continue;
      }

      /* SDT might have been parsed first... */
      std::vector<Service> services = network->getServices();
      std::vector<Service>::iterator it;
      bool found = false;
      for (it = services.begin(); it < services.end(); it++) {
         Service sv = *it;
         if (sv.getServiceID() == service_id) {
            found = true;
            srv = sv;
            break;
         }
      }
      if (!found) {
         // maybe PAT has not yet been parsed...
         srv.setServiceID(service_id);
         network->addService(srv);
         //srv.setTransportStreamID(tran)
         //s = alloc_service(current_tp, service_id);
      }
      srv.setPmtPID(((buf[2] & 0x1f) << 8) | buf[3]);
      //s->pmt_pid = ((buf[2] & 0x1f) << 8) | buf[3];
      if (!srv.hasPrivateData() && srv.getpmtPID()) {
         //s->priv = malloc(sizeof(struct section_buf));
         srv.setPrivateData(true);
         filter = new section_buf;
         CreateFilter(srv.getpmtPID(), srv.getServiceID(), 5, filter);
         filter->run_once = 1;
         filter->segmented = 0;
      }
   };

	 return false;
}

bool DVBScan::ParseSdt(const unsigned char *buffer, int secLen, int tsID) {
   (void) tsID;

   buffer += 3;
   while (secLen >= 5) {
      Service srv;
      int service_id = (buffer[0] << 8) | buffer[1];
      int descriptors_loop_len = ((buffer[3] & 0x0f) << 8) | buffer[4];
      //struct service *s;

      if (secLen < descriptors_loop_len || !descriptors_loop_len) {
         break;
      }

      std::vector<Service> services = network->getServices();
      std::vector<Service>::iterator it;
      bool found = false;
      for (it = services.begin(); it < services.end(); it++) {
         Service sv = *it;
         if (sv.getServiceID() == service_id) {
            found = true;
            srv = sv;
            break;
         }
      }
      if (!found) {
         // maybe PAT has not yet been parsed...
         srv.setServiceID(service_id);
         network->addService(srv);
         //srv.setTransportStreamID(tran)
         //s = alloc_service(current_tp, service_id);
      }
      //s->running = (buffer[3] >> 5) & 0x7;
      //s->scrambled = (buffer[3] >> 4) & 1;

      ParseDescriptors(SDT, buffer + 5, descriptors_loop_len, srv);

      secLen -= descriptors_loop_len + 5;
      buffer += descriptors_loop_len + 5;
   };

	 return false;
}

bool DVBScan::ParseServiceDescriptor(const unsigned char *buf, Service &s) {
   unsigned char len;
   unsigned char *src, *dest;
   s.setType(buf[2]);

   buf += 3;
   len = *buf;
   buf++;
   char providerName [len + 1];
   memcpy(providerName, buf, len);
   providerName[len] = '\0';

   /* remove control characters (FIXME: handle short/long name) */
   /* FIXME: handle character set correctly (e.g. via iconv)
    * c.f. EN 300 468 annex A */
   for (src = dest = (unsigned char *) providerName; *src; src++)
      if (*src >= 0x20 && (*src < 0x80 || *src > 0x9f))
         *dest++ = *src;
   *dest = '\0';
   if (!providerName[0]) {
      /* zap zero length names */
      providerName[0] = 0;
   }

   buf += len;
   len = *buf;
   buf++;

   char serviceName [len + 1];
   memcpy(serviceName, buf, len);
   serviceName[len] = '\0';

   /* remove control characters (FIXME: handle short/long name) */
   /* FIXME: handle character set correctly (e.g. via iconv)
    * c.f. EN 300 468 annex A */
   for (src = dest = (unsigned char *) serviceName; *src; src++)
      if (*src >= 0x20 && (*src < 0x80 || *src > 0x9f))
         *dest++ = *src;
   *dest = '\0';
   if (!serviceName[0]) {
      /* zap zero length names */
      serviceName[0] = 0;
   }
   if (serviceName != 0) {
      s.setServiceName(std::string(serviceName));
   }
   if (providerName != 0) {
      s.setProviderName(std::string(providerName));
   }
   network->addService(s);
   return true;
}

bool DVBScan::ParseDescriptors(table_type t, const unsigned char *buf,
      int descriptorsLoopLen, Service &s) {
   while (descriptorsLoopLen > 0) {
      unsigned char descriptorTag = buf[0];
      unsigned char descriptorLen = buf[1] + 2;

      if (!descriptorLen) {
         std::cerr << "descriptor_tag == 0x" << std::hex << descriptorTag << "len is 0"
               << std::dec << std::endl;
         break;
      }

      switch (descriptorTag) {
      case 0x0a:
         if (t == PMT)
            //parse_iso639_language_descriptor (buf, data);
            break;

      case 0x40:
         if (t == NIT) {
            //parse_network_name_descriptor (buf, data);
            int len = buf[1];
            char name[len+1];
            memccpy(name, buf + 2, len, len);
            name[len] = '\0';
            network->setNetworkName(std::string(name));
            //std::cout << "Network Name:  " << data << std::endl;
         }
         break;

         /*case 0x43:
          if (t == NIT)
          //parse_satellite_delivery_system_descriptor (buf, data);
          break;

          case 0x44:
          if (t == NIT)
          //parse_cable_delivery_system_descriptor (buf, data);
          break;*/

      case 0x48:
         if (t == SDT)
            ParseServiceDescriptor(buf, s);
         //parse_service_descriptor (buf, data);
         break;

      case 0x53:
         if (t == SDT)
            //parse_ca_identifier_descriptor (buf, data);
            break;

      case 0x5a:
         if (t == NIT)
            //parse_terrestrial_delivery_system_descriptor (buf, data);
            break;

      case 0x62:
         if (t == NIT)
            //parse_frequency_list_descriptor (buf, data);
            break;

         //default:
         // std::cerr << "skip descriptor 0x" << std::hex << descriptorTag << std::dec << std::endl;
      };

      buf += descriptorLen;
      descriptorsLoopLen -= descriptorLen;
   }
   return true;
}

void DVBScan::StopFilter(struct section_buf *s) {
   ioctl(s->fd, DMX_STOP);
   close(s->fd);
}

bool DVBScan::StartFilter(struct section_buf *s) {
   struct dmx_sct_filter_params f;
   if ((s->fd = open(s->dmx_devname, O_RDWR | O_NONBLOCK)) < 0)
      throw ;

      //std::cout << "start filter pid 0x" << std::hex << s->pid <<
      //" table_id 0x" << s->table_id << std::dec << std::endl;

      memset(&f, 0, sizeof(f));

      f.pid = (uint16_t) s->pid;

      if (s->table_id < 0x100 && s->table_id > 0) {
         f.filter.filter[0] = (uint8_t) s->table_id;
         f.filter.mask[0] = 0xff;
      }
      if (s->table_id_ext < 0x10000 && s->table_id_ext > 0) {
         f.filter.filter[1] = (uint8_t) ((s->table_id_ext >> 8) & 0xff);
         f.filter.filter[2] = (uint8_t) (s->table_id_ext & 0xff);
         f.filter.mask[1] = 0xff;
         f.filter.mask[2] = 0xff;
      }

      f.timeout = 0;
      f.flags = DMX_IMMEDIATE_START | DMX_CHECK_CRC;

      if (ioctl(s->fd, DMX_SET_FILTER, &f) == -1) {
         perror ("ioctl DMX_SET_FILTER failed");
         StopFilter(s);
         return false;
      }
      s->sectionfilter_done = 0;
      time(&s->start_time);
      return true;
   }

bool DVBScan::ReadSections(struct section_buf *section, unsigned long wait) {
   int section_length, count;

   if (section->sectionfilter_done && !section->segmented)
      return true;

   if (((count = read(section->fd, section->buf, sizeof(section->buf))) < 0)
         && errno == EOVERFLOW)
      count = read(section->fd, section->buf, sizeof(section->buf));
   if (count < 0) {
      if (wait > 10000000L) {
         std::cerr << "read_sections: read error" << std::endl;
         perror("read");
         StopFilter(section);
         return false;
      } else {
         //std::cout << "retrying...(" << wait << ')' << std::endl;
         usleep(wait);
         return ReadSections(section, wait * 5);
      }
   }

   if (count < 4) {
      StopFilter(section);
      return false;
   }

   section_length = ((section->buf[1] & 0x0f) << 8) | section->buf[2];

   if (count != section_length + 3) {
      StopFilter(section);
      return false;
   }

   if (ParseSection(section)) {
      return true;
   }
   return true;
}

bool DVBScan::ParseNetworkNameDescriptor(const unsigned char* buf, Service &s) {
   return true;
}

void DVBScan::CreateFilter(int pid, int tid, int timeout,
      struct section_buf *filter) {
   std::stringstream ss;
   ss << "/dev/dvb/adapter" << adapter << "/demux" << demux;
   memset(filter, 0, sizeof(struct section_buf));
   filter->dmx_devname = ss.str().c_str();
   filter->pid = pid;
   filter->table_id = tid;
   filter->run_once = 1;
   filter->timeout = timeout;
   filter->table_id_ext = tid;
   filter->section_version_number = -1;
   filter->segmented = 0;
   filter->table_id_ext = -1;
   filter->section_version_number = -1;
   filter->to_delete = false;
}

bool DVBScan::ScanTransponder(transponder *t) {
   struct section_buf sections[3];
   if (network != 0)
      delete network;
   network = new SBTVDNetwork(t->param.frequency);
   network->setChannelNumber(t->channel_number);
   filters.clear();
   CreateFilter(0x00, 0x00, 5, &sections[0]); // PAT
   CreateFilter(0x11, 0x42, 5, &sections[1]); // SDT
   CreateFilter(0x10, 0x40, 15, &sections[2]); // NIT
   for (int i = 0; i < 3; i++) {
      StartFilter(&sections[i]);
      filters.push_back(&sections[i]);
   }
   try {
      section_buf *s;
      while (filters.size() > 0) {
         s = filters.at(0);
         ReadSections(s, 1000);
         StopFilter(s);
         filters.pop_front();
         //if (s->to_delete)
            // delete s;
      }
   } catch (...) {
      std::cerr << "Exception here" << std::endl;
      notifyError();
      return false;
   }
   return true;
}

void DVBScan::ScanTransponders() {
   std::vector<transponder*>::iterator it;
   for (it = transponders.begin(); it < transponders.end(); it++) {
      transponder *t = *it;
      notifyScanChannelStart(t->channel_number);
      if (TuneTransponder(t)) {
         ScanTransponder(t);
         notifyNetwork();
      }
      notifyScanChannelEnd(t->channel_number);
   }
}

bool DVBScan::TuneTransponder(transponder *t) {
   fe_status_t s;
   std::cout << "Tunning " << t->param.frequency << std::endl;
   if (ioctl(frontendFD, FE_SET_FRONTEND, &t->param) == -1) {
      std::cerr << "Setting frontend parameters failed" << std::endl;
      ;
      return false;
   }
   for (int i = 0; i < 3; i++) {
      usleep(500000);
      if (ioctl(frontendFD, FE_READ_STATUS, &s) == -1) {
         std::cerr << "FE_READ_STATUS failed" << std::endl;
         return false;
      }
      if (s & FE_HAS_LOCK) {
         t->last_tuning_failed = 0;
         std::cout << "Tunned " << t->param.frequency << std::endl;
         return true;
      }
   }
   return false;
}

bool DVBScan::TuneInitial() {
   int MAX = sizeof(freqs) / sizeof(unsigned) / 2;
   for (int i = 0; i < MAX; i++) {
      transponder *t = new transponder;
      t->param.frequency = freqs[i][0];
      t->channel_number = freqs[i][1];
      t->type = FE_OFDM;
      t->param.inversion = INVERSION_AUTO;
      t->param.u.ofdm.bandwidth = BANDWIDTH_6_MHZ;
      t->param.u.ofdm.code_rate_LP = FEC_3_4;
      t->param.u.ofdm.code_rate_HP = FEC_AUTO;
      t->param.u.ofdm.constellation = QAM_AUTO;
      t->param.u.ofdm.transmission_mode = TRANSMISSION_MODE_AUTO;
      t->param.u.ofdm.guard_interval = GUARD_INTERVAL_AUTO;
      t->param.u.ofdm.hierarchy_information = HIERARCHY_AUTO;
      transponders.push_back(t);
   }
   return true;
}

void DVBScan::notifyStart() {
   std::vector<ScanListener*>::iterator it;
   for (it = listeners.begin(); it < listeners.end(); it++) {
      ScanListener *listener = *it;
      listener->ScanStarted();
   }
}
void DVBScan::notifyEnd() {
   std::vector<ScanListener*>::iterator it;
   for (it = listeners.begin(); it < listeners.end(); it++) {
      ScanListener *listener = *it;
      listener->ScanFinished();
   }
}
void DVBScan::notifyCancel() {
   std::vector<ScanListener*>::iterator it;
   for (it = listeners.begin(); it < listeners.end(); it++) {
      ScanListener *listener = *it;
      listener->ScanCancelled(network);
   }
}
void DVBScan::notifyNetwork() {
   std::vector<ScanListener*>::iterator it;
   for (it = listeners.begin(); it < listeners.end(); it++) {
      ScanListener *listener = *it;
      listener->NetworkReached(network);
   }
}

void DVBScan::notifyError() {
   std::vector<ScanListener*>::iterator it;
   for (it = listeners.begin(); it < listeners.end(); it++) {
      ScanListener *listener = *it;
      listener->ScanError();
   }
}

void DVBScan::notifyScanChannelStart(unsigned short channel) {
   std::vector<ScanListener*>::iterator it;
      for (it = listeners.begin(); it < listeners.end(); it++) {
         ScanListener *listener = *it;
         listener->StartedScanChannel(channel);
      }
}

void DVBScan::notifyScanChannelEnd(unsigned short channel) {
   std::vector<ScanListener*>::iterator it;
      for (it = listeners.begin(); it < listeners.end(); it++) {
         ScanListener *listener = *it;
         listener->FinishedScanChannel(channel);
      }
}
