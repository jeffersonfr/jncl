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
#include "tunermanager.h"
#include "filemediabank.h"
#include "ipmediabank.h"
#include "filetuner.h"
#include "iptuner.h"

namespace tuner {

TunerManager *TunerManager::_instance = NULL;

TunerManager::TunerManager()
{
	_tuner = NULL;
	
	_tuner = new FileTuner("./channels");

	/* TODO:: resolver
	if (ControllerConfigurator::GetInstance()->GetLiteralProperty("system.tuner") == "file") {
		_tuner = new FileTuner("./channels");
	} else if (ControllerConfigurator::GetInstance()->GetLiteralProperty("system.tuner") == "ip") {
		_tuner = new IPTuner(4000, 10);
	}
	*/
}

TunerManager::~TunerManager()
{
}

TunerManager * TunerManager::GetInstance()
{
	if (_instance == NULL) {
		_instance = new TunerManager();
	}

	return _instance;
}

Tuner * TunerManager::GetTuner()
{
	return _tuner;
}

};

