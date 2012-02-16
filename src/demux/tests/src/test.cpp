#include "tunermanager.h"
#include "demux.h"
#include "demuxmanager.h"
#include "filterpes.h"

#include <iostream>
#include <cstdlib>

#include <fcntl.h>

using namespace std;

int main(int argc, char *argv[])
{
	tuner::TunerManager *manager = NULL;
	tuner::Tuner *tuner = NULL;
	demux::Demux *demux = NULL;
	
	int // apid = 0,
			vpid = 0;

	manager = tuner::TunerManager::GetInstance();
	tuner = manager->GetTuner();
	
	if ((void *)tuner == NULL) {
		std::cout << "Cannot create a new Tuner to the Locator" << std::endl;

		exit(1);
	}

	std::cout << "Procurando canais ..." << std::endl;

	tuner->Scan();

	/*
	demux = demux::DemuxManager::getInstance()->getDemux(demux::SECTION);
	
	FilterSection *filter = new FilterSectionImpl();

	filter->setPid(16);
	filter->setTableID(0);
	filter->setTableIDExtension(0);
	*/

	demux = demux::DemuxManager::getInstance()->getDemux(demux::PES);

	demux::FilterPes *filter = new demux::FilterPes();

	filter->setPid(vpid);

	demux->setFilter(filter);
	demux->setTimeout(5);
	demux->start();

	char buffer[7*188];

	while(true){
		demux->readStream(buffer, 7*188);

		; // do anything
	}

	return EXIT_SUCCESS;
}

