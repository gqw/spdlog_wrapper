#include "logger.hpp"

#include "test/test_log.h"

int main() {
	using namespace wlog;
	if (!logger::get().init("logs/test.log")) {
		return 1;
	}
	logger::get().set_level(spdlog::level::trace);
	STREAM_DEBUG() << "STM_DEBUG " << 1;
	PRINT_WARN("PRINT_WARN, %d", 1);
	LOG_INFO("LOG_INFO {}", 1);

	logger::get().set_level(spdlog::level::info);
	STREAM_DEBUG() << "STM_DEBUG " << 2;
	PRINT_WARN("PRINT_WARN, %d", 2);
	LOG_INFO("LOG_INFO {}", 2);

	test_log();

	// call before spdlog static variables destroy
	logger::get().shutdown();
	return 0;
}