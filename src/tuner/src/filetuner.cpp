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
#include "filetuner.h"
#include "filechannel.h"
#include "jfile.h"
#include "jxmlparser.h"
#include "filemediabank.h"

#include <algorithm>

#include <stdlib.h>

namespace tuner {

struct ascending_sort {
	bool operator()(Channel *a, Channel *b)
	{
		if (a->GetName() < b->GetName()) {
			return true;
		}

		return false;
	}
};

FileTuner::FileTuner(std::string dir):
	Tuner()
{
	_source = new FileMediaBank();

	_directory = dir;

	Load();
}

FileTuner::~FileTuner()
{
}

int FileTuner::GetSignalStrength()
{
	return 100;
}
		
void FileTuner::Load()
{
	jcommon::XmlDocument doc("./config/channels.xml");

	if (!doc.LoadFile()) {
		return;
	}

	jcommon::XmlElement *root,
			   *psg;

	// parser servern node
	root = doc.RootElement();

	_index = -1;

	if (root != NULL) {
		if (strcmp(root->Value(), "channels") == 0) {
			std::string last,
				type,
				name,
				path;

			if (root->Attribute("last") != NULL) {
				type = root->Attribute("last");
			}

			psg = root->FirstChildElement("channel");

			do {
				if (psg == NULL || strcmp(psg->Value(), "channel") != 0) {
					break;
				}

				if (psg->Attribute("type") != NULL) {
					type = psg->Attribute("type");
				}

				if (type == "file") {
					if (psg->Attribute("name") != NULL) {
						name = psg->Attribute("name");
					}

					if (psg->Attribute("path") != NULL) {
						path = psg->Attribute("path");
					}

					_channels.push_back(new FileChannel(name, path));

					if (psg->Attribute("last") != NULL) {
						_index = _channels.size()-1;
					}
				}
			} while ((psg = psg->NextSiblingElement("channel")) != NULL);
		}
	}
	
	if (_channels.size() > 0) {
		if (_index < 0) {
			_index = 0;
		}

		SetChannel(_channels[_index]);
	}
}

void FileTuner::Save()
{
	std::ostringstream o;

	o << "<channels>\r\n";

	Channel *current = GetChannel();

	for (std::vector<Channel *>::iterator i=_channels.begin(); i!=_channels.end(); i++) {
		FileChannel *ch = (FileChannel *)(*i);

		o << "\t<channel type=\"file\" name=\"" << ch->GetName() << "\" path=\"" << ch->GetFileName() << "\" ";
	   
		if (ch == current) {
			o << "last=\"true\" ";
		}
			
		o << "/>\r\n";
	}
	
	o << "</channels>\r\n";

	jio::File *file = jio::File::OpenFile("./config/channels.xml", (jio::jfile_flags_t)(jio::JFF_TRUNCATE | jio::JFF_READ_WRITE));

	if (file != NULL) {
		file->Write(o.str().c_str(), o.str().size());

		delete file;
	}
}

void FileTuner::Scan()
{
	for (std::vector<Channel *>::iterator i=_channels.begin(); i!=_channels.end(); i++) {
		Channel *ch = (*i);

		delete ch;
	}

	_channels.clear();

	jio::File *file = jio::File::OpenDirectory(_directory);
	std::vector<std::string> files;
	
	if (file != NULL) {
		file->ListFiles(&files);

		delete file;
		file = NULL;
	}

	for (std::vector<std::string>::iterator i=files.begin(); i!=files.end(); i++) {
		std::string s = (*i);

		if (s.find("channel") == 0) {
			Channel *ch = new FileChannel(s, _directory + "/" + s);

			_channels.push_back(ch);

			DispatchEvent(new TunerEvent(this, ch, NEW_CHANNEL));
		}
	}

	std::sort(_channels.begin(), _channels.end(), ascending_sort());

	if (_channels.size() > 0) {
		_index = 0;
	
		SetChannel(_channels[_index]);
	} else {
		_index = -1;
	}

	Update();
}

void FileTuner::InterruptScan()
{
}

void FileTuner::Update()
{
	Save();
}

};

