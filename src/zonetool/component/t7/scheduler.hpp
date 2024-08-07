#pragma once

namespace t7
{
	namespace scheduler
	{
		enum pipeline
		{
			// Asynchronuous pipeline, disconnected from the game
			async = 0,

			// The game's main thread
			main,

			count,
		};

		static const bool cond_continue = false;
		static const bool cond_end = true;

		void schedule(const std::function<bool()>& callback, pipeline type = pipeline::async,
			std::chrono::milliseconds delay = 0ms);
		void loop(const std::function<void()>& callback, pipeline type = pipeline::async,
			std::chrono::milliseconds delay = 0ms);
		void once(const std::function<void()>& callback, pipeline type = pipeline::async,
			std::chrono::milliseconds delay = 0ms);
	}
}