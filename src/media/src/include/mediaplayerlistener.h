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
#ifndef MEDIAPLAYERLISTENER_H
#define MEDIAPLAYERLISTENER_H

#include "mediaplayerevent.h"

#include <string>

#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

namespace media {

class MediaPlayerListener{

	protected:
		/**
		 * \brief 
		 *
		 */
		MediaPlayerListener();

	public:
		/**
		 * \brief 
		 *
		 */
		virtual ~MediaPlayerListener();

		/**
		 * \brief 
		 *
		 */
		virtual void MediaStarted(MediaPlayerEvent *event);

		/**
		 * \brief 
		 *
		 */
		virtual void MediaResumed(MediaPlayerEvent *event);

		/**
		 * \brief 
		 *
		 */
		virtual void MediaPaused(MediaPlayerEvent *event);

		/**
		 * \brief 
		 *
		 */
		virtual void MediaStopped(MediaPlayerEvent *event);

		/**
		 * \brief 
		 *
		 */
		virtual void MediaFinished(MediaPlayerEvent *event);

};

}

#endif
