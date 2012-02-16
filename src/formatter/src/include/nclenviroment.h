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
#ifndef J_NCLENVIROMENT_H
#define J_NCLENVIROMENT_H

#include "ncleventlistener.h"

#include <string>
#include <vector>
#include <list>
#include <map>

namespace jncl {

/**
 * \brief
 * 
 * \author Jeff Ferr
 */
class NCLEnviroment{

	private:
		std::map<std::string, std::string> _params;
		std::string _maindocument,
			_basedirectory;
		int _width,
				_height;

	public:
		NCLEnviroment();
		virtual ~NCLEnviroment();

		void Reset();

		void SetMainDocument(std::string str);
		void SetBaseDirectory(std::string str);

		std::string GetMainDocument();
		std::string GetBaseDirectory();

		void SetWidth(int width);
		void SetHeight(int height);

		int GetWidth();
		int GetHeight();

		void SetProperty(std::string key, std::string value);

		std::string GetProperty(std::string key);
		
};

}

#endif

