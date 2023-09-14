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

// NOTE: Referenced from Dear ImGUI Thread pool implementation
// (1) https://github.com/ZenSepiol/Dear-ImGui-App-Framework/tree/main/src/lib/thread_pool
// (2) https://www.youtube.com/watch?v=6re5U82KwbY --> explanation

class ThreadWorker;

#include "ThreadWorker.h"

class ThreadPool
{
	friend class ThreadWorker;

public:
	int BusyThreads;

private:
	bool m_ShutdownRequested = false;

	mutable std::mutex m_Mutex; // mutable https://modoocode.com/253
	std::condition_variable m_ConditionVariable;
	std::vector<std::thread> m_Threads; // worker pool
	std::queue<std::function<void()>> m_TaskQueue; // queue of callable objects.

public:
	explicit ThreadPool(const int size)
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

	ThreadPool(ThreadPool&&) = delete;

	ThreadPool& operator=(const ThreadPool&) = delete;

	ThreadPool& operator=(ThreadPool&&) = delete;

	// Waits until threads finish their current task and shutdowns the pool
	void Shutdown()
	{
		{
			std::lock_guard<std::mutex> lock(m_Mutex);
			m_ShutdownRequested = true;
			m_ConditionVariable.notify_all(); // Unblocks all threads currently waiting for *this.
		}

		for (size_t i = 0; i < m_Threads.size(); ++i) {
			if (m_Threads[i].joinable()) {
				m_Threads[i].join();
			}
		}
	}

	template<typename F, typename... Args>
	auto AddTask(F&& f, Args&& ... args) -> std::future<decltype(f(args...))>
	{
		auto task_ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(std::forward<F>(f), std::forward<Args>(args)...);

		auto wrapper_func = [task_ptr]() { (*task_ptr)(); };
		{
			std::lock_guard<std::mutex> lock(m_Mutex);
			m_TaskQueue.push(wrapper_func);
			// Wake up one thread if its waiting
			m_ConditionVariable.notify_one();
		}

		// Return future from promise
		return task_ptr->get_future();
	}

	// 각각에 lock을 거는 이유는, 각 thread에서 threadpool에 접근하여 taskQueueSize를 얻어갈 수 있기 때문
	int GetTaskQueueSize() const
	{
		std::unique_lock<std::mutex> lock(m_Mutex);
		return m_TaskQueue.size();
	}


private:


};


#endif//SCOOP_THREADPOOL_H
