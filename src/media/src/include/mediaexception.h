#ifndef MEDIAEXCEPTION_H
#define MEDIAEXCEPTION_H

#include "jruntimeexception.h"

namespace media {

class MediaException : public jcommon::RuntimeException{

	public:
		/**
		 * \brief Construtor default.
		 *
		 */
		MediaException(std::string reason);

		/**
		 * \brief Destrutor virtual.
		 *
		 */
		virtual ~MediaException() throw();

};

}

#endif

