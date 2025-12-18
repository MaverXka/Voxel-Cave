#include "Thread.h"
#include <thread>
#include <functional>
#include <string>
#include <map>


std::map<std::thread::id, const char*> Thread::m_threadAssociations;

Thread Thread::Create(void(*func)(), const char* ThreadName)
{
	Thread newThread = Thread();
	newThread.m_thread = std::thread(func);
	m_threadAssociations[newThread.m_thread.get_id()] = ThreadName;
	return newThread;
}

Thread Thread::Create(std::function<void()> func, const char* ThreadName)
{
	Thread newThread = Thread();
	newThread.m_thread = std::thread(func);
	m_threadAssociations[newThread.m_thread.get_id()] = ThreadName;
	return newThread;
}

const char* Thread::GetThreadName() const
{
	return m_threadAssociations[m_thread.get_id()];
}

std::string Thread::GetCurrentThreadName()
{
	return m_threadAssociations[std::this_thread::get_id()];
}

void Thread::AssignMainThreadName()
{
	m_threadAssociations[std::this_thread::get_id()] = "Main";
}

void Thread::Join()
{
	if (m_thread.joinable())
	{
		m_thread.join();
	}
}

void Thread::Detach()
{
	m_thread.detach();
}
