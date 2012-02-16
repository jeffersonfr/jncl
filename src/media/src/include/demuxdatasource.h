/***************************************************************************
 *   Copyright (C) 2005 by Jeff Ferr                                       *
 *   root@sat                                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef DEMUXDATASOURCE_H
#define DEMUXDATASOURCE_H

#include "datasource.h"
#include "demux.h"

#include <stdint.h>

namespace media {

/**
 * \brief
 * 
 * \author Jeff Ferr
 */
class DemuxDataSource : public DataSource{

	private:
		demux::Demux *_demux;
		int _apid,
			_vpid;

	protected:

	public:
		/**
		* \brief Constritor default.
		*
		*/
		DemuxDataSource(int apid, int vpid);

		/**
		* \brief Destrutor virtual.
		*
		*/
		virtual ~DemuxDataSource();

		/**
		 * \brief
		 *
		 */
		virtual long long Available();

		/**
		 * \brief
		 *
		 */
		virtual long long GetCapacity();

		/**
		 * \brief
		 *
		 */
		virtual void Open();

		/**
		 * \brief
		 *
		 */
		virtual void Reset();

		/**
		 * \brief
		 *
		 */
		virtual int AddData(char *data, int size);

		/**
		 * \brief
		 *
		 */
		virtual int GetData(char *data, int size);

};

};

#endif

