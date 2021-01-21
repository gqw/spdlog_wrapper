#pragma once

#include <filesystem>
#include <sstream>
#include <memory>

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
#include <spdlog/sinks/stdout_color_sinks.h>

namespace fs = std::filesystem;
/// spdlog wrap class

namespace wlog {
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
			std::vector<spdlog::sink_ptr> sinks;
			auto daily_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(log_path.string(), 0, 2);
			sinks.push_back(daily_sink);

			// auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(log_path.string(), true);
			// sinks.push_back(file_sink);

#if defined(_DEBUG) && defined(WIN32)  			
			auto ms_sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
			sinks.push_back(ms_sink);
#endif //  _DEBUG

#ifdef _LOG_CONSOLE
			auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
			sinks.push_back(console_sink);
#endif	
			spdlog::set_default_logger(std::make_shared<spdlog::logger>("", sinks.begin(), sinks.end()));

			spdlog::set_pattern("%s(%#): [%L %D %T.%e %P %t %!] %v");
			spdlog::flush_on(spdlog::level::warn);
			spdlog::set_level(_log_level);
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
		
	}

	void set_flush_on(spdlog::level::level_enum lvl) {
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
	spdlog::level::level_enum _log_level = spdlog::level::trace;
};
} // namespace wlog


// got short filename(exlude file directory)
#define __FILENAME__ (wlog::logger::get_shortname(__FILE__))

// use fmt lib, e.g. LOG_WARN("warn log, {1}, {1}, {2}", 1, 2);
#define LOG_TRACE(msg,...) { if (wlog::logger::get().getLogLevel() == wlog::spdlog::level::trace) spdlog::log({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::level::trace, msg, ##__VA_ARGS__); };
#define LOG_DEBUG(msg,...) spdlog::log({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::level::debug, msg, ##__VA_ARGS__)
#define LOG_INFO(msg,...)  spdlog::log({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::level::info, msg, ##__VA_ARGS__)
#define LOG_WARN(msg,...)  spdlog::log({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::level::warn, msg, ##__VA_ARGS__)
#define LOG_ERROR(msg,...) spdlog::log({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::level::err, msg, ##__VA_ARGS__)
#define LOG_FATAL(msg,...) spdlog::log({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::level::critical, msg, ##__VA_ARGS__)

// use like sprintf, e.g. PRINT_WARN("warn log, %d-%d", 1, 2);
#define PRINT_TRACE(msg,...) wlog::logger::get().printf({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::level::trace, msg, ##__VA_ARGS__);
#define PRINT_DEBUG(msg,...) wlog::logger::get().printf({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::level::debug, msg, ##__VA_ARGS__);
#define PRINT_INFO(msg,...)  wlog::logger::get().printf({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::level::info, msg, ##__VA_ARGS__);
#define PRINT_WARN(msg,...)  wlog::logger::get().printf({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::level::warn, msg, ##__VA_ARGS__);
#define PRINT_ERROR(msg,...) wlog::logger::get().printf({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::level::err, msg, ##__VA_ARGS__);
#define PRINT_FATAL(msg,...) wlog::logger::get().printf({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::level::critical, msg, ##__VA_ARGS__);

// use like stream , e.g. STM_WARN() << "warn log: " << 1;
#define STM_TRACE() wlog::logger::log_stream({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::level::trace, "")
#define STM_DEBUG() wlog::logger::log_stream({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::level::debug, "")
#define STM_INFO()	wlog::logger::log_stream({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::level::info, "")
#define STM_WARN()	wlog::logger::log_stream({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::level::warn, "")
#define STM_ERROR() wlog::logger::log_stream({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::level::err, "")
#define STM_FATAL() wlog::logger::log_stream({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::level::critical, "")

