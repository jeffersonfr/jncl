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
#include "packet.h"

#define TS_PACKET_SIZE  188

namespace demux{

const uint8_t bitMask[8] = { 0x80, 0x40, 0x20, 0x10,0x08, 0x04, 0x02, 0x01 };

uint8_t getByteAt( char * bitStream,int byteIndex)
{
	return bitStream[byteIndex];
}

int getIntAtBits( char * bitStream,int firstBitOffset, int length)
{
	int integer = 0x00000000;
	uint8_t * byte = (uint8_t *) &integer;
	int lastBitOffset = firstBitOffset + length - 1;

	int i, bit;
	for (bit = lastBitOffset, i = 31;
			bit >= firstBitOffset;
			bit--, i--){
		int byteIndex = bit / 8;
		int bitOffset = bit % 8;
		if ((bitStream[byteIndex] & bitMask[bitOffset]) != 0){
			int byteIndex2 = 3 - (i / 8);
			int bitOffset2 = i % 8;
			byte[byteIndex2] = byte[byteIndex2] | bitMask[bitOffset2];
		}
	}

	return integer;
}

uint8_t getByteAtBits( char * bitStream,int firstBitOffset, int length)
{
	uint8_t byte = 0x00;
	int lastBitOffset = firstBitOffset + length - 1;

	int i, bit;
	for (bit = lastBitOffset, i = 7;
			bit >= firstBitOffset;
			bit--, i--){
		int byteIndex = bit / 8;
		int bitOffset = bit % 8;
		if ((bitStream[byteIndex] & bitMask[bitOffset]) != 0){
			byte = byte | bitMask[i];
		}
	}

	return byte;
}

bool getBitAt(char * bitStream, int bitIndex)
{
	int byteIndex = bitIndex / 8;
	int bitOffset = bitIndex % 8;

	if ((bitStream[byteIndex] & bitMask[bitOffset]) != 0) {
		return true;
	}

	return false;
}

Packet::Packet()
{
}

Packet::~Packet()
{
}

char* Packet::getPayloadBeginingAtUnitStarted( char* packet)
{
	int payloadIndex = 4 + (Packet::hasAdaptationField(packet) ? 1 : 0) + Packet::getAdaptationFieldLength(packet);
	int size;
	if ( Packet::hasPointerField(packet) ) {
		size = TS_PACKET_SIZE - payloadIndex - 1 - (Packet::getPointerField(packet));

		/*unsigned char * payload = (unsigned char *)malloc(sizeof(unsigned char)*size);*/

		//DPRINTF("INICIO DO PAYLOAD %d\n",payloadIndex + 1 + getPointerField(packet));
		return (&packet[payloadIndex + 1 + getPointerField(packet)]);

		/*memcpy(payload, &packet[payloadIndex + 1 + getPointerField(packet)], size);*/
		/*return payload;*/
	} else {
		return (NULL);
	}
}

bool  Packet::hasPointerField(char* packet)
{
	return ( getBitAt(packet, 9) );
}

bool  Packet::hasPayload(char* packet)
{
	unsigned int adaptationFieldControl = getByteAtBits(packet, 26, 2);

	if ( (adaptationFieldControl == 0x1) || (adaptationFieldControl == 0x3) ) {
		return true;
	}

	return false;
}

uint8_t Packet::getPayloadLength( char* packet)
{
	if (!Packet::hasPayload(packet)) {
		return 0;
	}

	return (TS_PACKET_SIZE - (Packet::hasAdaptationField(packet) ? 1 : 0) - Packet::getAdaptationFieldLength(packet) - 4);
}

char * Packet::getPayloadCopy(char* packet)
{
	int payloadIndex = 4 + (Packet::hasAdaptationField(packet) ? 1 : 0) + Packet::getAdaptationFieldLength(packet);

	if ( Packet::hasPayload(packet) ) {
		return (&packet[payloadIndex]);
	}

	return (NULL);
}

uint8_t Packet::getPayloadBeginingAtUnitStartedLength( char* packet)
{
	int payloadIndex = 4 + (Packet::hasAdaptationField(packet) ? 1 : 0) + Packet::getAdaptationFieldLength(packet);

	if ( Packet::hasPointerField(packet) ) {
		return (TS_PACKET_SIZE - payloadIndex - 1 - Packet::getPointerField(packet));
	}

	return (0);
}

uint8_t Packet::getPointerField( char* packet)
{
	int payloadIndex = 4 + (Packet::hasAdaptationField(packet) ? 1 : 0) + Packet::getAdaptationFieldLength(packet);

	if ( Packet::hasPointerField(packet) ) {
		return ( getByteAt(packet, payloadIndex) );
	}

	return (0);
}

int Packet::getPid(char* packet)
{
	return ( getIntAtBits(packet, 11, 13) );
}

uint8_t Packet::getAdaptationFieldLength(char* packet)
{
	if ( Packet::hasAdaptationField(packet) ) {
		return ( getByteAt(packet, 4) );
	}

	return (0);
}

int Packet::getContinuityCounter(char* packet)
{
	return ( getIntAtBits(packet, 28, 4) );
}

bool  Packet::hasAdaptationField(char* packet)
{
	int adaptationFieldControl = getByteAtBits(packet, 26, 2);

	if ( (adaptationFieldControl == 0x2) || (adaptationFieldControl == 0x3) ) {
		return true;
	}

	return false;
}

bool  Packet::parser(char* packet)
{
	if (getByteAt(packet, 0) != 0x47) {
		return false;
	}

	if ( getBitAt(packet, 8) ) {
		return false;
	}
	
	return true;
}

}
