#ifndef WLOG_MACRO_DEFINE_
#define WLOG_MACRO_DEFINE_

#include <wlog.hpp>

#define LOG_LEVEL_TRACE    0
#define LOG_LEVEL_DEBUG    1
#define LOG_LEVEL_INFO     2
#define LOG_LEVEL_WARN     3
#define LOG_LEVEL_ERROR    4
#define LOG_LEVEL_FATAL    5
#define LOG_LEVEL_CLOSE    6

// 全局日志开关，优先级高于Logger::Get().SetLevel(spdlog::GetLevel::err);
#define LOGGER_LEVEL LOG_LEVEL_CLOSE

// got short filename(exlude file directory)
#define __FILENAME__ (wlog::Logger::GetShortName(__FILE__))


#if (LOGGER_LEVEL <= LOG_LEVEL_TRACE)
#	define	 LOG_TRACE(fmt, ...) 		spdlog::log({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::level::trace, fmt, ##__VA_ARGS__);
#	define 	 PRINT_TRACE(fmt,...) 		wlog::Logger::Get().Printf({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::level::trace,fmt, ##__VA_ARGS__);
#	define	 STREAM_TRACE() 			wlog::Logger::LogStream({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::level::trace, "")
// with prefix version
// #	define LOG_TRACE(fmt, ...) spdlog::Log({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::GetLevel::trace, "MyPrefix " fmt, ##__VA_ARGS__);
// #	define PRINT_TRACE(fmt, ...) wlog::Logger::Get().Printf({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::GetLevel::trace, "MyPrefix " fmt, ##__VA_ARGS__);
// #	define STREAM_TRACE() wlog::Logger::LogStream({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::GetLevel::trace, "MyPrefix")
#else
#	define	 LOG_TRACE(fmt, ...)
#	define 	 PRINT_TRACE(fmt,...)
#	define	 STREAM_TRACE() wlog::LoggerNone::Get()
#endif

#if (LOGGER_LEVEL <= LOG_LEVEL_DEBUG)
#	define	 LOG_DEBUG(fmt, ...) 		spdlog::log({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::level::debug, fmt, ##__VA_ARGS__);
#	define 	 PRINT_DEBUG(fmt,...) 		wlog::Logger::Get().Printf({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::level::debug, fmt, ##__VA_ARGS__);
#	define	 STREAM_DEBUG() 			wlog::Logger::LogStream({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::level::debug, "")
#else
#	define	 LOG_DEBUG(fmt, ...)
#	define 	 PRINT_DEBUG(fmt,...)
#	define	 STREAM_DEBUG() wlog::LoggerNone::Get()
#endif

#if (LOGGER_LEVEL <= LOG_LEVEL_INFO)
#	define	 LOG_INFO(fmt, ...) 		spdlog::log({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::level::info, fmt, ##__VA_ARGS__);
#	define 	 PRINT_INFO(fmt,...) 		wlog::Logger::Get().Printf({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::level::info, fmt, ##__VA_ARGS__);
#	define	 STREAM_INFO() 			wlog::Logger::LogStream({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::level::info, "")
#else
#	define	 LOG_INFO(fmt, ...)
#	define 	 PRINT_INFO(fmt,...)
#	define	 STREAM_INFO() wlog::LoggerNone::Get()
#endif

#if (LOGGER_LEVEL <= LOG_LEVEL_WARN)
#	define	 LOG_WARN(fmt, ...) 		spdlog::log({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::level::warn, fmt, ##__VA_ARGS__);
#	define 	 PRINT_WARN(fmt,...) 		wlog::Logger::Get().Printf({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::level::warn, fmt, ##__VA_ARGS__);
#	define	 STREAM_WARN() 			wlog::Logger::LogStream({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::level::warn, "")
#else
#	define	 LOG_WARN(fmt, ...)
#	define 	 PRINT_WARN(fmt,...)
#	define	 STREAM_WARN() wlog::LoggerNone::Get()
#endif

#if (LOGGER_LEVEL <= LOG_LEVEL_ERROR)
#	define	 LOG_ERROR(fmt, ...) 		spdlog::log({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::level::error, fmt, ##__VA_ARGS__);
#	define 	 PRINT_ERROR(fmt,...) 		wlog::Logger::Get().Printf({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::level::error, fmt, ##__VA_ARGS__);
#	define	 STREAM_ERROR() 			wlog::Logger::LogStream({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::level::error, "")
#else
#	define	 LOG_ERROR(fmt, ...)
#	define 	 PRINT_ERROR(fmt,...)
#	define	 STREAM_ERROR() wlog::LoggerNone::Get()
#endif

#if (LOGGER_LEVEL <= LOG_LEVEL_FATAL)
#	define	 LOG_FATAL(fmt, ...) 		spdlog::log({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::level::fatal, fmt, ##__VA_ARGS__);
#	define 	 PRINT_FATAL(fmt,...) 		wlog::Logger::Get().Printf({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::level::fatal, fmt, ##__VA_ARGS__);
#	define	 STREAM_FATAL() 			wlog::Logger::LogStream({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::level::fatal, "")
#else
#	define	 LOG_FATAL(fmt, ...)
#	define 	 PRINT_FATAL(fmt,...)
#	define	 STREAM_FATAL() wlog::LoggerNone::Get()
#endif

#endif // WLOG_MACRO_DEFINE_

