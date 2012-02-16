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
#ifndef SECTION_H_
#define SECTION_H_

#include <stdint.h>

#define MAX_SECTION_SIZE (4096)

#define MAX_SECTIONS_COUNT (20)

#define SECTIONS_BUFFER_SIZE (MAX_SECTIONS_COUNT*MAX_SECTION_SIZE)

#define SECTION_HEADER_SIZE (3)

namespace demux{

class Section{

	private:
		char buffer[SECTIONS_BUFFER_SIZE];
		int sectionsBufferBegin,
				sectionsBufferEnd,
				bufferSectionsCount;

	private:
		void copyData( char *data, int *position, int size, int fromBuffer);
		int getIntAtBits(char *bitStream, int firstBitOffset, int length);
		int getSectionSize(char *payload);

	public:
		Section();
		virtual ~Section();

		void init();
		int getSectionsCount();
		void addSection( char *section);
		void removeSection(char* buffer);
		char * getData();
		int getSectionSize();
		void incrementSection(int offset,char *data,int total);
		int sectionSize;
		int sectionRead;

};

}

#endif /*SECTION_H_*/
