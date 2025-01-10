#pragma once

#include "Timestamp.h"
#include "SafeQueue.h"

#define METHOD_NAME             functionToLocation(std::source_location::current().function_name())

#define LOG_INIT()			    Log::init()
#define LOG_SHUTDOWN()			Log::shutdown()

#define LOG_TRACE				Log(LogLevel::Trace, METHOD_NAME)
#define LOG_INFO				Log(LogLevel::Info, METHOD_NAME)
#define LOG_WARN				Log(LogLevel::Warn, METHOD_NAME)
#define LOG_ERROR				Log(LogLevel::Error, METHOD_NAME)
#define LOG_FATAL				Log(LogLevel::Fatal, METHOD_NAME)

#define LOG_TRACE_IMMEDIATELY	Log(LogLevel::Trace, METHOD_NAME, true)
#define LOG_INFO_IMMEDIATELY	Log(LogLevel::Info, METHOD_NAME, true)
#define LOG_WARN_IMMEDIATELY	Log(LogLevel::Warn, METHOD_NAME, true)
#define LOG_ERROR_IMMEDIATELY	Log(LogLevel::Error, METHOD_NAME, true)
#define LOG_FATAL_IMMEDIATELY	Log(LogLevel::Fatal, METHOD_NAME, true)

#ifdef __cpp_lib_format
    #define FMT_TRACE(fmt, ...)		Log::format(LogLevel::Trace, METHOD_NAME, fmt, __VA_ARGS__)
    #define FMT_INFO(fmt, ...)		Log::format(LogLevel::Info, METHOD_NAME, fmt, __VA_ARGS__)
    #define FMT_WARN(fmt, ...)		Log::format(LogLevel::Warn, METHOD_NAME, fmt, __VA_ARGS__)
    #define FMT_ERROR(fmt, ...)		Log::format(LogLevel::Error, METHOD_NAME, fmt, __VA_ARGS__)
    #define FMT_FATAL(fmt, ...)		Log::format(LogLevel::Fatal, METHOD_NAME, fmt, __VA_ARGS__)
#endif

enum class LogLevel {
	Trace,
	Info,
	Warn,
	Error,
	Fatal
};

class Log
{
public:
	Log(LogLevel level, std::string_view location, bool immediately = false);
	~Log();

	template <typename T>
	Log& operator<<(T&& t) { m_stream << std::forward<T>(t); return *this; }

	Log& operator<<(std::ostream& (*manip)(std::ostream&)) { m_stream << manip; return *this; }

	static void init();
	static void shutdown();

#ifdef __cpp_lib_format
	template <typename ...T>
	inline static constexpr void format(const LogLevel level, const std::string_view location, const std::string_view fmt, const T& ...args)
	{
		Log(level, location) << std::vformat(fmt, std::make_format_args(args...));
	}
#endif

private:
	enum class Color : uint16_t 
	{
		Standard = 0,
		Red = 31,
		Green = 32,
		Yellow = 33
	};

	static void printToConsole(LogLevel level, std::string_view msg);
	static void setColor(Color color);

	const int m_uncaught;
	const LogLevel m_level;
	const bool m_immediately;

	std::stringstream m_stream;

	static bool s_initialized;
	static std::jthread s_thread;
	static std::condition_variable_any s_condition;
	static SafeQueue<std::tuple<LogLevel, std::string>> s_queue;
    static std::mutex s_mutex;

};

static std::string functionToLocation(const std::string& func)
{
	std::smatch match;
	if (std::regex_search(func, match, std::regex(R"((?:\b(\w+)\b::)?\b(\w+)\b(?=\s*\())"))) {
		if (match[1].matched) {
			return match[1].str() + "::" + match[2].str();
		}
		else {
			return match[2].str();
		}
	}

	return "";
}