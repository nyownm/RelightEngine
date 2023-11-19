#include "Core/Logging.h"
#include <spdlog/common.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace RE {
void FLogging::Init() {
    std::vector<spdlog::sink_ptr> sinks;
    sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    auto logger = std::make_shared<spdlog::logger>("logger", sinks.begin(), sinks.end());

#if RE_DEVELOPMENT
    logger->set_level(spdlog::level::debug);
#else
    logger->set_level(spdlog::level::info);
#endif

    logger->set_pattern(RE_LOGGER_FORMAT);
    spdlog::set_default_logger(logger);

    RE_LOGI("Logger initialized");
}
}