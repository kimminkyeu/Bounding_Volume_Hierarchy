//
// Created by Minkyeu Kim on 9/14/23.
//

#ifndef SCOOP_THREADWORKER_H
#define SCOOP_THREADWORKER_H

#include "ThreadPool.h"

class ThreadWorker
{
//	friend class ThreadPool;

public:
	ThreadWorker() = delete;

	explicit ThreadWorker(ThreadPool* pool)
		: thread_pool(pool)
	{}

	void operator()()
	{
		std::unique_lock<std::mutex> lock(thread_pool->m_Mutex);
		while (!thread_pool->m_ShutdownRequested || (thread_pool->m_ShutdownRequested && !thread_pool->m_Queue.empty()))
		{
			thread_pool->BusyThreads--;
			thread_pool->m_ConditionVariable.wait(lock, [this] {
				return (this->thread_pool->m_ShutdownRequested || !this->thread_pool->m_Queue.empty());
			});
			thread_pool->BusyThreads++;

			if (!this->thread_pool->m_Queue.empty())
			{
				auto func = thread_pool->m_Queue.front();
				thread_pool->m_Queue.pop();

				lock.unlock();
				func();
				lock.lock();
			}
		}
	}
private:
	ThreadPool* thread_pool;
};

#endif//SCOOP_THREADWORKER_H
