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
#include "section.h"
#include <iostream>
#include <string.h>

namespace demux
{
const uint8_t bitMask[8] = { 0x80, 0x40, 0x20, 0x10,0x08, 0x04, 0x02, 0x01 };
Section::Section()
{
	this->init();
}

Section::~Section()
{
}

void Section::copyData(char *data, int *position, int size, int fromBuffer) {
        char *source, *target;
        int rightSpace;
		
        rightSpace = SECTIONS_BUFFER_SIZE - *position;
//        printf("size %d rightSpace %d\n",size,rightSpace);
        if (fromBuffer) {
//        	    printf("frombuffer\n");
                source = this->buffer + *position;
                target = data;
        } else {
//        	    printf("no frombuffer\n");
                source = data;
                target = this->buffer + *position;
        }
		
        if (size > rightSpace) {
//                printf("copyData1 \n");
                memcpy(target, source, rightSpace);
                
                if (fromBuffer) {
//                		printf("copyData1.1\n");
                        target = data + rightSpace;
                        source = this->buffer;
                } else {
//                		printf("copyData1.2 \n");
                        target = this->buffer;
                        source = data + rightSpace;
//                        for(int i=0;i<6;i++)
//							printf("%x|",(unsigned)data[i]);
//						printf("\n");
//						for(int i=0;i<6;i++)
//							printf("%x|",(unsigned)source[i]);
//						printf("\n");
                }
                memcpy(target, source, size - rightSpace);
        
        } else {
//        	printf("copyData2 \n");
        
                memcpy(target, source, size);
        
        }
//        printf("copyData\n");
//		for(int i=0;i<6;i++)
//		printf("%x|",(unsigned)buffer[i]);
//		printf("\n");
        *position = (*position + size) % SECTIONS_BUFFER_SIZE;
        
}
void Section::addSection( char *section) {
        copyData(section, &this->sectionsBufferEnd, getSectionSize(section), 0);
        this->bufferSectionsCount++;

}
void Section::incrementSection(int offset,char *data,int total){
	
	memcpy(this->buffer + offset, data, total);

}

int Section::getSectionSize(){
		return this->getSectionSize(this->buffer);
}
int Section::getSectionSize(char* payload){

      return getIntAtBits(payload, 12, 12)+3;

}

int Section::getIntAtBits(char * bitStream,int firstBitOffset, int length)
   {
      uint32_t integer = 0x00000000;
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

      return (int )integer;
   }



void Section::removeSection( char *section) {
	
        
        this->copyData(section, &this->sectionsBufferBegin, SECTION_HEADER_SIZE, 1);

        copyData(section + SECTION_HEADER_SIZE, &this->sectionsBufferBegin, this->getSectionSize(section) - SECTION_HEADER_SIZE, 1);

        this->bufferSectionsCount--;
}



int Section::getSectionsCount() {
        return this->bufferSectionsCount;
}

char * Section::getData(){

	return buffer;
}

void Section::init(){
	memset(buffer, 0, SECTIONS_BUFFER_SIZE);
	this->sectionsBufferBegin=0;
    this->sectionsBufferEnd=0;
    this->bufferSectionsCount=0;
    this->sectionSize=-1;
    this->sectionRead=0;
}
}
