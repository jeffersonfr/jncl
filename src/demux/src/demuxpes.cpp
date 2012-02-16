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
#include "demuxpes.h"
#include "packet.h"
#include <iostream>
#include <string.h>
namespace demux
{

DemuxPES::DemuxPES()
{

//	file = fopen("/opt/Streamer/2.GLOBO.2008-11-14.ts","r");

}
 
DemuxPES::~DemuxPES()
{
}

int DemuxPES::readStream(char *buffer, int size)
{
	unsigned char *payload;
	int lido,
			payloadSize,
			pesSize,
			pesOffset=0;
	char packet[188];
	bool newPacket=true;

	while(true){
		memset(packet,0,188);
		lido = readPacket(packet);

		payload = (unsigned char *)Packet::getPayloadCopy(packet);
		payloadSize = Packet::getPayloadLength(packet);

		payload = (unsigned char *)Packet::getPayloadCopy(packet);
		payloadSize = Packet::getPayloadLength(packet);

		if(newPacket){
			if(payload[0]==0x00 && payload[1]==0x00 && payload[2]==0x01){
				newPacket=false;
				memcpy(&pesSize,&payload[4],2);
				memcpy(buffer,payload,payloadSize);
				pesOffset = pesOffset+ payloadSize;
			}
			else continue;
		}else{
			memcpy(buffer+pesOffset,payload,payloadSize);
			pesOffset = pesOffset + payloadSize;
		}
		if(pesOffset>=pesSize){
			return pesSize;
		}
	}
}
 
int DemuxPES::readPacket(char *data){
	int ret;

	while(true){
		ret = source->read(data,188);

		if(filter->isValid(data,188)){
			return ret;
		}
	}
}
 
}
