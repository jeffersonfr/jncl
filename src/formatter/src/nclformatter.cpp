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
#include "nclformatter.h"
#include "nclloader.h"
#include "nclexception.h"
#include "nclimageplayer.h"
#include "nclvideoplayer.h"
#include "nclaudioplayer.h"
#include "nclluaplayer.h"
#include "nclxhtmlplayer.h"
#include "ncltextplayer.h"
#include "nclenviroment.h"
#include "nclhelper.h"
#include "nclselectionevent.h"
#include "nclpresentationevent.h"
#include "nclinput.h"

#include "jautolock.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

namespace jncl {

NCLPlayer *_player = NULL;

NCLFormatter::NCLFormatter(std::string uri, int width, int height):
	NCLEventListener(),
	jthread::Thread()
{
	std::string maindocument = uri,
		basedirectory;

	if (uri.find("/") != std::string::npos) {
		maindocument = uri.substr(uri.rfind("/")+1);
		basedirectory = uri.substr(0, uri.rfind("/")+1);
	}

	_loader = NULL;
	_released = false;

	_enviroment = new NCLEnviroment();

	_enviroment->SetWidth(width);
	_enviroment->SetHeight(height);

	_enviroment->SetMainDocument(maindocument);
	_enviroment->SetBaseDirectory(basedirectory);

	NCLInput::GetInstance()->RegisterEventListener(this);
}

NCLFormatter::NCLFormatter(std::string maindocument, std::string basedirectory, int width, int height)
{
	_loader = NULL;
	_released = false;

	_enviroment = new NCLEnviroment();

	_enviroment->SetWidth(width);
	_enviroment->SetHeight(height);

	_enviroment->SetMainDocument(maindocument);
	_enviroment->SetBaseDirectory(basedirectory);
	
	NCLInput::GetInstance()->RegisterEventListener(this);
}

NCLFormatter::~NCLFormatter()
{
	NCLInput::GetInstance()->RemoveEventListener(this);

	jthread::AutoLock lock(&_mutex);

	for (std::map<std::string, NCLPlayer *>::iterator i=_players.begin(); i!=_players.end(); i++) {
		NCLPlayer *player = i->second;

		delete player;
	}

	if (_loader != NULL) {
		delete _loader;
	}

	// TODO::
	// delete _enviroment
}

NCLPlayer * NCLFormatter::GetPlayer(struct nclcontext_t *ctx, std::string id)
{
	std::map<std::string, NCLPlayer *>::iterator i=_players.find(id);

	if (i != _players.end()) {
		return i->second;
	}

	struct nclmedia_t *t = _loader->GetMedia(ctx, id);

	if (t != NULL) {
		NCLPlayer *player = NULL;

		if (t->refer != "") {
			// TODO:: adicionar um loop, caso seja necessaria a busca de referencias mais profundas
			struct nclmedia_t *refer = _loader->GetMedia(ctx, t->refer);

			if (refer != NULL) {
				t->descriptor = refer->descriptor;
				t->descriptor_id = refer->descriptor_id;
				t->src = refer->src;
				t->newinstance = refer->newinstance;
				t->mime = refer->mime;
			}
		}

		if (t->mime == NCL_MIME_BMP || t->mime == NCL_MIME_PNG || t->mime == NCL_MIME_GIF || t->mime == NCL_MIME_JPG) {
			player = new NCLImagePlayer(_enviroment, t);
		} else if (t->mime == NCL_MIME_MPEG_VIDEO || t->mime == NCL_MIME_MPEG4 || t->mime == NCL_MIME_MOV) {
			player = new NCLVideoPlayer(_enviroment, t);
		} else if (t->mime == NCL_MIME_PLAIN) {
			player = new NCLTextPlayer(_enviroment, t);
		} else if (t->mime == NCL_MIME_LUA) {
			player = new NCLLuaPlayer(_enviroment, t);
		}

		if (player != NULL) {
			bool flag = false;

			for (std::map<std::string, NCLPlayer *>::iterator i=_players.begin(); i!=_players.end(); i++) {
				struct nclregion_t *upper_region = i->second->GetMedia()->descriptor->region,
													 *lower_region = t->descriptor->region;

				if (upper_region->zindex >= lower_region->zindex) {
					_players.insert(i, std::pair<std::string, NCLPlayer *>(id, player));

					jgui::Window *upper = i->second->GetComponent(),
						*lower = player->GetComponent();

					if (upper != NULL && lower != NULL) {
						lower->PutBelow(upper);
					}

					flag = true;

					break;
				}
			}

			std::vector<struct ncllink_t *> *links = player->GetLinks();

			for (std::vector<struct ncllink_t *>::iterator i=ctx->links.begin();i!=ctx->links.end(); i++) {
				for (std::vector<struct nclbind_t *>::iterator j=(*i)->binds.begin(); j!=(*i)->binds.end(); j++) {
					struct nclbind_t *bind = (*j);

					if (strcasecmp(bind->role.c_str(), "onBegin") == 0 ||
							strcasecmp(bind->role.c_str(), "onEnd") == 0 ||
							strcasecmp(bind->role.c_str(), "onAbort") == 0 ||
							strcasecmp(bind->role.c_str(), "onPause") == 0 ||
							strcasecmp(bind->role.c_str(), "onResume") == 0 ||
							strcasecmp(bind->role.c_str(), "onSelection") == 0 ||
							strcasecmp(bind->role.c_str(), "onKeySelection") == 0 ||
							strcasecmp(bind->role.c_str(), "onBeginAttribution") == 0 ||
							strcasecmp(bind->role.c_str(), "onEndAttribution") == 0) {
						if (bind->component == t->id) {
							links->push_back((*i));
						}
					}
				}
			}

			if (flag == false) {
				_players[id] = player;
			}
		}

		return player;
	}

	return NULL;
}

void NCLFormatter::LoadPresentation()
{
	if (_loader != NULL) {
		return;
	}

	try{
		_loader = new NCLLoader(_enviroment);

		_loader->Load();
	} catch (NCLException &e) {
		_loader = NULL;
		
		throw e;
	}
}

void NCLFormatter::StartPresentation()
{
	jthread::AutoLock lock(&_mutex);

	if (_loader == NULL) {
		throw NCLException("Null pointer exeception");
	}

	struct nclcontext_t *ctx = _loader->GetContext("body");
	std::vector<std::vector<struct nclport_t *> *> lports,
		ltemp;

	lports.push_back(&(ctx->ports));

	Start();

	do {
		std::vector<struct nclport_t *> *ports = (*lports.begin());

		for (std::vector<struct nclport_t *>::iterator i=ports->begin(); i!=ports->end(); i++) {
			struct nclport_t *port = (*i);
			struct nclmedia_t *media = _loader->GetMedia(port->context, port->component);

			if (media != NULL) {
				NCLPlayer *player = GetPlayer(ctx, port->component);

				if (player != NULL) {
					player->Play();

					_player = player;

					ActionPerformed(new NCLPresentationEvent(_player, NCL_ACTION_START, NCL_TRANSITION_START));
				}
			} else {
				struct nclcontext_t *ctx1 = _loader->GetContext((*i)->component);

				if (ctx1 != NULL) {
					ctx = ctx1;

					lports.push_back(&ctx1->ports);
				}
			}
		}

		lports.erase(lports.begin());
	} while (lports.size() > 0);
}

void NCLFormatter::StopPresentation()
{
	jthread::AutoLock lock(&_mutex);

}

void NCLFormatter::PausePresentation()
{
	jthread::AutoLock lock(&_mutex);

	if (_loader == NULL) {
		throw NCLException("Null pointer exeception");
	}
}

void NCLFormatter::ResumePresentation()
{
	jthread::AutoLock lock(&_mutex);

	if (_loader == NULL) {
		throw NCLException("Null pointer exeception");
	}
}

void NCLFormatter::Release()
{
	// NCLInput::GetInstance()->Release();
	// NCLTimer::GetInstance()->Release();
}

void NCLFormatter::Wait()
{
	_semaphore.Wait();
}

void NCLFormatter::Notify()
{
	_semaphore.Notify();
}

void NCLFormatter::DoAction(std::vector<struct nclbind_t *> *binds)
{
	for (std::vector<struct nclbind_t *>::iterator k=binds->begin(); k!=binds->end(); k++) {
		struct nclbind_t *bind = (*k);
		struct nclcontext_t *context = NULL;

		if (bind->interface != "") {
			context = _loader->GetContext(bind->interface);
		} else {
			context = _loader->GetContext("body");
		}

		NCLPlayer *p = GetPlayer(context, bind->component);

		if (p != NULL) {
			nclevent_action_t action;
			nclevent_transition_t transition;

			if (strcasecmp(bind->role.c_str(), "start") == 0) {
				action = NCL_ACTION_START;
				transition = NCL_TRANSITION_START;

				p->Play();
			} else if (strcasecmp(bind->role.c_str(), "stop") == 0) {
				action = NCL_ACTION_STOP;
				transition = NCL_TRANSITION_STOP;
				
				p->Stop();
			} else if (strcasecmp(bind->role.c_str(), "pause") == 0) {
				action = NCL_ACTION_PAUSE;
				transition = NCL_TRANSITION_PAUSE;
				
				p->Pause();
			} else if (strcasecmp(bind->role.c_str(), "resume") == 0) {
				action = NCL_ACTION_RESUME;
				transition = NCL_TRANSITION_RESUME;
				
				p->Resume();
			} else if (strcasecmp(bind->role.c_str(), "abort") == 0) {
				action = NCL_ACTION_ABORT;
				transition = NCL_TRANSITION_ABORT;
				
				p->Abort();
			} else {
				// INFO:: evita a execucao de regras invalidas ou naum implementadas
				continue;
			}
					
			ActionPerformed(new NCLPresentationEvent(p, action, transition));
		}
	}
}

std::vector<NCLEvent *> _events;
jthread::Condition _sem;

void NCLFormatter::ActionPerformed(NCLEvent *event)
{
	jthread::AutoLock lock(&_mutex);

	_events.push_back(event);

	_sem.Notify();
}

void NCLFormatter::Run()
{
	do {
		while (_events.size() == 0) {
			_sem.Wait();
		}

		jthread::AutoLock lock(&_mutex);

		NCLEvent *event = *(_events.begin());

		_events.erase(_events.begin());

		if (event->GetType() == NCL_EVENT_SELECTION) {
			NCLSelectionEvent *nclevent = (NCLSelectionEvent *)event;

			if (nclevent->GetSymbol() == NCL_SYMBOL_EXIT) {
				_semaphore.Notify();
			} else {
				std::vector<NCLPlayer *> players;
				int zindex = -1;

				for (std::map<std::string, NCLPlayer *>::iterator i=_players.begin(); i!=_players.end(); i++) {
					if (i->second->IsAlive() == true) {
						players.push_back(i->second);
					
						if (i->second->GetMedia()->descriptor->region->zindex > zindex) {
							// TODO:: usar o zindex ?
							zindex = i->second->GetMedia()->descriptor->region->zindex;
						}
					}
				}

				for (std::vector<NCLPlayer *>::iterator i=players.begin(); i!=players.end(); i++) {
					NCLPlayer *player = (*i);

					for (std::vector<struct ncllink_t *>::iterator i=player->GetLinks()->begin(); i!=player->GetLinks()->end(); i++) {
						struct ncllink_t *link = (*i);

						for (std::vector<struct nclbind_t *>::iterator j=link->binds.begin(); j!=link->binds.end(); j++) {
							struct nclbind_t *bind = (*j);

							if (strcasecmp(bind->role.c_str(), "onSelection") == 0) {
								if (bind->component == player->GetMedia()->id) {
									struct nclparam_t param;

									// WARN:: verificar se o tipo do parametro eh keyCode
									if (bind->params.size() > 0) {
										param = bind->params[0];
									} else if (link->params.size() > 0) {
										param = link->params[0];
									} else {
										break;
									}

									if (NCLHelper::TranslateKey(param.value) == nclevent->GetSymbol()) {
										DoAction(&link->binds);

										goto _exit_01;
									}
								}
							}
						}
					}
				}
_exit_01:
				{
				}
			}
		} else if (event->GetType() == NCL_EVENT_PRESENTATION) {
			NCLPresentationEvent *nclevent = (NCLPresentationEvent *)event;
			NCLPlayer * player = nclevent->GetPlayer();

			for (std::vector<struct ncllink_t *>::iterator i=player->GetLinks()->begin(); i!=player->GetLinks()->end(); i++) {
				struct ncllink_t *link = (*i);

				for (std::vector<struct nclbind_t *>::iterator j=link->binds.begin(); j!=link->binds.end(); j++) {
					struct nclbind_t *bind = (*j);

					if ((nclevent->GetAction() == NCL_ACTION_START && strcasecmp(bind->role.c_str(), "onBegin") == 0) ||
							(nclevent->GetAction() == NCL_ACTION_STOP && strcasecmp(bind->role.c_str(), "onStop") == 0) ||
							(nclevent->GetAction() == NCL_ACTION_PAUSE && strcasecmp(bind->role.c_str(), "onPause") == 0) ||
							(nclevent->GetAction() == NCL_ACTION_RESUME && strcasecmp(bind->role.c_str(), "onResume") == 0) ||
							(nclevent->GetAction() == NCL_ACTION_ABORT && strcasecmp(bind->role.c_str(), "onAbort") == 0)) {
						if (bind->component == player->GetMedia()->id) {
							DoAction(&link->binds);
						}
					}
				}
			}
		}
	} while (true);
}

}

