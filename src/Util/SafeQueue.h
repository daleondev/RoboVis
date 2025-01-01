#pragma once

#define SAFE(x) { std::lock_guard<std::mutex> lock(m_mutex); x }

template <typename T>
class SafeQueue
{
public:
	SafeQueue() = default;
	~SafeQueue() = default;

	bool empty() const { SAFE(return m_queue.empty();) }
	size_t size() const { SAFE(return m_queue.size();) }

	void push(const T& value) { SAFE(m_queue.push(value);) }
	void push(T&& value) { SAFE(m_queue.push(std::move(value));) }

	template< typename... Type>
	decltype(auto) emplace(Type&&... args) { SAFE(return m_queue.emplace(args...);) }

	T pop() { SAFE(T temp = m_queue.front(); m_queue.pop(); return temp;) }

private:
	std::queue<T> m_queue;
	mutable std::mutex m_mutex;

};