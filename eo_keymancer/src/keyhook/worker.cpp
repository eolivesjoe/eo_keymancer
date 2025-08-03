#include "worker.h"

namespace key_hook
{
	std::queue<INPUT> Worker::m_queue;
	std::mutex Worker::m_mutex;
	std::condition_variable Worker::m_cv;
	std::atomic<bool> Worker::m_running = false;
	std::thread Worker::m_thread;

	void Worker::Start()
	{
		m_running = true;
		m_thread = std::thread(InputWorker);
	}

	void Worker::Stop()
	{
		m_running = false;
		m_cv.notify_all();

		if (m_thread.joinable())
		{
			m_thread.join();
		}
	}

	void Worker::QueueInput(const INPUT& input)
	{
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			m_queue.push(input);
		}
		m_cv.notify_one();
	}

	void Worker::InputWorker()
	{
		while (m_running)
		{
			INPUT input;
			bool hasInput = false;

			{
				std::unique_lock<std::mutex> lock(m_mutex);
				m_cv.wait(lock, [] {
					return !m_queue.empty() || !m_running;
					});

				if (!m_queue.empty())
				{
					input = m_queue.front();
					m_queue.pop();
					hasInput = true;
				}

				if (!hasInput && !m_running)
					return;
			}

			if (hasInput)
			{
				SendInput(1, &input, sizeof(INPUT));
			}
		}
	}
}
