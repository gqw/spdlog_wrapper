#include "logger.hpp"

#include "test/test_log.h"
#include <iostream>
#include "spdlog/stopwatch.h"
int main() {
	using namespace wlog;
	if (!Logger::Get().Init("spdlog_warp", static_cast<spdlog::level::level_enum>(LOGGER_LEVEL))) {
		return 1;
	}

	STREAM_DEBUG() << "STM_DEBUG " << 1;
    PRINT_WARN("PRINT_WARN, %d", 1);
	LOG_INFO("LOG_INFO {}", 1);


	LOG_INFO("this is a new test {} {} {}",1.2,"sa","das");

    PRINT_TRACE("hello {%d}", 1);
	STREAM_TRACE()<<"hello";


	// Logger::Get().SetLevel(spdlog::GetLevel::info);
	STREAM_DEBUG() << "STM_DEBUG " << 2;
    PRINT_WARN("PRINT_WARN, %d", 2);
	LOG_INFO("LOG_INFO {}", 2);

	spdlog::stopwatch sw;
	spdlog::debug("Elapsed {}", sw);
	LOG_DEBUG("Elapsed {:.3}", sw);

	test_log();


	// call before spdlog static variables destroy
    Logger::Shutdown();
	return 0;
}