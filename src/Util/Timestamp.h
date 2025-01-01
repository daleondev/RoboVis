#pragma once

struct TimeData
{
    int64_t timeSinceEpoch_ms;

    uint16_t year;
	uint16_t month;
	uint16_t day;
	uint16_t hour;
	uint16_t minute;
	uint16_t second;
	uint16_t millisecond;
};

class Timestamp
{
public:
	Timestamp();
	Timestamp(const std::string& timestamp);
	~Timestamp() = default;

	std::string dateStr() const;
	std::string timeStr() const;
	std::string dateTimeStr() const;

	friend std::ostream& operator<<(std::ostream& os, const Timestamp& t);
	friend bool operator!=(const Timestamp& lhs, const Timestamp& rhs);
	friend bool operator>(const Timestamp& lhs, const Timestamp& rhs);

	inline TimeData getData() const { return m_data; }

private:
	TimeData m_data;

};