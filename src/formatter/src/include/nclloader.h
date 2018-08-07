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
#ifndef J_NCLLOADER_H
#define J_NCLLOADER_H

#include "nclenviroment.h"

#include "jcommon/jxmlparser.h"

#include <string>
#include <vector>
#include <list>
#include <map>

namespace jncl {

enum nclmimetype_t {
	NCL_MIME_UNKNOWN,
	NCL_MIME_HTML,
	NCL_MIME_WML,
	NCL_MIME_PLAIN,
	NCL_MIME_CSS,
	NCL_MIME_XML,
	NCL_MIME_BMP,
	NCL_MIME_PNG,
	NCL_MIME_GIF,
	NCL_MIME_JPG,
	NCL_MIME_BASIC_AUDIO,
	NCL_MIME_MP3,
	NCL_MIME_MP2,
	NCL_MIME_MPEG_AUDIO,
	NCL_MIME_MPEG_VIDEO,
	NCL_MIME_MPEG4,
	NCL_MIME_MOV,
	NCL_MIME_LUA,
	NCL_MIME_NCLET,
	NCL_MIME_SETTINGS,
	NCL_MIME_TIME
};

struct nclparam_t {
	std::string name,
				 value;
};

struct nclregion_t {
	std::string id,
		title;
	int left,
			top,
			width,
			height,
			zindex;
};

struct ncldescriptor_t {
	struct nclregion_t *region;
	std::string id,
		region_id,
		player,
		focussrc,
		focusselsrc,
		moveleft,
		moveup,
		moveright,
		movedown,
		transin,
		transout;
	uint32_t focusbordercolor,
					 selbordercolor;
	int explicitdur,
			focusindex,
			focusborderwidth,
			focusbordertransparency;
	bool freeze;
	std::vector<nclparam_t> params;
};

struct nclcausalconnector_t {
	std::string id;
	std::vector<struct nclparam_t> params;
	std::vector<struct nclsimplecondition_t *> simpleconditions;
	std::vector<struct nclsimpleaction_t *> simpleactions;
	std::vector<struct nclcompoundcondition_t *> compoundconditions;
	std::vector<struct nclcompoundaction_t *> compoundactions;
};

struct nclsimplecondition_t {
	std::string role,
		delay, 
		event_type,
		key,
		transition,
		min,
		max,
		qualifier;
};

struct nclsimpleaction_t {
	std::string role,
		delay, 
		event_type,
		action_type,
		value,
		repeat,
		repeat_delay,
		duration,
		by,
		min,
		max,
		qualifier;
};

struct nclcompoundcondition_t {
	std::string logical_operator,
		delay;
};

struct nclcompoundaction_t {
	std::string logical_operator,
		delay;
};

struct nclassessmentstatement_t {
};

struct nclattributeassessment_t {
};

struct nclvalueassessment_t {
};

struct nclregionbase_t {
	std::string id,
		alias,
		uri,
		device;
	std::vector<nclregion_t *> regions;
};

struct ncldescriptorbase_t {
	std::string id;
	std::vector<ncldescriptor_t *> descriptors;
};

struct nclconnectorbase_t {
	std::string id,
		alias,
		uri;
	std::vector<struct nclcausalconnector_t *> causalconnectors;
};

struct nclport_t {
	struct nclcontext_t *context;
	std::string id,
		component,
		interface;
};

struct nclarea_t {
	std::string id,
		coords,
		text,
		position,
		first,
		last,
		label;
	int begin,
		end;
};

struct nclmedia_t {
	struct ncldescriptor_t *descriptor;
	std::string id,
		descriptor_id,
		src,
		refer,
		newinstance;
	nclmimetype_t mime;
	std::vector<struct nclparam_t> params;
	std::vector<struct nclarea_t> areas;
};

struct ncllink_t {
	std::string id,
		xconnector_id;
	struct nclcausalconnector_t *xconnector;
	std::vector<struct nclparam_t> params;
	std::vector<struct nclbind_t *> binds;
};

struct nclbind_t {
	std::string id,
		role,
		component,
		interface,
		descriptor;
	std::vector<struct nclparam_t> params;
};

struct nclproperty_t {
};

struct nclcontext_t {
	std::string id,
		refer;
	std::vector<struct nclcontext_t *> contexts;
	std::vector<struct nclport_t *> ports;
	std::vector<struct nclmedia_t *> medias;
	std::vector<struct ncllink_t *> links;
};

class NCLFormatter;

/**
 * \brief
 * 
 * \author Jeff Ferr
 */
class NCLLoader{

	private:
		std::vector<struct nclregionbase_t *> _regions;
		std::vector<struct ncldescriptorbase_t *> _descriptors;
		std::vector<struct nclconnectorbase_t *> _connectors;
		std::vector<struct nclcontext_t *> _contexts;
		NCLEnviroment *_enviroment;
		std::string _filename;
		int _width,
				_height;

		bool InnerParser(std::string file);

		bool InnerParserHead(jcommon::XmlElement *element);
		bool InnerParserBody(jcommon::XmlElement *element);
		
		bool InnerParserRegionBase(std::string alias, std::string uri, jcommon::XmlElement *element);
		bool InnerParserDescriptorBase(std::string alias, std::string uri, jcommon::XmlElement *element);
		bool InnerParserConnectorBase(std::string alias, std::string uri, jcommon::XmlElement *element);
		
		bool InnerParserPort(struct nclcontext_t *parent, jcommon::XmlElement *context);
		bool InnerParserLink(struct nclcontext_t *parent, jcommon::XmlElement *context);
		bool InnerParserMedia(struct nclcontext_t *parent, jcommon::XmlElement *context);

		void InnerRegionCalculate(struct nclregion_t *parent, struct nclregion_t *t, std::string left, std::string top, std::string right, std::string bottom, std::string width, std::string height);

	public:
		NCLLoader(NCLEnviroment *env);
		virtual ~NCLLoader();

		void Load();

		struct nclregionbase_t * GetRegionBase(std::string id);
		struct ncldescriptorbase_t * GetDescriptorBase(std::string id);

		struct nclregion_t * GetRegion(std::string id);
		struct ncldescriptor_t * GetDescriptor(std::string id);
		struct nclcausalconnector_t * GetCausalConnector(std::string id);
		struct nclmedia_t * GetMedia(struct nclcontext_t *ctx, std::string id);
		
		struct nclcontext_t * GetContext(std::string id);

};

}

#endif

