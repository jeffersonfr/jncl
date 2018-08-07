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
#include "nclloader.h"
#include "nclhelper.h"
#include "nclformatter.h"
#include "nclexception.h"
#include "nclenviroment.h"

#include "jlogger/jloggerlib.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define ATTRIBUTE(name) 												\
	if (strcasecmp(#name, attr->Name()) == 0) { 	\
		name = attr->Value(); 											\
	} 																						\

namespace jncl {

NCLLoader::NCLLoader(NCLEnviroment *env)
{
	_enviroment = env;

	_filename = _enviroment->GetBaseDirectory() + _enviroment->GetMainDocument();

	_width = _enviroment->GetWidth();
	_height = _enviroment->GetHeight();
}

NCLLoader::~NCLLoader()
{
	for (std::vector<struct nclregionbase_t *>::iterator i=_regions.begin(); i!=_regions.end(); i++) {
		for (std::vector<nclregion_t *>::iterator j=(*i)->regions.begin(); j!=(*i)->regions.end(); j++) {
			delete (*j);
		}

		delete (*i);
	}

	for (std::vector<struct ncldescriptorbase_t *>::iterator i=_descriptors.begin(); i!=_descriptors.end(); i++) {
		for (std::vector<ncldescriptor_t *>::iterator j=(*i)->descriptors.begin(); j!=(*i)->descriptors.end(); j++) {
			delete (*j);
		}

		delete (*i);
	}

	for (std::vector<struct nclconnectorbase_t *>::iterator i=_connectors.begin(); i!=_connectors.end(); i++) {
		delete (*i);
	}

	std::vector<std::vector<struct nclcontext_t *> *> contexts;

	contexts.push_back(&_contexts);

	do {
		std::vector<struct nclcontext_t *> *lcontexts = (*contexts.begin());

		for (std::vector<struct nclcontext_t *>::iterator i=lcontexts->begin(); i!=lcontexts->end(); i++) {
			struct nclcontext_t *t = (*i);

			for (std::vector<struct nclcontext_t *>::iterator j=t->contexts.begin(); j!=t->contexts.end(); j++) {
				struct nclcontext_t *ctx = (*j);

				if (ctx->contexts.size() > 0) {
					contexts.push_back(&(ctx->contexts));
				}

				delete (*j);
			}

			for (std::vector<struct nclport_t *>::iterator j=t->ports.begin(); j!=t->ports.end(); j++) {
				delete (*j);
			}

			for (std::vector<struct nclmedia_t *>::iterator j=t->medias.begin(); j!=t->medias.end(); j++) {
				delete (*j);
			}

			delete t;
		}

		contexts.erase(contexts.begin());
	} while (contexts.size() > 0);
}

void NCLLoader::Load()
{
	if (InnerParser(_filename) == false) {
		throw NCLException("Invalid ncl document");
	}
}

bool NCLLoader::InnerParser(std::string file)
{
	JDEBUG(JINFO, "# begin\n");

	jcommon::XmlDocument doc(file);

	if (!doc.LoadFile()) {
		JDEBUG(JERROR, "file \"%s\" is not a ncl document\n", file.c_str());
		
		return false;
	}

	jcommon::XmlElement *root;

	root = doc.RootElement();

	if (root != NULL) {
		if (strcmp(root->Value(), "ncl") == 0) {
			if (InnerParserHead(root->FirstChildElement("head")) == false) {
				return false;
			}
			
			if (InnerParserBody(root->FirstChildElement("body")) == false) {
				return false;
			}
		}
	}

	JDEBUG(JINFO, "# end\n");

	return true;
}

bool NCLLoader::InnerParserHead(jcommon::XmlElement *element)
{
	if (element == NULL) {
		return false;
	}

	JDEBUG(JINFO, "# begin\n");

	jcommon::XmlElement *psg = element->FirstChildElement();

	if (psg == NULL) {
		JDEBUG(JERROR, "not children in head\n");

		return false;
	}

	do {
		if (strcasecmp(psg->Value(), "regionbase") == 0) {
			InnerParserRegionBase("", "", psg);
		}

		if (strcasecmp(psg->Value(), "descriptorbase") == 0) {
			InnerParserDescriptorBase("", "", psg);
		}

		if (strcasecmp(psg->Value(), "connectorbase") == 0) {
			InnerParserConnectorBase("", "", psg);
		}

		if (strcasecmp(psg->Value(), "compositerule") == 0) {
			JDEBUG(JERROR, "<compositerule> parser not implemented\n");
		}
	} while ((psg = psg->NextSiblingElement()) != NULL);

	JDEBUG(JINFO, "# end\n");

	return true;
}

bool NCLLoader::InnerParserBody(jcommon::XmlElement *element)
{
	JDEBUG(JINFO, "# begin\n");

	if (element == NULL) {
		JDEBUG(JERROR, "ncl document has no contexts\n");

		return false;
	}

	std::vector<jcommon::XmlElement *> contexts;

	contexts.push_back(element);

	// struct nclcontext_t *parent = NULL;

	do {
		jcommon::XmlElement *psg = (*contexts.begin()),
			*context = psg->FirstChildElement();

		if (context == NULL) {
			JDEBUG(JWARN, "context has no children\n");
			
			contexts.erase(contexts.begin());

			continue;
		}

		std::string id,
			refer;
		const char *attr;

		if ((attr = psg->Attribute("id")) != NULL) {
			id = attr;
		} else {
			if (strcasecmp(psg->Value(), "body") == 0) {
				id = "body";
			}
		}

		if ((attr = psg->Attribute("refer")) != NULL) {
			refer = attr;
		}

		JDEBUG(JINFO, "<context id=%s refer=%s>\n", id.c_str(), refer.c_str());

		struct nclcontext_t *t = new struct nclcontext_t;

		t->id = id;
		t->refer = refer;

		do {
			if (strcasecmp(context->Value(), "context") == 0) {
				std::string id;

				jcommon::XmlAttribute *attr = context->FirstAttribute();

				while (attr != NULL) {
					ATTRIBUTE(id);

					attr = attr->Next();
				}

				if (context->HasChildren() == true) {
					contexts.push_back(context);
				}

				// CHANGE:: verificar se eh necessario adicionar filhos ao contexto
				struct nclcontext_t *context = new struct nclcontext_t;

				t->contexts.push_back(context);
			}

			if (strcasecmp(context->Value(), "port") == 0) {
				InnerParserPort(t, context);
			}

			if (strcasecmp(context->Value(), "media") == 0) {
				InnerParserMedia(t, context);
			}

			if (strcasecmp(context->Value(), "link") == 0) {
				InnerParserLink(t, context);
			}
		} while ((context = context->NextSiblingElement()) != NULL);

		_contexts.push_back(t);

		contexts.erase(contexts.begin());
	} while (contexts.size() > 0);

	JDEBUG(JINFO, "# end\n");

	return true;
}

bool NCLLoader::InnerParserPort(struct nclcontext_t *parent, jcommon::XmlElement *context)
{
	if (context == NULL) {
		JDEBUG(JWARN, "link node is null\n");

		return false;
	}

	std::string id,
		component,
		interface;

	jcommon::XmlAttribute *attr = context->FirstAttribute();

	while (attr != NULL) {
		ATTRIBUTE(id);
		ATTRIBUTE(component);
		ATTRIBUTE(interface);

		attr = attr->Next();
	}

	JDEBUG(JINFO, "<port id=%s component=%s interface=%s>\n", id.c_str(), component.c_str(), interface.c_str());

	if (id != "" && component != "") {
		struct nclport_t *port = new struct nclport_t;

		port->context = parent;
		port->id = id;
		port->component = component;
		port->interface = interface;

		parent->ports.push_back(port);
	}

	return true;
}

bool NCLLoader::InnerParserLink(struct nclcontext_t *parent, jcommon::XmlElement *context)
{
	if (context == NULL) {
		JDEBUG(JWARN, "link node is null\n");

		return false;
	}

	if (context->HasChildren() == false) {
		JDEBUG(JERROR, "link has no children\n");

		return false;
	}

	std::string id,
		xconnector;

	jcommon::XmlAttribute *attr = context->FirstAttribute();

	while (attr != NULL) {
		ATTRIBUTE(id);
		ATTRIBUTE(xconnector);

		attr = attr->Next();
	}

	struct ncllink_t *link = new struct ncllink_t;

	link->id = id;
	link->xconnector_id = xconnector;
	link->xconnector = GetCausalConnector(xconnector);

	JDEBUG(JINFO, "<link id=%s xconnector=%s>\n", id.c_str(), xconnector.c_str());

	jcommon::XmlElement *elos = context->FirstChildElement();

	if (elos == NULL) {
		JDEBUG(JINFO, "link has no children\n");

		return true;
	}

	do {
		if (strcasecmp(elos->Value(), "linkparam") == 0) {
			std::string name,
				value;

			jcommon::XmlAttribute *attr = elos->FirstAttribute();

			while (attr != NULL) {
				ATTRIBUTE(name);
				ATTRIBUTE(value);

				attr = attr->Next();
			}

			JDEBUG(JINFO, "<linkparam id=%s name=%s value=%s>\n", id.c_str(), name.c_str(), value.c_str());

			struct nclparam_t param;

			param.name = name;
			param.value = value;

			link->params.push_back(param);
		}

		if (strcasecmp(elos->Value(), "bind") == 0) {
			std::string id,
				role, 
				component,
				interface, 
				descriptor;

			jcommon::XmlAttribute *attr = elos->FirstAttribute();

			while (attr != NULL) {
				ATTRIBUTE(id);
				ATTRIBUTE(role);
				ATTRIBUTE(component);
				ATTRIBUTE(interface);
				ATTRIBUTE(descriptor);

				attr = attr->Next();
			}

			struct nclbind_t *bind = new struct nclbind_t;

			bind->id = id;
			bind->role = role;
			bind->component = component;
			bind->interface = interface;
			bind->descriptor = descriptor;

			JDEBUG(JINFO, "<bind id=%s role=%s component=%s interface=%s descriptor=%s>\n", 
					id.c_str(), role.c_str(), component.c_str(), interface.c_str(), descriptor.c_str());

			link->binds.push_back(bind);

			if (elos->HasChildren() == false) {
				// JDEBUG(JERROR, "bind has no params\n");

				continue;
			}

			jcommon::XmlElement *lparams = elos->FirstChildElement();

			do {
				if (strcasecmp(lparams->Value(), "bindparam") == 0) {
					std::string name,
						value;

					jcommon::XmlAttribute *attr = lparams->FirstAttribute();

					while (attr != NULL) {
						ATTRIBUTE(name);
						ATTRIBUTE(value);

						attr = attr->Next();
					}

					struct nclparam_t param;

					param.name = name;
					param.value = value;

					JDEBUG(JINFO, "<bindparam name=%s value=%s>\n", name.c_str(), value.c_str());

					bind->params.push_back(param);
				}
			} while ((lparams = lparams->NextSiblingElement()) != NULL);
		}
	} while ((elos = elos->NextSiblingElement()) != NULL);

	parent->links.push_back(link);


	return true;
}

bool NCLLoader::InnerParserMedia(struct nclcontext_t *parent, jcommon::XmlElement *context)
{
	if (context == NULL) {
		JDEBUG(JWARN, "media node is null\n");

		return false;
	}

	std::string id,
		type,
		src,
		descriptor,
		refer,
		newinstance;

	jcommon::XmlAttribute *attr = context->FirstAttribute();

	while (attr != NULL) {
		ATTRIBUTE(id);
		ATTRIBUTE(type);
		ATTRIBUTE(src);
		ATTRIBUTE(descriptor);
		ATTRIBUTE(refer);
		ATTRIBUTE(newinstance);

		attr = attr->Next();
	}

	JDEBUG(JINFO, "<media id=%s type=%s src=%s descriptor=%s refer=%s newinstance=%s>\n", 
			id.c_str(), type.c_str(), src.c_str(), descriptor.c_str(), refer.c_str(), newinstance.c_str());

	if (id != "") {
		struct nclmedia_t *media = new struct nclmedia_t;

		media->id = id;
		media->descriptor_id = descriptor;
		media->src = src;
		media->refer = refer;
		media->newinstance = newinstance;
		media->mime = NCLHelper::GetMimeTypeByID(type);

		if (media->mime == NCL_MIME_UNKNOWN) {
			media->mime = NCLHelper::GetMimeTypeByExtension(media->src);

			if (media->mime == NCL_MIME_UNKNOWN) {
				JDEBUG(JWARN, "unknown mime-type:: [type, source] = [%s, %s]\n", type.c_str(), src.c_str());

				delete media;

				return true;
			}
		}

		media->descriptor = GetDescriptor(descriptor);

		if (media->descriptor == NULL) {
			JDEBUG(JWARN, "descriptor \"%s\" not found\n", descriptor.c_str());

			delete media;

			return true;
		}

		// CHANGE:: verificar se (region != NULL) antes de recuperar outra region (otimizacao)
		media->descriptor->region = GetRegion(media->descriptor->region_id);

		if (media->descriptor->region == NULL) {
			JDEBUG(JWARN, "region \"%s\" not found\n", media->descriptor->region_id.c_str());

			delete media;

			return true;
		} else {
			// INFO:: atualiza o zindex dos componentes naum graficos para -1
			nclmimetype_t mime = media->mime;

			if (mime == NCL_MIME_CSS ||
					mime == NCL_MIME_XML ||
					mime == NCL_MIME_BASIC_AUDIO ||
					mime == NCL_MIME_MP2 ||
					mime == NCL_MIME_MP3 ||
					mime == NCL_MIME_MPEG_AUDIO ||
					mime == NCL_MIME_SETTINGS ||
					mime == NCL_MIME_TIME) {
				media->descriptor->region->zindex = -1;
			}
		}

		if (context->HasChildren() == true) {
			jcommon::XmlElement *property = context->FirstChildElement();

			do {
				if (strcasecmp(property->Value(), "property") == 0) {
					std::string name,
						value;

					jcommon::XmlAttribute *attr = property->FirstAttribute();

					while (attr != NULL) {
						ATTRIBUTE(name);
						ATTRIBUTE(value);

						attr = attr->Next();
					}

					JDEBUG(JINFO, "<property name=%s value=%s>\n", name.c_str(), value.c_str());

					struct nclparam_t param;

					param.name = name;
					param.value = value;

					media->params.push_back(param);
				} else if (strcasecmp(property->Value(), "area") == 0) {
					std::string id,
						coords,
						text,
						position,
						first,
						last,
						label,
						begin,
						end;

					jcommon::XmlAttribute *attr = property->FirstAttribute();

					while (attr != NULL) {
						ATTRIBUTE(id);
						ATTRIBUTE(coords);
						ATTRIBUTE(text);
						ATTRIBUTE(position);
						ATTRIBUTE(first);
						ATTRIBUTE(last);
						ATTRIBUTE(label);
						ATTRIBUTE(begin);
						ATTRIBUTE(end);

						attr = attr->Next();
					}

					JDEBUG(JINFO, "<area id=%s coords=%s text=%s position=%s first=%s last=%s label=%s begin=%s end=%s />\n", 
							id.c_str(), coords.c_str(), text.c_str(), position.c_str(), first.c_str(), last.c_str(), label.c_str(), begin.c_str(), end.c_str());

					struct nclarea_t area;

					area.id = id;
					area.coords = coords;
					area.text = text;
					area.position = position;
					area.first = first;
					area.last = last;
					area.label = label;
					area.begin = atoi(begin.c_str());
					area.end = atoi(end.c_str());

					media->areas.push_back(area);
				}
			} while ((property = property->NextSiblingElement()) != NULL);
		}

		parent->medias.push_back(media);
	}

	return true;
}

bool NCLLoader::InnerParserRegionBase(std::string alias, std::string uri, jcommon::XmlElement *element)
{
	// TODO:: parse assessment

	JDEBUG(JINFO, "# begin\n");

	if (element == NULL) {
		JDEBUG(JWARN, "regionbase node is null\n");

		return false;
	}

	if (element->HasChildren() == false) {
		JDEBUG(JERROR, "regionbase has no children\n");

		return false;
	}

	struct nclregionbase_t *base = new struct nclregionbase_t;

	std::string id,
		device;

	jcommon::XmlAttribute *attr = element->FirstAttribute();

	while (attr != NULL) {
		ATTRIBUTE(id);
		ATTRIBUTE(device);

		attr = attr->Next();
	}

	base->id = id;
	base->alias = alias;
	base->uri = uri;
	base->device = device;
	
	JDEBUG(JINFO, "<regionbase id=%s device=%s>\n", id.c_str(), device.c_str());

	std::vector<jcommon::XmlElement *> regions;
	std::vector<struct nclregion_t *> parents;

	regions.push_back(element);

	struct nclregion_t *parent = NULL;

	do {
		jcommon::XmlElement *region = (*regions.begin())->FirstChildElement();

		if (parents.size() > 0) {
			parent = (*parents.begin());

			parents.erase(parents.begin());
		}

		if (strcasecmp(region->Value(), "importbase") == 0) {
			std::string alias,
				documenturi;

			jcommon::XmlAttribute *attr = region->FirstAttribute();

			while (attr != NULL) {
				ATTRIBUTE(alias);
				ATTRIBUTE(documenturi);

				attr = attr->Next();
			}

			jcommon::XmlDocument doc(_enviroment->GetBaseDirectory() + "/" + documenturi);

			if (!doc.LoadFile()) {
				JDEBUG(JERROR, "file \"%s\" is not a valid connectorbase\n", (_enviroment->GetBaseDirectory() + "/" + documenturi).c_str());

				continue;
			}

			JDEBUG(JINFO, "<importbase alias=%s documenturi=%s>\n", alias.c_str(), documenturi.c_str());

			jcommon::XmlElement *root;

			root = doc.RootElement();

			if (root != NULL && strcmp(root->Value(), "ncl") == 0) {
				root = root->FirstChildElement();

				if (root != NULL && strcmp(root->Value(), "head") == 0) {
					InnerParserConnectorBase(alias, documenturi, root->FirstChildElement());
				}
			}

			continue;
		}

		do {
			if (strcasecmp(region->Value(), "region") == 0) {
				std::string id,
					title,
					left,
					top,
					right,
					bottom,
					width,
					height,
					zindex;

				jcommon::XmlAttribute *attr = region->FirstAttribute();

				while (attr != NULL) {
					ATTRIBUTE(id);
					ATTRIBUTE(title);
					ATTRIBUTE(left);
					ATTRIBUTE(top);
					ATTRIBUTE(right);
					ATTRIBUTE(bottom);
					ATTRIBUTE(width);
					ATTRIBUTE(height);
					ATTRIBUTE(zindex);

					attr = attr->Next();
				}

				if (id != "") {
					struct nclregion_t *t = new struct nclregion_t;

					t->id = id;
					t->title = title;
					t->zindex = NCLHelper::ParseNumber(zindex, 0, 255);

					InnerRegionCalculate(parent, t, left, top, right, bottom, width, height);

					base->regions.push_back(t);

					if (region->HasChildren() == true) {
						parents.push_back(t);
						regions.push_back(region);
					}

					JDEBUG(JINFO, "<region id=%s title=%s left=%d top=%d width=%d height=%d zindex=%d>\n", 
							t->id.c_str(), t->title.c_str(), t->left, t->top, t->width, t->height, t->zindex);
				}
			}
		} while ((region = region->NextSiblingElement()) != NULL);

		regions.erase(regions.begin());
	} while (regions.size() > 0);

	_regions.push_back(base);
	
	JDEBUG(JINFO, "# end\n");

	return true;
}



bool NCLLoader::InnerParserDescriptorBase(std::string alias, std::string uri, jcommon::XmlElement *element)
{
	// TODO:: parse assessment

	JDEBUG(JINFO, "# begin\n");

	if (element == NULL) {
		JDEBUG(JWARN, "descriptorbase node is null\n");

		return false;
	}

	if (element->HasChildren() == false) {
		JDEBUG(JERROR, "descriptorbase has no children\n");

		return false;
	}

	struct ncldescriptorbase_t *base = new struct ncldescriptorbase_t;

	std::string id;

	jcommon::XmlAttribute *attr = element->FirstAttribute();

	while (attr != NULL) {
		ATTRIBUTE(id);

		attr = attr->Next();
	}

	base->id = id;

	JDEBUG(JINFO, "<descriptorbase id=%s>\n", id.c_str());

	jcommon::XmlElement *descriptor = element->FirstChildElement();

	do {
		if (strcasecmp(descriptor->Value(), "importbase") == 0) {
			std::string alias,
				documenturi;

			jcommon::XmlAttribute *attr = descriptor->FirstAttribute();

			while (attr != NULL) {
				ATTRIBUTE(alias);
				ATTRIBUTE(documenturi);

				attr = attr->Next();
			}

			jcommon::XmlDocument doc(_enviroment->GetBaseDirectory() + "/" + documenturi);

			if (!doc.LoadFile()) {
				JDEBUG(JERROR, "file \"%s\" is not a valid connectorbase\n", (_enviroment->GetBaseDirectory() + "/" + documenturi).c_str());

				continue;
			}

			JDEBUG(JINFO, "<importbase alias=%s documenturi=%s>\n", alias.c_str(), documenturi.c_str());

			jcommon::XmlElement *root;

			root = doc.RootElement();

			if (root != NULL && strcmp(root->Value(), "ncl") == 0) {
				root = root->FirstChildElement();

				if (root != NULL && strcmp(root->Value(), "head") == 0) {
					InnerParserConnectorBase(alias, documenturi, root->FirstChildElement());
				}
			}

			continue;
		}

		if (strcasecmp(descriptor->Value(), "descriptor") == 0) {
			std::string id,
				player,
				explicitdur,
				region,
				freeze,
				focusindex,
				focusbordercolor,
				focusborderwidth,
				focusbordertransparency,
				focussrc,
				focusselsrc,
				selbordercolor,
				moveleft,
				moveup,
				moveright,
				movedown,
				transin,
				transout;

			jcommon::XmlAttribute *attr = descriptor->FirstAttribute();

			while (attr != NULL) {
				ATTRIBUTE(id);
				ATTRIBUTE(player);
				ATTRIBUTE(explicitdur);
				ATTRIBUTE(region);
				ATTRIBUTE(freeze);
				ATTRIBUTE(focusindex);
				ATTRIBUTE(focusbordercolor);
				ATTRIBUTE(focusborderwidth);
				ATTRIBUTE(focusbordertransparency);
				ATTRIBUTE(focussrc);
				ATTRIBUTE(focusselsrc);
				ATTRIBUTE(selbordercolor);
				ATTRIBUTE(moveleft);
				ATTRIBUTE(moveup);
				ATTRIBUTE(moveright);
				ATTRIBUTE(movedown);
				ATTRIBUTE(transin);
				ATTRIBUTE(transout);

				attr = attr->Next();
			}

			if (id != "") {
				struct ncldescriptor_t *t = new struct ncldescriptor_t;

				t->id = id;
				t->region = NULL;
				t->region_id = region;
				t->player = player;
				t->focussrc = focussrc;
				t->focusselsrc = focusselsrc;
				t->moveleft = moveleft;
				t->moveup = moveup;
				t->moveright = moveright;
				t->movedown = movedown;
				t->transin = transin;
				t->transout = transout;
				t->focusbordercolor = NCLHelper::ParseColor(focusbordercolor);
				t->selbordercolor = NCLHelper::ParseColor(selbordercolor);
				t->explicitdur = NCLHelper::ParseTime(explicitdur.c_str());
				t->focusindex = atoi(focusindex.c_str());
				t->focusborderwidth = atoi(focusborderwidth.c_str());
				t->focusbordertransparency = NCLHelper::ParseNumber(focusbordertransparency, 255, 0, 255);
				t->freeze = atoi(freeze.c_str());

				JDEBUG(JINFO, "<descriptor id=%s player=%s explicitdur=%s region=%s freeze=%s focusindex=%s focusbordercolor=%s focusborderwidth=%s focusbordertransparency=%s focussrc=%s focusselsrc=%s selbordercolor=%s moveleft=%s moveup=%s moveright=%s movedown=%s transin=%s transout=%s>\n", 
						id.c_str(), player.c_str(), explicitdur.c_str(), region.c_str(), freeze.c_str(), focusindex.c_str(), focusbordercolor.c_str(), focusborderwidth.c_str(), focusbordertransparency.c_str(), focussrc.c_str(), focusselsrc.c_str(), selbordercolor.c_str(), moveleft.c_str(), moveup.c_str(), moveright.c_str(), movedown.c_str(), transin.c_str(), transout.c_str());

				jcommon::XmlElement *param = descriptor->FirstChildElement();

				if (param != NULL) {
					std::string name,
						value;

					do {
						if (strcasecmp(param->Value(), "descriptorparam") == 0) {

							jcommon::XmlAttribute *attr = param->FirstAttribute();

							while (attr != NULL) {
								ATTRIBUTE(name);
								ATTRIBUTE(value);

								attr = attr->Next();
							}

							struct nclparam_t param;

							param.name = name;
							param.value = value;

							t->params.push_back(param);

							JDEBUG(JINFO, "<descriptorparam name=%s value=%s\n", name.c_str(), value.c_str());
						}
					} while ((param = param->NextSiblingElement()) != NULL);
				}

				base->descriptors.push_back(t);
			}
		}
	} while ((descriptor = descriptor->NextSiblingElement()) != NULL);

	_descriptors.push_back(base);

	JDEBUG(JINFO, "# end\n");

	return true;
}

bool NCLLoader::InnerParserConnectorBase(std::string alias, std::string uri, jcommon::XmlElement *element)
{
	// TODO:: parse assessment

	JDEBUG(JINFO, "# begin\n");

	if (element == NULL) {
		JDEBUG(JWARN, "connector node is null\n");

		return false;
	}

	if (element->HasChildren() == false) {
		JDEBUG(JERROR, "connectorbase has no children\n");

		return false;
	}

	struct nclconnectorbase_t *base = new struct nclconnectorbase_t;

	std::string id;

	jcommon::XmlAttribute *attr = element->FirstAttribute();

	while (attr != NULL) {
		ATTRIBUTE(id);

		attr = attr->Next();
	}

	base->id = id;
	base->alias = alias;
	base->uri = uri;

	JDEBUG(JINFO, "<connectorbase id=%s alias=%s>\n", id.c_str(), alias.c_str());

	jcommon::XmlElement *connector = element->FirstChildElement();

	do {
		if (strcasecmp(connector->Value(), "importbase") == 0) {
			std::string alias,
				documenturi;

			jcommon::XmlAttribute *attr = connector->FirstAttribute();

			while (attr != NULL) {
				ATTRIBUTE(alias);
				ATTRIBUTE(documenturi);

				attr = attr->Next();
			}

			jcommon::XmlDocument doc(_enviroment->GetBaseDirectory() + "/" + documenturi);

			if (!doc.LoadFile()) {
				JDEBUG(JERROR, "file \"%s\" is not a valid connectorbase\n", (_enviroment->GetBaseDirectory() + "/" + documenturi).c_str());

				continue;
			}

			JDEBUG(JINFO, "<importbase alias=%s documenturi=%s>\n", alias.c_str(), documenturi.c_str());

			jcommon::XmlElement *root;

			root = doc.RootElement();

			if (root != NULL && strcmp(root->Value(), "ncl") == 0) {
				root = root->FirstChildElement();

				if (root != NULL && strcmp(root->Value(), "head") == 0) {
					InnerParserConnectorBase(alias, documenturi, root->FirstChildElement());
				}
			}

			continue;
		}

		if (strcasecmp(connector->Value(), "causalconnector") == 0) {
			if (element->HasChildren() == false) {
				JDEBUG(JERROR, "connectorbase has no children\n");

				continue;
			}

			std::string id;

			jcommon::XmlAttribute *attr = connector->FirstAttribute();

			while (attr != NULL) {
				ATTRIBUTE(id);

				attr = attr->Next();
			}

			if (id == "") {
				JDEBUG(JERROR, "connectorbase id not defined\n");

				continue;
			}

			JDEBUG(JINFO, "<causalconnector id=%s>\n", id.c_str());

			struct nclcausalconnector_t *causal = new nclcausalconnector_t;

			causal->id = id;

			jcommon::XmlElement *action = connector->FirstChildElement();

			bool has_condition = false,
					 has_action = false;

			do {
				if (strcasecmp(action->Value(), "connectorparam") == 0) {
					std::string name,
						type;

					jcommon::XmlAttribute *attr = connector->FirstAttribute();

					while (attr != NULL) {
						ATTRIBUTE(name);
						ATTRIBUTE(type);

						attr = attr->Next();
					}

					JDEBUG(JINFO, "<connectorparam name=%s value=%s>\n", name.c_str(), type.c_str());

					struct nclparam_t param;

					param.name = name;
					param.value = type;

					causal->params.push_back(param);
				}

				if (has_condition == false && strcasecmp(action->Value(), "simplecondition") == 0) {
					has_condition = true;

					std::string role,
						delay, 
						eventtype,
						key,
						transition,
						min,
						max,
						qualifier;

					jcommon::XmlAttribute *attr = action->FirstAttribute();

					while (attr != NULL) {
						ATTRIBUTE(role);
						ATTRIBUTE(delay);
						ATTRIBUTE(eventtype);
						ATTRIBUTE(key);
						ATTRIBUTE(transition);
						ATTRIBUTE(min);
						ATTRIBUTE(max);
						ATTRIBUTE(qualifier);

						attr = attr->Next();
					}

					if (role == "") {
						has_condition = false;

						JDEBUG(JERROR, "simplecondition role not defined\n");

						continue;
					}

					JDEBUG(JINFO, "<connectorparam role=%s delay=%s eventtype=%s key=%s transition=%s min=%s max=%s qualifier=%s>\n", 
							role.c_str(), delay.c_str(), eventtype.c_str(), key.c_str(), transition.c_str(), min.c_str(), max.c_str(), qualifier.c_str());

					struct nclsimplecondition_t *condition = new struct nclsimplecondition_t;

					condition->role = role;
					condition->delay = delay;
					condition->event_type = eventtype;
					condition->key = key;
					condition->transition = transition;
					condition->min = min;
					condition->max = max;
					condition->qualifier = qualifier;

					causal->simpleconditions.push_back(condition);
				}

				if (has_condition == false && strcasecmp(action->Value(), "compoundcondition") == 0) {
					has_condition = true;

					std::string logical_operator,
						delay;
					const char *attr;

					if ((attr = action->Attribute("operator")) != NULL) {
						logical_operator = attr;
					}

					if ((attr = action->Attribute("delay")) != NULL) {
						delay = attr;
					}

					JDEBUG(JINFO, "<connectorparam operator=%s delay=%s>\n", logical_operator.c_str(), delay.c_str());

					struct nclcompoundcondition_t *condition = new struct nclcompoundcondition_t;

					condition->logical_operator = logical_operator;
					condition->delay = delay;

					causal->compoundconditions.push_back(condition);
				}

				if (has_action == false && strcasecmp(action->Value(), "simpleaction") == 0) {
					has_action = true;

					std::string role,
						delay, 
						eventtype,
						actiontype,
						value,
						repeat,
						repeatdelay,
						duration,
						by,
						min,
						max,
						qualifier;

					jcommon::XmlAttribute *attr = action->FirstAttribute();

					while (attr != NULL) {
						ATTRIBUTE(role);
						ATTRIBUTE(delay);
						ATTRIBUTE(eventtype);
						ATTRIBUTE(actiontype);
						ATTRIBUTE(value);
						ATTRIBUTE(repeat);
						ATTRIBUTE(repeatdelay);
						ATTRIBUTE(duration);
						ATTRIBUTE(by);
						ATTRIBUTE(min);
						ATTRIBUTE(max);
						ATTRIBUTE(qualifier);

						attr = attr->Next();
					}

					if (role == "") {
						has_action = false;

						JDEBUG(JERROR, "simpleaction role not defined\n");

						continue;
					}

					JDEBUG(JINFO, "<connectorparam role=%s delay=%s eventtype=%s actiontype=%s value=%s repeat=%s repeatdelay=%s duration=%s by=%s min=%s max=%s qualifier=%s>\n", 
							role.c_str(), delay.c_str(), eventtype.c_str(), actiontype.c_str(), value.c_str(), repeat.c_str(), repeatdelay.c_str(), duration.c_str(), by.c_str(), min.c_str(), max.c_str(), qualifier.c_str());

					struct nclsimpleaction_t *action = new struct nclsimpleaction_t;

					action->role = role;
					action->delay = delay;
					action->event_type = eventtype;
					action->action_type = actiontype;
					action->value = value;
					action->repeat = repeat;
					action->repeat_delay = repeatdelay;
					action->duration = duration;
					action->by = by;
					action->min = min;
					action->max = max;
					action->qualifier = qualifier;

					causal->simpleactions.push_back(action);
				}

				if (has_action == false && strcasecmp(action->Value(), "compoundaction") == 0) {
					has_action = true;

					std::string logical_operator,
						delay;
					const char *attr;

					if ((attr = action->Attribute("operator")) != NULL) {
						logical_operator = attr;
					}

					if ((attr = action->Attribute("delay")) != NULL) {
						delay = attr;
					}

					struct nclcompoundaction_t *action = new struct nclcompoundaction_t;

					action->logical_operator = logical_operator;
					action->delay = delay;

					causal->compoundactions.push_back(action);
				}
			} while ((action = action->NextSiblingElement()) != NULL);

			if (has_condition != false && has_action != false) {
				base->causalconnectors.push_back(causal);
			} else {
				JDEBUG(JINFO, "causalconnector has no condition or action\n");

				// TODO:: delete causal
			}
		}
	} while ((connector = connector->NextSiblingElement()) != NULL);

	_connectors.push_back(base);

	JDEBUG(JINFO, "# end\n");

	return true;
}

void NCLLoader::InnerRegionCalculate(struct nclregion_t *parent, struct nclregion_t *t, std::string left, std::string top, std::string right, std::string bottom, std::string width, std::string height)
{
	int pleft = 0,
			ptop = 0,
			pwidth = _width,
			pheight = _height;

	if (parent != NULL) {
		pleft = parent->left;
		ptop = parent->top;
		pwidth = parent->width;
		pheight = parent->height;
	}

	int tleft = pleft,
			ttop = ptop,
			twidth = pwidth,
			theight = pheight;

	if (left != "") {
		tleft = NCLHelper::ParseNumber(left, pwidth, 0, pwidth);

		if (width != "") {
			twidth = NCLHelper::ParseNumber(width, pwidth, 0, pwidth-tleft);
		} else if (right != "") {
			twidth = pwidth-tleft-NCLHelper::ParseNumber(right, pwidth, 0, pwidth-tleft);
		} else {
			twidth = pwidth-tleft;
		}
	} else {
		int sright = NCLHelper::ParseNumber(right, pwidth, 0, pwidth);

		if (width != "") {
			twidth = NCLHelper::ParseNumber(width, pwidth, 0, pwidth-sright);
			tleft = pwidth-sright-twidth;
		} else if (left != "") {
			tleft = NCLHelper::ParseNumber(right, pwidth, 0, pwidth-sright);
			twidth = pwidth-sright-tleft;
		} else {
			twidth = pwidth-sright;
		}
	}

	if (top != "") {
		ttop = NCLHelper::ParseNumber(top, pheight, 0, pheight);

		if (height != "") {
			theight = NCLHelper::ParseNumber(height, pheight, 0, pheight-ttop);
		} else if (bottom != "") {
			theight = pheight-ttop-NCLHelper::ParseNumber(bottom, pheight, 0, pheight-ttop);
		} else {
			theight = pheight-ttop;
		}
	} else {
		int sbottom = NCLHelper::ParseNumber(bottom, pheight, 0, pheight);

		if (height != "") {
			theight = NCLHelper::ParseNumber(height, pheight, 0, pheight-sbottom);
			ttop = pheight-sbottom-theight;
		} else if (top != "") {
			ttop = NCLHelper::ParseNumber(bottom, pheight, 0, pheight-sbottom);
			theight = pheight-sbottom-ttop;
		} else {
			theight = pheight-sbottom;
		}
	}

	t->left = tleft+pleft;
	t->top = ttop+ptop;
	t->width = twidth;
	t->height = theight;
}

struct nclregionbase_t * NCLLoader::GetRegionBase(std::string id)
{
	return NULL;
}

struct ncldescriptorbase_t * NCLLoader::GetDescriptorBase(std::string id)
{
	return NULL;
}

struct nclcausalconnector_t * NCLLoader::GetCausalConnector(std::string id)
{
	for (std::vector<struct nclconnectorbase_t *>::iterator i=_connectors.begin(); i!=_connectors.end(); i++) {
		struct nclconnectorbase_t *base = (*i);

		for (std::vector<struct nclcausalconnector_t *>::iterator j=base->causalconnectors.begin(); j!=base->causalconnectors.end(); j++) {
			struct nclcausalconnector_t *causal = (*j);
			
			if (id == causal->id || id == (base->alias + "#" + causal->id) || id == (base->uri + "#" + causal->id)) {
				return causal;
			}
		}
	}

	return NULL;
}

struct nclregion_t * NCLLoader::GetRegion(std::string id)
{
	for (std::vector<struct nclregionbase_t *>::iterator i=_regions.begin(); i!=_regions.end(); i++) {
		for (std::vector<struct nclregion_t *>::iterator j=(*i)->regions.begin(); j!=(*i)->regions.end(); j++) {
			if ((*j)->id == id) {
				return (*j);
			}
		}
	}

	return NULL;
}

struct ncldescriptor_t * NCLLoader::GetDescriptor(std::string id)
{
	for (std::vector<struct ncldescriptorbase_t *>::iterator i=_descriptors.begin(); i!=_descriptors.end(); i++) {
		for (std::vector<struct ncldescriptor_t *>::iterator j=(*i)->descriptors.begin(); j!=(*i)->descriptors.end(); j++) {
			if ((*j)->id == id) {
				return (*j);
			}
		}
	}

	return NULL;
}

struct nclmedia_t * NCLLoader::GetMedia(struct nclcontext_t *ctx, std::string id)
{
	if (ctx != NULL) {
		for (std::vector<struct nclmedia_t *>::iterator i=ctx->medias.begin(); i!=ctx->medias.end(); i++) {
			if ((*i)->id == id) {
				return (*i);
			}
		}
	}

	return NULL;
}

struct nclcontext_t * NCLLoader::GetContext(std::string id)
{
	std::vector<std::vector<struct nclcontext_t *> *> contexts;

	contexts.push_back(&_contexts);

	do {
		std::vector<struct nclcontext_t *> *lcontexts = (*contexts.begin());

		for (std::vector<struct nclcontext_t *>::iterator i=lcontexts->begin(); i!=lcontexts->end(); i++) {
			struct nclcontext_t *ctx = (*i);

			if (ctx->id == id) {
				return ctx;
			}

			if (ctx->contexts.size() > 0) {
				contexts.push_back(&(ctx->contexts));
			}
		}

		contexts.erase(contexts.begin());
	} while (contexts.size() > 0);

	return NULL;
}

}

