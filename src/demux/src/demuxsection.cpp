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
#include "demuxsection.h"
#include "section.h"
#include "filtersection.h"
#include "packet.h"

#include <iostream>

#include <sys/time.h>
#include <time.h>
#include <string.h>
#include <stdio.h>

namespace demux{

DemuxSection::DemuxSection():
	Demux()
{
	section = new Section();
}

DemuxSection::~DemuxSection()
{
	if(section!=NULL)
		delete section;
}

void DemuxSection::start()
{
	section->init();

	Demux::start();
}

void DemuxSection::testTimeout()
{
	gettimeofday(&currentTime, NULL);
	timeDiff.tv_sec =  currentTime.tv_sec-initTime.tv_sec;
	timeDiff.tv_usec =  currentTime.tv_usec-initTime.tv_usec;

	if(((timeDiff.tv_sec) > (long) (this->getTimeout() * 2)) ||  (timeDiff.tv_sec < (long)0) ){
		initTime.tv_sec = currentTime.tv_sec-previousTimeDiff.tv_sec;
		initTime.tv_usec = currentTime.tv_usec-previousTimeDiff.tv_usec;
		timeDiff.tv_sec =  currentTime.tv_sec-initTime.tv_sec;
		timeDiff.tv_usec =  currentTime.tv_usec-initTime.tv_usec;
	}

	previousTimeDiff = timeDiff;

	if((timeDiff.tv_sec)>=(long)this->getTimeout()){

		throw TimeoutException("Timeout na leitura de seção");
	}
}

int DemuxSection::readStream(char *buffer,int size)
{
	char packet[188];
	unsigned char *payloadData;
	int payloadSize;
	int firstPart;
	int quantity;
	int remain;
	int result;
	int flagShiftPointerField = 1;

	gettimeofday(&initTime, NULL);

	while (section->getSectionsCount() < 1) {
		this->testTimeout();

		if(this->isStopped()){
			section->removeSection(buffer);

			return -5;
		}

		result = readPacket(packet);

		flagShiftPointerField = 1;

		if (result < 0) {
			return result;
		} else if (Packet::getPayloadLength(packet) < 1) {
			continue;
		}

		payloadData = (unsigned char *)Packet::getPayloadCopy(packet);
		payloadSize = Packet::getPayloadLength(packet);

		firstPart = 1;

		if(this->isStopped()){
			section->removeSection(buffer);

			return -5;
		}

		while (payloadSize > 0) {
			if(this->isStopped()){
				section->removeSection(buffer);

				return -5;
			}

			if ((section->sectionRead == 0) && (firstPart) ){
				if(this->isStopped()){
					section->removeSection(buffer);

					return -5;
				}

				if (Packet::hasPointerField(packet)) {
					payloadData = (unsigned char *)Packet::getPayloadBeginingAtUnitStarted(packet);
					payloadSize = Packet::getPayloadBeginingAtUnitStartedLength(packet);

					firstPart = 0;
					flagShiftPointerField = 0;
				} else {
					break;
				}
			} else if ((section->sectionRead == 0) && (payloadData[0] == 0xFF) ){
				if(this->isStopped()){
					section->removeSection(buffer);

					return -5;
				}

				break;
			} else {
				if(this->isStopped()){
					section->removeSection(buffer);

					return -5;
				}

				remain = (section->sectionSize < 0 ? SECTION_HEADER_SIZE : section->sectionSize) - section->sectionRead;
				quantity = payloadSize > remain ? remain : payloadSize;

				if(Packet::hasPointerField(packet) && flagShiftPointerField == 1){
					flagShiftPointerField = 0;
					payloadData++;
					section->incrementSection(section->sectionRead,(char *)payloadData, quantity);
				} else {
					section->incrementSection(section->sectionRead,(char *)payloadData, quantity);
				}

				// update values
				payloadData += quantity;
				payloadSize -= quantity;
				section->sectionRead += quantity;

				if (section->sectionSize < 0) {
					if (section->sectionRead == SECTION_HEADER_SIZE) {
						section->sectionSize = section->getSectionSize();
					}
				} else {
					if (section->sectionRead == section->sectionSize) {
						if (filter->isValid(section->getData(),section->getSectionSize())) {
							section->addSection(section->getData());
						}

						section->sectionRead = 0;
						section->sectionSize = -1;

						if (firstPart) {
							payloadData = (unsigned char *)Packet::getPayloadBeginingAtUnitStarted(packet);
							payloadSize = Packet::getPayloadBeginingAtUnitStartedLength(packet);

							firstPart = 0;
						}
					}
				}
			}
		}
	}

	int retorno = section->getSectionSize();

	memcpy(buffer,section->getData(),section->getSectionSize());
	section->removeSection(section->getData());
	
	if(this->isStopped())
		return -5;

	section->init();

	return retorno;
}

int DemuxSection::readPacket(char *data)
{
	FilterSection *fs = (FilterSection *)filter;
	int ret;

	while(true){
		this->testTimeout();
		memset(data,0,188);
		ret = source->read(data,188);

		if(ret==-1 || !Packet::parser(data)) {
			continue;
		}

		if(ret<0) {
			throw DemuxException("Problema no tuner\n");
		}

		if(filter->getType() == FILTER_AND){
			return ret;
		}

		if(fs->getPid()== Packet::getPid(data)) {
			return ret;
		}
	}
}

}
