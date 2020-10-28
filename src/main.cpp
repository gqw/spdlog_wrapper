#include "logger.h"

int main() {
	if (!logger::get().init("logs/test.log")) {
		return 1;
	}
	
	STM_DEBUG() << "STM_DEBUG" << 1;
	PRINT_WARN("PRINT_WARN, %d", 1);
	LOG_INFO("LOG_INFO {}", 1);

	logger::get().set_level(spdlog::level::info);
	STM_DEBUG() << "STM_DEBUG " << 2;
	PRINT_WARN("PRINT_WARN, %d", 2);
	LOG_INFO("LOG_INFO {}", 2);

	// call before spdlog static variables destroy
	logger::get().shutdown();
	return 0;
}