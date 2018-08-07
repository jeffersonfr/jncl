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
#ifndef J_NCLHELPER_H
#define J_NCLHELPER_H

#include "nclloader.h"
#include "nclselectionevent.h"

#include "jevent/jkeyevent.h"

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
class NCLHelper{

	private:

	public:
		NCLHelper();
		virtual ~NCLHelper();

		/**
		 * \brief
		 *
		 */
		static double ParseTime(std::string str);
		
		/**
		 * \brief
		 *
		 */
		static double ParseNumber(std::string str, double min, double max);
		
		/**
		 * \brief Processa um numero inteiro com base na assinatura. Casos aceitos:
		 * 1. ("0.5", 128) = 64 // valores entre 0 e 1
		 * 2. ("2.5", 128) = 2 // converte para valor inteiro
		 * 3. ("50%", 128) = 64 // processa a porcentagem tendo como base a referencia
		 * 4. ("50", 128) = 50 // valor inteiro puro
		 */
		static double ParseNumber(std::string str, double reference, double min, double max);
		
		/**
		 * \brief
		 *
		 */
		static uint32_t ParseColor(std::string str);
		
		/**
		 * \brief
		 *
		 */
		static std::string ParseEscape(std::string str);
		
		/**
		 * \brief
		 *
		 */
		static nclevent_symbol_t TranslateKey(std::string key);

		/**
		 * \brief
		 *
		 */
		static nclevent_symbol_t TranslateKey(jevent::jkeyevent_symbol_t key);

		/**
		 * \brief
		 *
		 */
		static nclmimetype_t GetMimeTypeByID(std::string id);
		
		/**
		 * \brief
		 *
		 */
		static nclmimetype_t GetMimeTypeByExtension(std::string file);

};

}

#endif

