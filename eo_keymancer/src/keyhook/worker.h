#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>
#include <Windows.h>

namespace keyHook
{
	class Worker
	{
	public:
		static void start();
		static void stop();
		static void queueInput(const INPUT& input);

	private:
		static void inputWorker();

		static std::queue<INPUT> queue;
		static std::mutex m;
		static std::condition_variable cv;
		static std::atomic<bool> running;
		static std::thread t;
	};
}
