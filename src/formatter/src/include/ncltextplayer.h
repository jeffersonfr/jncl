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
#ifndef J_NCLTEXTPLAYER_H
#define J_NCLTEXTPLAYER_H

#include "nclplayer.h"
#include "nclenviroment.h"

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
class NCLTextPlayer : public NCLPlayer{

	private:
		NCLEnviroment *_enviroment;
		jgui::Font *_font;
		std::string _filename,
			_font_family,
			_font_style,
			_font_variant, 
			_font_weight;
		uint32_t _background,
						 _font_size,
						 _font_color;

	public:
		NCLTextPlayer(NCLEnviroment *env, struct nclmedia_t *media);
		virtual ~NCLTextPlayer();

		virtual void Hide();

		virtual void Play();
		virtual void Stop();
		virtual void Pause();
		virtual void Resume();
		virtual void Abort();

		virtual void Release();

		virtual void Render();

};

}

#endif

