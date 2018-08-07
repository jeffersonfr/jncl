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
#ifndef J_NCLFORMATTER_H
#define J_NCLFORMATTER_H

#include "nclloader.h"
#include "nclplayer.h"
#include "nclenviroment.h"

#include <string>
#include <vector>
#include <list>
#include <map>
#include <thread>
#include <condition_variable>

namespace jncl {

/**
 * \brief
 * 
 * \author Jeff Ferr
 */
class NCLFormatter : public NCLEventListener {

	private:
		std::map<std::string, NCLPlayer *> _players;
    std::thread _thread;
    std::condition_variable _semaphore;
    std::mutex _mutex;
		NCLLoader *_loader;
		NCLEnviroment *_enviroment;
		bool _released;

	public:
		NCLFormatter(std::string uri, int width, int height);
		NCLFormatter(std::string maindocument, std::string basedirectory, int width, int height);
		virtual ~NCLFormatter();

		NCLPlayer * GetPlayer(struct nclcontext_t *ctx, std::string id);

		void LoadPresentation();
		void StartPresentation();
		void StopPresentation();
		void PausePresentation();
		void ResumePresentation();

		void Release();
		void Wait();
		void Notify();

		void DoAction(std::vector<struct nclbind_t *> *binds);

		virtual void ActionPerformed(NCLEvent *event);
		
		virtual void Run();

};

}

#endif

