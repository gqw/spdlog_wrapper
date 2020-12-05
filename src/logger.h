#pragma once

#include <filesystem>
#include <sstream>

#ifndef NOMINMAX
#	undef min
#	undef max
#endif

#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include <spdlog/logger.h>
#include <spdlog/fmt/fmt.h>
#include <spdlog/fmt/bundled/printf.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/msvc_sink.h>
#include <spdlog/sinks/stdout_sinks.h>

namespace fs = std::filesystem;
/// spdlog wrap class
class logger final {
public:
	/// let logger like stream
	struct log_stream : public std::ostringstream
	{
	public:
		log_stream(const spdlog::source_loc& _loc, spdlog::level::level_enum _lvl, std::string_view _prefix)
			: loc(_loc)
			, lvl(_lvl)
			, prefix(_prefix)
		{
		}

		~log_stream()
		{
			flush();
		}

		void flush()
		{
			logger::get().log(loc, lvl, (prefix + str()).c_str());
		}

	private:
		spdlog::source_loc loc;
		spdlog::level::level_enum lvl = spdlog::level::info;
		std::string prefix;
	};

public:
	static logger& get() {
		static logger logger;
		return logger;
	}

	bool init(std::string_view log_file_path) {
		if (_is_inited) return true;
		try
		{
			// check log path and try to create log directory
			fs::path log_path(log_file_path);
			fs::path log_dir = log_path.parent_path();
			if (!fs::exists(log_path)) {
				fs::create_directories(log_dir);
			}
			// initialize spdlog
			constexpr std::size_t log_buffer_size = 32 * 1024; // 32kb
			// constexpr std::size_t max_file_size = 50 * 1024 * 1024; // 50mb
			spdlog::init_thread_pool(log_buffer_size, std::thread::hardware_concurrency());
			spdlog::set_level(_log_level);
			spdlog::flush_on(_log_level);
			spdlog::set_pattern("%s(%#): [%L %D %T.%e %P %t %!] %v");
			spdlog::set_default_logger(spdlog::daily_logger_mt("daily_logger", log_path.string(), false, 2));
		}
		catch (std::exception_ptr e)
		{
			assert(false);
			return false;
		}
		_is_inited = true;
		return true;
	}

	void shutdown() { spdlog::shutdown(); }

	template <typename... Args>
	void log(const spdlog::source_loc& loc, spdlog::level::level_enum lvl, const char* fmt, const Args &... args)
	{
		spdlog::log(loc, lvl, fmt, args...);
	}

	template <typename... Args>
	void printf(const spdlog::source_loc& loc, spdlog::level::level_enum lvl, const char* fmt, const Args &... args)
	{
		spdlog::log(loc, lvl, fmt::sprintf(fmt, args...).c_str());
	}

	spdlog::level::level_enum level() {
		return _log_level;
	}

	void set_level(spdlog::level::level_enum lvl) {
		_log_level = lvl;
		spdlog::set_level(lvl);
		spdlog::flush_on(lvl);
	}

	static const char* get_shortname(std::string_view path) {
		if (path.empty())
			return path.data();

		size_t pos = path.find_last_of("/\\");
		return path.data() + ((pos == path.npos) ? 0 : pos + 1);
	}

private:
	logger() = default;
	~logger() = default;

	logger(const logger&) = delete;
	void operator=(const logger&) = delete;

private:
	std::atomic_bool _is_inited = false;
	spdlog::level::level_enum _log_level = spdlog::level::info;
};

// got short filename(exlude file directory)
#define __FILENAME__ (logger::get_shortname(__FILE__))

// use fmt lib, e.g. LOG_WARN("warn log, {1}, {1}, {2}", 1, 2);
#define LOG_TRACE(msg,...) { if (logger::get().getLogLevel() == spdlog::level::trace) spdlog::log({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::level::trace, msg, ##__VA_ARGS__); };
#define LOG_DEBUG(msg,...) spdlog::log({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::level::debug, msg, ##__VA_ARGS__);
#define LOG_INFO(msg,...) spdlog::log({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::level::info, msg, ##__VA_ARGS__);
#define LOG_WARN(msg,...) spdlog::log({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::level::warn, msg, ##__VA_ARGS__);
#define LOG_ERROR(msg,...) spdlog::log({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::level::err, msg, ##__VA_ARGS__);
#define LOG_FATAL(msg,...) spdlog::log({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::level::critical, msg, ##__VA_ARGS__);

// use like sprintf, e.g. PRINT_WARN("warn log, %d-%d", 1, 2);
#define PRINT_TRACE(msg,...) logger::get().printf({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::level::trace, msg, ##__VA_ARGS__);
#define PRINT_DEBUG(msg,...) logger::get().printf({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::level::debug, msg, ##__VA_ARGS__);
#define PRINT_INFO(msg,...) logger::get().printf({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::level::info, msg, ##__VA_ARGS__);
#define PRINT_WARN(msg,...) logger::get().printf({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::level::warn, msg, ##__VA_ARGS__);
#define PRINT_ERROR(msg,...) logger::get().printf({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::level::err, msg, ##__VA_ARGS__);
#define PRINT_FATAL(msg,...) logger::get().printf({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::level::critical, msg, ##__VA_ARGS__);

// use like stream , e.g. STM_WARN() << "warn log: " << 1;
#define STM_TRACE() logger::log_stream({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::level::trace, "")
#define STM_DEBUG() logger::log_stream({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::level::debug, "")
#define STM_INFO()	logger::log_stream({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::level::info, "")
#define STM_WARN()	logger::log_stream({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::level::warn, "")
#define STM_ERROR() logger::log_stream({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::level::err, "")
#define STM_FATAL() logger::log_stream({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::level::critical, "")