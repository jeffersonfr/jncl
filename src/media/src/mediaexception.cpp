#include "mediaexception.h"

namespace media {

MediaException::MediaException(std::string reason):
	jexception::RuntimeException(reason)
{
}

MediaException::~MediaException() throw()
{
}

}

