#include "test_log.h"

#include <logger.hpp>

void test_log()
{
	STREAM_DEBUG() << "STM_DEBUG " << 3;
    PRINT_WARN("PRINT_WARN, %d", 3);
	LOG_INFO("LOG_INFO {}", 3);
}