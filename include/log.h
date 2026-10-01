#ifndef __LOG_H__
#define __LOG_H__

#include "logmanager.h"

#include <spdlog/spdlog.h>

#define BRONCHOX_DEFAULT_LOGGER "bronchoxlogger"

#ifndef BRONCHO_RELEASE
#define BRONCHOX_TRACE(...) if(spdlog::get(BRONCHOX_DEFAULT_LOGGER) != nullptr) \
  {spdlog::get(BRONCHOX_DEFAULT_LOGGER)->trace(__VA_ARGS__);}

#define BRONCHOX_DEBUG(...) if(spdlog::get(BRONCHOX_DEFAULT_LOGGER) != nullptr) \
  {spdlog::get(BRONCHOX_DEFAULT_LOGGER)->debug(__VA_ARGS__);}

#define BRONCHOX_INFO(...) if(spdlog::get(BRONCHOX_DEFAULT_LOGGER) != nullptr) \
  {spdlog::get(BRONCHOX_DEFAULT_LOGGER)->info(__VA_ARGS__);}

#define BRONCHOX_WARN(...) if(spdlog::get(BRONCHOX_DEFAULT_LOGGER) != nullptr) \
  {spdlog::get(BRONCHOX_DEFAULT_LOGGER)->warn(__VA_ARGS__);}

#define BRONCHOX_ERROR(...) if(spdlog::get(BRONCHOX_DEFAULT_LOGGER) != nullptr) \
  {spdlog::get(BRONCHOX_DEFAULT_LOGGER)->error(__VA_ARGS__);}

#define BRONCHOX_CRITICAL(...) if(spdlog::get(BRONCHOX_DEFAULT_LOGGER) != nullptr) \
  {spdlog::get(BRONCHOX_DEFAULT_LOGGER)->critical(__VA_ARGS__);}
#endif

#endif  //__LOG_H__