/******************************************************************************
 Este arquivo eh parte da implementacao do Projeto OpenGinga

 Direitos Autorais Reservados (c) 2005-2009 UFPB/LAVID

 Este programa eh software livre; voce pode redistribui-lo e/ou modificah-lo sob
 os termos da Licenca Publica Geral GNU versao 2 conforme publicada pela Free
 Software Foundation.

 Este programa eh distribuido na expectativa de que seja util, porem, SEM
 NENHUMA GARANTIA; nem mesmo a garantia implicita de COMERCIABILIDADE OU
 ADEQUACAO A UMA FINALIDADE ESPECIFICA. Consulte a Licenca Publica Geral do
 GNU versao 2 para mais detalhes.

 Voce deve ter recebido uma copia da Licenca Publica Geral do GNU versao 2 junto
 com este programa; se nao, escreva para a Free Software Foundation, Inc., no
 endereco 59 Temple Street, Suite 330, Boston, MA 02111-1307 USA.

 Para maiores informacoes:
 ginga @ lavid.ufpb.br
 http://www.openginga.org
 http://www.ginga.org.br
 http://www.lavid.ufpb.br
 ******************************************************************************
 This file is part of OpenGinga Project

 Copyright: 2005-2009 UFPB/LAVID, All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License version 2 as published by
 the Free Software Foundation.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 PARTICULAR PURPOSE.  See the GNU General Public License version 2 for more
 details.

 You should have received a copy of the GNU General Public License version 2
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA

 For further information contact:
 ginga @ lavid.ufpb.br
 http://www.openginga.org
 http://www.ginga.org.br
 http://www.lavid.ufpb.br
 *******************************************************************************/
#include "demuxmanager.h"
#include "demuxsection.h"
#include "demuxpes.h"
#include "demuxraw.h"
#include "demuxbypass.h"
#include "filterand.h"
#include "filtersection.h"
#include "filterpes.h"
#include "filterraw.h"
#include "filterand.h"
#include "tunermanager.h"

#include <string.h>

namespace demux {

DemuxManager *DemuxManager::_instance = NULL;

DemuxManager::DemuxManager()
{
	buffer = new jshared::IndexedBuffer(4096, 188*7);
	
  _thread = std::thread(&DemuxManager::Run, this);
}

DemuxManager::~DemuxManager() 
{
  _thread.join();
}

DemuxManager * DemuxManager::getInstance() 
{
	if (_instance == NULL) {
		_instance = new DemuxManager();
	}

	return _instance;
}

Demux * DemuxManager::getDemux(demux_t type, int tuner) 
{
	Demux *demux = NULL;
	
	if (type == SECTION){
		demux = new DemuxSection();
	}else if (type == PES){
		demux = new DemuxPES();
	}else if (type == RAW){
		demux = new DemuxRaw();
	}else if (type == BYPASS){
		demux = new DemuxBypass();
	}

	if (demux != NULL) {
		demux->setSource(new Source(this->buffer));
	}

	return demux;
}

void DemuxManager::Run() 
{
	tuner::Tuner *tuner = tuner::TunerManager::GetInstance()->GetTuner();

	tuner::MediaBank *bank = tuner->GetMediaBank();

	uint8_t buf[4096];
	int size,
			max = 7*188;

	while (true) {
		size = bank->GetData((char *)buf, max);

		if (size == max) {
			buffer->Write(buf, max);
		}
	}
}

Filter * DemuxManager::createFilter(filter_t type)
{
	Filter *filter = NULL;

	if(type == FILTER_SECTION){
		filter = (Filter *)new FilterSection();
	}else if(type == FILTER_PES){
		filter = (Filter *)new FilterPes();
	}else if(type == FILTER_RAW){
		filter = (Filter *)new FilterRaw();
	}else if(type == FILTER_AND){
		filter = (Filter *)new FilterAnd();
	}
	
	return filter;
}

}
