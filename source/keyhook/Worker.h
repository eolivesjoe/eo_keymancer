#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>
#include <windows.h>

namespace keyhook
{
	class Worker
	{
	public:
		static void Start();
		static void Stop();
		static void QueueInput(const INPUT& input);

	private:
		static void InputWorker();

		static std::queue<INPUT> m_queue;
		static std::mutex m_mutex;
		static std::condition_variable m_cv;
		static std::atomic<bool> m_running;
		static std::thread m_thread;
	};
} // namespace keyhook
