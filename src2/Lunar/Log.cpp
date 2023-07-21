//
// Created by USER on 2023-07-21.
//

#include "Log.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace Lunar {
	std::shared_ptr<spdlog::logger> Log::s_Logger; // 이거 안하면 터짐. s_Logger 변수 선언.
	// Initialize Log Setting for spdlog library
	void Log::Init()
	{
		// https://github.com/gabime/spdlog/wiki/3.-Custom-formatting
		spdlog::set_pattern("%^[%T] %n: %v%$");
		Log::s_Logger = spdlog::stdout_color_mt("Lunar");
		Log::s_Logger->set_level(spdlog::level::trace);
	}
}
