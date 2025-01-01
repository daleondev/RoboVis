#include "pch.h"

#include "Timestamp.h"

Timestamp::Timestamp()
{
	// local time
	tm localtime{};
	const std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	const time_t currentTime = std::chrono::system_clock::to_time_t(now);
    localtime_r(&currentTime, &localtime);

	// milliseconds
	const std::chrono::duration<double> tse = now.time_since_epoch();
	m_data.timeSinceEpoch_ms = std::chrono::duration_cast<std::chrono::milliseconds>(tse).count();

	// timestamp
	m_data.year			= static_cast<uint16_t>(localtime.tm_year + 1900);
	m_data.month		= static_cast<uint16_t>(localtime.tm_mon + 1);
	m_data.day			= static_cast<uint16_t>(localtime.tm_mday);
	m_data.hour			= static_cast<uint16_t>(localtime.tm_hour);
	m_data.minute		= static_cast<uint16_t>(localtime.tm_min);
	m_data.second		= static_cast<uint16_t>(localtime.tm_sec);
	m_data.millisecond	= static_cast<uint16_t>(m_data.timeSinceEpoch_ms % 1000);
}

Timestamp::Timestamp(const std::string& timestamp)
{
	m_data.day			= static_cast<uint16_t>(std::stoi(timestamp.substr(0, 2)));
	m_data.month		= static_cast<uint16_t>(std::stoi(timestamp.substr(3, 2)));
	m_data.year			= static_cast<uint16_t>(std::stoi(timestamp.substr(6, 4)));
	m_data.hour			= static_cast<uint16_t>(std::stoi(timestamp.substr(11, 2)));
	m_data.minute		= static_cast<uint16_t>(std::stoi(timestamp.substr(14, 2)));
	m_data.second		= static_cast<uint16_t>(std::stoi(timestamp.substr(17, 2)));
	m_data.millisecond	= static_cast<uint16_t>(std::stoi(timestamp.substr(20, 3)));
}

std::string Timestamp::dateStr() const
{
	std::stringstream ss;

	m_data.day < 10 ? ss << '0' << m_data.day : ss << m_data.day;
	ss << '.';
	m_data.month < 10 ? ss << '0' << m_data.month : ss << m_data.month;
	ss << '.';
	ss << m_data.year;

	return ss.str();
}

std::string Timestamp::timeStr() const
{
	std::stringstream ss;

	m_data.hour < 10 ? ss << '0' << m_data.hour : ss << m_data.hour;
	ss << ':';
	m_data.minute < 10 ? ss << '0' << m_data.minute : ss << m_data.minute;
	ss << ':';
	m_data.second < 10 ? ss << '0' << m_data.second : ss << m_data.second;
	ss << ':';
	m_data.millisecond < 100 ? m_data.millisecond < 10 ? ss << "00" << m_data.millisecond : ss << '0' << m_data.millisecond : ss << m_data.millisecond;

	return ss.str();
}

std::string Timestamp::dateTimeStr() const
{
	std::stringstream ss;

	ss << dateStr();
    ss << ' ';
    ss << timeStr();

	return ss.str();
}

bool operator!=(const Timestamp& lhs, const Timestamp& rhs)
{
	return lhs.m_data.year != rhs.m_data.year || lhs.m_data.month != rhs.m_data.month || lhs.m_data.day != rhs.m_data.day
		|| lhs.m_data.hour != rhs.m_data.hour || lhs.m_data.minute != rhs.m_data.minute || lhs.m_data.second != rhs.m_data.second || lhs.m_data.millisecond != rhs.m_data.millisecond;
}

std::ostream& operator<<(std::ostream& os, const Timestamp& t)
{
	os << t.dateTimeStr();
	return os;
}

bool operator>(const Timestamp& lhs, const Timestamp& rhs)
{
	if (lhs.m_data.year > rhs.m_data.year)
		return true;
	if (lhs.m_data.year < rhs.m_data.year)
		return false;

	if (lhs.m_data.month > rhs.m_data.month)
		return true;
	if (lhs.m_data.month < rhs.m_data.month)
		return false;

	if (lhs.m_data.day > rhs.m_data.day)
		return true;
	if (lhs.m_data.day < rhs.m_data.day)
		return false;

	if (lhs.m_data.hour > rhs.m_data.hour)
		return true;
	if (lhs.m_data.hour < rhs.m_data.hour)
		return false;

	if (lhs.m_data.minute > rhs.m_data.minute)
		return true;
	if (lhs.m_data.minute < rhs.m_data.minute)
		return false;

	if (lhs.m_data.second > rhs.m_data.second)
		return true;
	if (lhs.m_data.second < rhs.m_data.second)
		return false;

	if (lhs.m_data.millisecond > rhs.m_data.millisecond)
		return true;

	return false;
}