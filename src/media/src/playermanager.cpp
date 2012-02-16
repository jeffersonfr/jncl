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
#include "playermanager.h"
#include "audioplayer.h"
#include "videoplayer.h"
#include "jurl.h"
#include "mediaexception.h"

namespace media {

PlayerManager *PlayerManager::_instance = NULL;

PlayerManager::PlayerManager()
{
	_main = NULL;
}

PlayerManager::~PlayerManager()
{
}

PlayerManager * PlayerManager::GetInstance()
{
	if (_instance == NULL) {
		_instance = new PlayerManager();
	}

	return _instance;
}

Player * PlayerManager::CreatePlayer(tuner::Locator *locator)
{
	Player *player = NULL;

	std::string file = locator->GetPath();

	std::cout << locator->what() << std::endl;
	try {
		if (file.size() > 4) {
			if (strcasecmp(((const char *)file.c_str()+file.size()-3), "mp3") == 0) {
				player = new AudioPlayer(locator);
			} else if ((strcasecmp(((const char *)file.c_str()+file.size()-2), "ts") == 0 ||
						strcasecmp(((const char *)file.c_str()+file.size()-3), "mp4") == 0 ||
						strcasecmp(((const char *)file.c_str()+file.size()-3), "mpg") == 0 ||
						strcasecmp(((const char *)file.c_str()+file.size()-3), "m2t") == 0)) {
				player = new VideoPlayer(locator);
			} else {
				player = new VideoPlayer(locator);
			}
		}
	} catch (media::MediaException &e) {
	}

	return player;
}

}

