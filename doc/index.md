# spdlog_wrapper

| date| title|description|author|
| ------------- |:-------------:| :-------------:|-----:|
| 2020-10-28      | `spdlog`简单封装 | `spdlog`使用技巧 | ([@gqw](https://gqw.github.io))|

----
## 工程介绍
----

`spdlog`是一个简单高效的modern c++日志库，它使用c++ 20中全新的format格式，如果全新的工程使用它简单方便。但是实际工作中我们经常会要整合老旧的代码，这些代码中可能会包含各种各样的日志系统，如果想统一这些日志输出全部改一遍费时费力。下面介绍个小技巧让spdlog兼容这些老旧的日志系统格式。

以前的日志通常是通过sprintf函数族和stream字节流两种方式格式化字符串。所以我们需要让`spdlog`能够模仿这两种方法。

一般日志系统都会提供宏来定义日志输出指令，因为这样会方便使用编译器提供`__FILE__`、`__LINE__`这样的内置宏输出文件路径、行号、函数名等信息。所以我们可以通过宏定义替换以前的宏。

```cpp
// got short filename(exlude file directory)
#define __FILENAME__ (__FILE__ + logger::get_filename_pos(__FILE__))

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
```

上面的定义中LOG_XXXX宏直接使用spdlog输出， PRINT_XXXX宏使用printf格式， STM_XXXX是使用字节流的方式。 __FILENAME__ 是从完整路径中提取文件名的宏（因为很多时候完整路径会很冗余特别是使用cmake编译后的代码），下面看下logger类的封装：

```cpp
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

	static std::size_t get_filename_pos(std::string_view path) {
		if (path.empty())
			return 0;

		size_t pos = path.find_last_of("/\\");
		return (pos == path.npos) ? 0 : pos + 1;
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
```

其中`printf`直接封装`fmt::sprintf`没什么好说的， STM_XXXX字节流方式使用封装类log_stream，它继承ostringstream，充分利用标准库中现有的东西，结合构造和析构函数实现日志的写入操作。

测试代码：

```CPP
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
```

项目地址：https://github.com/gqw/spdlog_wrapper