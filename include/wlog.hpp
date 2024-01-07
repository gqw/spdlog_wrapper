
#ifndef WLOG_LOGGER_
#define WLOG_LOGGER_

#include <iomanip>
#include <sstream>
#include <iostream>

#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include <spdlog/logger.h>
#include <spdlog/fmt/fmt.h>
#include <spdlog/fmt/bundled/printf.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/sinks/stdout_color_sinks.h>


namespace wlog {
class Logger final {
public:

	Logger(const Logger&) = delete;
	void operator=(const Logger&) = delete;


	/// let Logger like stream
	struct LogStream : public std::ostringstream
	{
	public:
		LogStream(const spdlog::source_loc& loc, spdlog::level::level_enum lvl, std::string_view prefix)
			: loc_(loc)
			, lvl_(lvl)
			, prefix_(prefix)
		{
		}

		~LogStream() override
		{
			// 析构输出缓存区的日志文件
			Flush();
		}

		void Flush()
		{
            Logger::Get().Log(loc_, lvl_, (prefix_ + str()).c_str());
		}

	private:
		spdlog::source_loc loc_;
		spdlog::level::level_enum lvl_ = spdlog::level::info;
		std::string prefix_;
	};

public:
	static Logger& Get() {
		static Logger logger;
		return logger;
	}

	bool Init(const std::string& log_file_name,spdlog::level::level_enum lvl=spdlog::level::trace) {
        if (is_inited_.load()) return true;
		try
		{
			// for asyn Logger
			// constexpr std::size_t log_buffer_size = 32 * 1024 * 1024; // 32MB
			// spdlog::init_thread_pool(log_buffer_size, std::thread::hardware_concurrency());

			// 格式化时间，例如 "2_0240106_153045"（年月日_时分秒）
			std::ostringstream oss;
			const auto t = std::time(nullptr);
			const auto tm = *std::localtime(&t);
			oss << std::put_time(&tm, "_%Y%m%d_%H%M%S");
			auto log_path =  "logs/" + log_file_name + oss.str() + ".Log";


			// 线程安全sink
			std::vector<spdlog::sink_ptr> sinks;
			const auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(log_path, true);
			sinks.push_back(file_sink);
			const auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
			// TODO use %^ and %$包裹起来的代码就是带颜色的
			console_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%-8l%$] [%s:%#] %v");
			sinks.push_back(console_sink);
			spdlog::set_default_logger(std::make_shared<spdlog::logger>("", sinks.begin(), sinks.end()));

			// 立即输出等级
			spdlog::flush_on(spdlog::level::debug);
            log_level_ = lvl;
			spdlog::set_level(log_level_);
		}
		catch (const spdlog::spdlog_ex& ex)
		{
			std::cerr << "Log initialization failed: " << ex.what() << std::endl;
			return false;
		}
        is_inited_.store(true);
		return true;
	}

	static void Shutdown() { spdlog::shutdown(); }

	template <typename... Args>
	void Log(const spdlog::source_loc& loc, spdlog::level::level_enum lvl, const char* fmt, const Args &... args)
	{
		spdlog::log(loc, lvl, fmt, args...);
	}

	template <typename... Args>
	void Printf(const spdlog::source_loc& loc, spdlog::level::level_enum lvl, const char* fmt, const Args &... args)
	{
		spdlog::log(loc, lvl, fmt::sprintf(fmt, args...).c_str());
	}

	spdlog::level::level_enum GetLevel() {
		return log_level_;
	}

	void SetLevel(spdlog::level::level_enum lvl) {
        log_level_ = lvl;
		spdlog::set_level(lvl);

	}

	static void SetFlushOn(spdlog::level::level_enum lvl) {
		spdlog::flush_on(lvl);
	}

	static const char* GetShortName(std::string_view path) {
        if (path.empty())
            return path.data();

        size_t pos = path.find_last_of("/\\");
        return path.data() + ((pos == std::string_view::npos) ? 0 : pos + 1);
    }
private:
	Logger() = default;
	~Logger() = default;

private:
	std::atomic_bool is_inited_ = false;
	spdlog::level::level_enum log_level_ = spdlog::level::trace;
};

class LoggerNone {
public:
	LoggerNone() = default;

    static LoggerNone& Get() {
        static LoggerNone logger_none_instance;
        return logger_none_instance;
    }

    template<typename T>
    LoggerNone& operator<<(const T&) {
        return *this;
    }
};

} // namespace wlog

#endif