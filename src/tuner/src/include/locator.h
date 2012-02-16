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
#ifndef LOCATOR_H
#define LOCATOR_H

#include "jurl.h"

#include <stdint.h>

namespace tuner {

/**
 * \brief
 * 
 * \author Jeff Ferr
 */
class Locator : public jcommon::URL{

	private:

	protected:

	public:
		/**
		* \brief
		*
		*/
		Locator(std::string locator);

		/**
		* \brief Destrutor virtual.
		*
		*/
		virtual ~Locator();

};

};

#endif
