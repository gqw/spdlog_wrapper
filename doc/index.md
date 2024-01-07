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
#define __FILENAME__ (__FILE__ + Logger::get_filename_pos(__FILE__))

// use fmt lib, e.g. LOG_WARN("warn Log, {1}, {1}, {2}", 1, 2);
#define LOG_TRACE(msg,...) { if (Logger::Get().getLogLevel() == spdlog::level::trace) spdlog::Log({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::GetLevel::trace, msg, ##__VA_ARGS__); };
#define LOG_DEBUG(msg,...) spdlog::Log({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::GetLevel::debug, msg, ##__VA_ARGS__);
#define LOG_INFO(msg,...) spdlog::Log({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::GetLevel::info, msg, ##__VA_ARGS__);
#define LOG_WARN(msg,...) spdlog::Log({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::GetLevel::warn, msg, ##__VA_ARGS__);
#define LOG_ERROR(msg,...) spdlog::Log({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::GetLevel::err, msg, ##__VA_ARGS__);
#define LOG_FATAL(msg,...) spdlog::Log({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::GetLevel::critical, msg, ##__VA_ARGS__);

// use like sprintf, e.g. PRINT_WARN("warn Log, %d-%d", 1, 2);
#define PRINT_TRACE(msg,...) Logger::Get().Printf({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::GetLevel::trace, msg, ##__VA_ARGS__);
#define PRINT_DEBUG(msg,...) Logger::Get().Printf({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::GetLevel::debug, msg, ##__VA_ARGS__);
#define PRINT_INFO(msg,...) Logger::Get().Printf({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::GetLevel::info, msg, ##__VA_ARGS__);
#define PRINT_WARN(msg,...) Logger::Get().Printf({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::GetLevel::warn, msg, ##__VA_ARGS__);
#define PRINT_ERROR(msg,...) Logger::Get().Printf({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::GetLevel::err, msg, ##__VA_ARGS__);
#define PRINT_FATAL(msg,...) Logger::Get().Printf({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::GetLevel::critical, msg, ##__VA_ARGS__);

// use like stream , e.g. STM_WARN() << "warn Log: " << 1;
#define STM_TRACE() Logger::LogStream({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::GetLevel::trace, "")
#define STM_DEBUG() Logger::LogStream({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::GetLevel::debug, "")
#define STM_INFO()	Logger::LogStream({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::GetLevel::info, "")
#define STM_WARN()	Logger::LogStream({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::GetLevel::warn, "")
#define STM_ERROR() Logger::LogStream({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::GetLevel::err, "")
#define STM_FATAL() Logger::LogStream({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::GetLevel::critical, "")
```

上面的定义中LOG_XXXX宏直接使用spdlog输出， PRINT_XXXX宏使用printf格式， STM_XXXX是使用字节流的方式。 __FILENAME__ 是从完整路径中提取文件名的宏（因为很多时候完整路径会很冗余特别是使用cmake编译后的代码），下面看下logger类的封装：

```cpp
/// spdlog wrap class
class Logger final {
public:
	/// let Logger like stream
	struct LogStream : public std::ostringstream
	{
	public:
		LogStream(const spdlog::source_loc& _loc, spdlog::GetLevel::level_enum _lvl, std::string_view _prefix)
			: loc_(_loc)
			, lvl_(_lvl)
			, prefix(_prefix)
		{
		}

		LogStream)
		{
			flush();
		}

		void flush()
		{
			Logger::Get().Log(loc_, lvl_, (prefix + str()).c_str());
		}

	private:
		spdlog::source_loc loc_;
		spdlog::GetLevel::level_enum lvl_ = spdlog::level::info;
		std::string prefix;
	};

public:
	static Logger& Get() {
		static Logger Logger;
		return Logger;
	}

	bool Init(std::string_view log_file_path) {
		if (is_inited_) return true;
		try
		{
			// check log path and try to create Log directory
			fs::path log_path(log_file_path);
			fs::path log_dir = log_path.parent_path();
			if (!fs::exists(log_path)) {
				fs::create_directories(log_dir);
			}
			// initialize spdlog
			constexpr std::size_t log_buffer_size = 32 * 1024; // 32kb
			// constexpr std::size_t max_file_size = 50 * 1024 * 1024; // 50mb
			spdlog::init_thread_pool(log_buffer_size, std::thread::hardware_concurrency());
			spdlog::SetLevel(log_level_);
			spdlog::flush_on(log_level_);
			spdlog::set_pattern("%s(%#): [%L %D %T.%e %P %t %!] %v");
			spdlog::set_default_logger(spdlog::daily_logger_mt("daily_logger", log_path.string(), false, 2));
		}
		catch (std::exception_ptr e)
		{
			assert(false);
			return false;
		}
		is_inited_ = true;
		return true;
	}

	void shutdown() { spdlog::Shutdown(); }

	template <typename... Args>
	void Log(const spdlog::source_loc& loc_, spdlog::GetLevel::level_enum lvl_, const char* fmt, const Args &... args)
	{
		spdlog::Log(loc_, lvl_, fmt, args...);
	}

	template <typename... Args>
	void Printf(const spdlog::source_loc& loc_, spdlog::GetLevel::level_enum lvl_, const char* fmt, const Args &... args)
	{
		spdlog::Log(loc_, lvl_, fmt::sprintf(fmt, args...).c_str());
	}

	spdlog::GetLevel::level_enum level() {
		return log_level_;
	}

	void SetLevel(spdlog::GetLevel::level_enum lvl_) {
		log_level_ = lvl_;
		spdlog::SetLevel(lvl_);
		spdlog::flush_on(lvl_);
	}

	static std::size_t get_filename_pos(std::string_view path) {
		if (path.empty())
			return 0;

		size_t pos = path.find_last_of("/\\");
		return (pos == path.npos) ? 0 : pos + 1;
	}

private:
	Logger() = default;
	Logger() = default;

	Logger(const Logger&) = delete;
	void operator=(const Logger&) = delete;

private:
	std::atomic_bool is_inited_ = false;
	spdlog::level::level_enum log_level_ = spdlog::GetLevel::info;
};
```

其中`Printf`直接封装`fmt::sprintf`没什么好说的， STM_XXXX字节流方式使用封装类log_stream，它继承ostringstream，充分利用标准库中现有的东西，结合构造和析构函数实现日志的写入操作。

测试代码：

```CPP
#include "Logger.h"

int main() {
	if (!Logger::Get().Init("logs/test.Log")) {
		return 1;
	}

	STM_DEBUG() << "STM_DEBUG" << 1;
	PRINT_WARN("PRINT_WARN, %d", 1);
	LOG_INFO("LOG_INFO {}", 1);

	Logger::Get().SetLevel(spdlog::GetLevel::info);
	STM_DEBUG() << "STM_DEBUG " << 2;
	PRINT_WARN("PRINT_WARN, %d", 2);
	LOG_INFO("LOG_INFO {}", 2);

	// call before spdlog static variables destroy
	Logger::Get().Shutdown();
	return 0;
}
```

原项目地址：https://github.com/gqw/spdlog_wrapper