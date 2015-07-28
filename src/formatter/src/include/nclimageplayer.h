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
#ifndef J_NCLIMAGEPLAYER_H
#define J_NCLIMAGEPLAYER_H

#include "nclenviroment.h"
#include "nclplayer.h"
#include "jimage.h"

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
class NCLImagePlayer : public NCLPlayer{

	private:
		NCLEnviroment *_enviroment;
		jgui::Image *_image;
		std::string _filename;

	public:
		NCLImagePlayer(NCLEnviroment *env, struct nclmedia_t *media);
		virtual ~NCLImagePlayer();

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

