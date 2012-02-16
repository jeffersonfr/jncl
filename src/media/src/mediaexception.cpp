#include "mediaexception.h"

namespace media {

MediaException::MediaException(std::string reason):
	jcommon::RuntimeException(reason)
{
}

MediaException::~MediaException() throw()
{
}

}

