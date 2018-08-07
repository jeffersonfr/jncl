#ifndef MEDIAEXCEPTION_H
#define MEDIAEXCEPTION_H

#include "jexception/jruntimeexception.h"

namespace media {

class MediaException : public jexception::RuntimeException {

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

