#pragma once
#include "re-core_export.h"

#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>

#define RE_LOGGER_FORMAT "[%^%l%$] %v"

// Mainly for IDEs
#ifndef RE_ROOT_PATH_SIZE
    #define RE_ROOT_PATH_SIZE 0
#endif

#define RE_LOGI(...) spdlog::info(__VA_ARGS__)
#define RE_LOGW(...) spdlog::warn(__VA_ARGS__)
#define RE_LOGE(...)                                                             \
    spdlog::error(                                                               \
        "[{}:{}] {}", (static_cast<const char *>(__FILE__) + RE_ROOT_PATH_SIZE), \
        __LINE__, fmt::format(__VA_ARGS__))
#define RE_LOGD(...) spdlog::debug(__VA_ARGS__)

#if RE_DEVELOPMENT
    #define checkf(expr, ...)         \
        do {                          \
            if(!(expr)) {             \
                RE_LOGE(__VA_ARGS__); \
                __debugbreak();       \
            }                         \
        } while(false)
    #define check(expr) checkf(expr, "Check failed")
#else
    #define checkf(expr, ...) (expr)
#endif

namespace RE {
class RE_CORE_EXPORT FLogging {
public:
    static void Init();
};
} // namespace RE