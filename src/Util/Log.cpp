#include "pch.h"

#include "Log.h"
#include "util.h"

bool											Log::s_initialized = false;
std::jthread									Log::s_thread;
std::condition_variable_any						Log::s_condition;
SafeQueue<std::tuple<LogLevel, std::string>>	Log::s_queue;
std::mutex										Log::s_mutex;

Log::Log(const LogLevel level, const std::string_view location, const bool immediately)
	: m_uncaught(std::uncaught_exceptions()), m_level(level), m_immediately(immediately)
{
	m_stream << "[" << Timestamp().timeStr() << "] <" << location << "> ";
}

Log::~Log()
{
	if (m_uncaught != std::uncaught_exceptions() || !s_initialized)
		return;

	m_stream << '\n';

	if (m_immediately) {
		printToConsole(m_level, m_stream.str());
		return;
	}

	s_queue.emplace(m_level, m_stream.str());
	s_condition.notify_all();
}

void Log::init()
{
	assert(!s_initialized && "Logging is already initialized");
	s_initialized = true;

	const auto task = [](const std::stop_token& token) {
		while (!token.stop_requested()) {
			{
				std::unique_lock lock(s_mutex);
				s_condition.wait(lock, token, [&token] { return !s_queue.empty(); });
			}

			while (!s_queue.empty() && !token.stop_requested()) {
				const auto[level, msg] = s_queue.pop();
				printToConsole(level, msg);
			}
		}
	};
	s_thread = std::jthread(task);
}

void Log::shutdown()
{
	assert(s_initialized && "Logging has not been initialized");
	s_initialized = false;

	s_thread.request_stop();
	if (s_thread.joinable()) 
		s_thread.join();
}

void Log::printToConsole(const LogLevel level, const std::string_view msg)
{
	std::lock_guard<std::mutex> lock(s_mutex);

	switch (level) {
		case LogLevel::Trace:
			setColor(Color::Standard);
			break;
		case LogLevel::Info:
			setColor(Color::Green);
			break;
		case LogLevel::Warn:
			setColor(Color::Yellow);
			break;
		case LogLevel::Error: case LogLevel::Fatal:
			setColor(Color::Red);
			break;
	}

	std::cout << msg;
	setColor(Color::Standard);
}

void Log::setColor(const Color color)
{
	std::cout << "\033[" << static_cast<uint16_t>(color) << 'm';
}