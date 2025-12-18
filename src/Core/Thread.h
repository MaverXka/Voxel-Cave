#pragma once

#include <thread>
#include <functional>
#include <string>
#include <map>

class Thread
{
public:

	static Thread Create(void(*func)(), const char* ThreadName);
	static Thread Create(std::function<void()> func, const char* ThreadName);

	const char* GetThreadName() const;

	static std::string GetCurrentThreadName();

	static void AssignMainThreadName();

	void Join();
	void Detach();

protected:

	static std::map<std::thread::id, const char*> m_threadAssociations;

	std::thread m_thread;

};

