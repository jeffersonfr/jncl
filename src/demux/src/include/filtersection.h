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
#ifndef FILTERSECTION_H_
#define FILTERSECTION_H_

#include "filter.h"

#include <iostream>
#include <stdint.h>

namespace demux{

class FilterSection : public Filter{

	private:
		int tid,
				tidx;
		bool crc;

	private:
		int getIntAtBits( char * bitStream,int firstBitOffset, int length);
		int takeTidx(char *buffer);
		int takeTid(char *buffer);
		int getByteAt(char * bitStream, int byteIndex);
		bool checkCRC32(char* section);
		int getSectionSize(char* payload);
		uint32_t calculateCrc(unsigned char *begin, unsigned count);

	public:
		FilterSection();
		virtual ~FilterSection();

		void setTableID(int tid);
		int getTableID();
		void setTableIDExtension(int tidx);
		int getTableIDExtension();
		void checkCRC(bool value);

		virtual bool isValid(char *buffer, int size);
};

}

#endif /*FILTERSECTION_H_*/
