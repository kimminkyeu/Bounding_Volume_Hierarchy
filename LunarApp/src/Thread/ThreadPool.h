//
// Created by Minkyeu Kim on 9/14/23.
//

#ifndef SCOOP_THREADPOOL_H
#define SCOOP_THREADPOOL_H

#include <functional> // https://modoocode.com/254
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
#include <vector>

// Reference : ImGUI Thread pool implementation
// (1) https://github.com/ZenSepiol/Dear-ImGui-App-Framework/tree/main/src/lib/thread_pool
// (2) https://www.youtube.com/watch?v=6re5U82KwbY --> explanation
#include "ThreadWorker.h"

class ThreadWorker;

class ThreadPool
{
	friend class ThreadWorker;

public:
	int BusyThreads;

private:
	mutable std::mutex m_Mutex; // mutable https://modoocode.com/253
	std::condition_variable m_ConditionVariable;

	std::vector<std::thread> m_Threads;
	bool m_ShutdownRequested = false;
	std::queue<std::function<void()>> m_Queue;

public:
	ThreadPool(const int size)
		: BusyThreads(size), m_Threads(size)
	{
		for (size_t i = 0; i < size; ++i)
		{
			m_Threads[i] = std::thread(ThreadWorker(this));
		}
	}

	~ThreadPool()
	{
		Shutdown();
	}

	ThreadPool(const ThreadPool&) = delete;
	ThreadPool(ThreadPool&&)      = delete;
	ThreadPool& operator=(const ThreadPool&) = delete;
	ThreadPool& operator=(ThreadPool&&)      = delete;

	// Waits until threads finish their current task and shutdowns the pool
	void Shutdown()
	{
		{
			std::lock_guard<std::mutex> lock(m_Mutex);
			m_ShutdownRequested = true;
			m_ConditionVariable.notify_all(); // Unblocks all threads currently waiting for *this.
		}

		for (size_t i = 0; i < m_Threads.size(); ++i)
		{
			if (m_Threads[i].joinable())
			{
				m_Threads[i].join();
			}
		}
	}

private:


};


#endif//SCOOP_THREADPOOL_H
