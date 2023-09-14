//
// Created by Minkyeu Kim on 9/14/23.
//

#ifndef SCOOP_THREADWORKER_H
#define SCOOP_THREADWORKER_H

class ThreadPool;

#include "ThreadPool.h"

class ThreadWorker
{
private:
	ThreadPool* m_ThreadPoolPtr;

public:
	ThreadWorker() = delete;

	explicit ThreadWorker(ThreadPool* pool)
		: m_ThreadPoolPtr(pool)
	{}

	void operator()()
	{
		std::unique_lock<std::mutex> lock(m_ThreadPoolPtr->m_Mutex);
		while ( (!m_ThreadPoolPtr->m_ShutdownRequested)
				|| ( m_ThreadPoolPtr->m_ShutdownRequested && !m_ThreadPoolPtr->m_TaskQueue.empty()) )
		{
			m_ThreadPoolPtr->BusyThreads--;
			m_ThreadPoolPtr->m_ConditionVariable.wait(lock, [this]
			{
				return (this->m_ThreadPoolPtr->m_ShutdownRequested || !this->m_ThreadPoolPtr->m_TaskQueue.empty());
			});

			m_ThreadPoolPtr->BusyThreads++;

			if (!this->m_ThreadPoolPtr->m_TaskQueue.empty())
			{
				auto func = m_ThreadPoolPtr->m_TaskQueue.front();
				m_ThreadPoolPtr->m_TaskQueue.pop();

				lock.unlock();
				func();
				lock.lock();
			}
		}
	}
};

#endif//SCOOP_THREADWORKER_H
