#include "worker.h"

namespace keyHook
{
	std::queue<INPUT> Worker::queue;
	std::mutex Worker::m;
	std::condition_variable Worker::cv;
	std::atomic<bool> Worker::running = false;
	std::thread Worker::t;

	void Worker::start()
	{
		running = true;
		t = std::thread(inputWorker);
	}

	void Worker::stop()
	{
		running = false;
		cv.notify_all();

		if (t.joinable())
		{
			t.join();
		}
	}

	void Worker::queueInput(const INPUT& input)
	{
		{
			std::lock_guard<std::mutex> lock(m);
			queue.push(input);
		}
		cv.notify_one();
	}

	void Worker::inputWorker()
	{
		while (running)
		{
			INPUT input;
			bool has_input = false;

			{
				std::unique_lock<std::mutex> lock(m);
				cv.wait(lock, [] {
					return !queue.empty() || !running;
					});

				if (!queue.empty())
				{
					input = queue.front();
					queue.pop();
					has_input = true;
				}

				if (!has_input && !running)
					return;
			}

			if (has_input)
			{
				SendInput(1, &input, sizeof(INPUT));
			}
		}
	}
}
