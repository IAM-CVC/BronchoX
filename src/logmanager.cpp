//
// Created by Esmitt Ramirez on 17/9/21.
//

#include <spdlog/sinks/stdout_color_sinks.h>
#include <memory>

#include "logmanager.h"
namespace bronchox::log {

	void LogManager::Initialize() {
		auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_st>();
		consoleSink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] %v");

		std::vector<spdlog::sink_ptr> sinks{ consoleSink };
		auto logger = std::make_shared<spdlog::logger>(BRONCHOX_DEFAULT_LOGGER, sinks.begin(), sinks.end());
		logger->set_level(spdlog::level::trace);
		logger->flush_on(spdlog::level::trace);
		spdlog::register_logger(logger);
	}

	void LogManager::Shutdown() {
		spdlog::shutdown();
	}
}