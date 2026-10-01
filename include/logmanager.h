#ifndef __LOGMANAGER_H__
#define __LOGMANAGER_H__

#include "log.h"

namespace bronchox {
	namespace log {

		class LogManager {
		public:
			LogManager() = default;
			~LogManager() = default;

			void Initialize();
			void Shutdown();
		};

	}
}

#endif // __LOGMANAGER_H__
