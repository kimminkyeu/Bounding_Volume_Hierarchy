//
// Created by USER on 2023-07-21.
//

#ifndef SCOOP_LOG_H
#define SCOOP_LOG_H

#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

// How and Why Logging ??
// https://www.youtube.com/watch?v=dZr-53LAlOw&list=PLlrATfBNZ98dC-V-N3m0Go4deliWHPFwT&index=6

namespace Lunar {
	class Log
	{
	private:
		static std::shared_ptr<spdlog::logger> s_Logger; // create static instance.
	public:
		static void Init();
		inline static std::shared_ptr<spdlog::logger>& GetLogger()
		{ return s_Logger; }
	};
}

// WARN: distribution 버전의 경우 아래 매크로를 사용하지 않도록 할 것.
// Log macros (Variadic Macros)
// https://en.wikipedia.org/wiki/Variadic_macro_in_the_C_preprocessor
#if (DEBUG_MODE == true)
	#define LOG_TRACE(...)         ::Lunar::Log::GetLogger()->trace(__VA_ARGS__)
	#define LOG_INFO(...)          ::Lunar::Log::GetLogger()->info(__VA_ARGS__)
	#define LOG_WARN(...)          ::Lunar::Log::GetLogger()->warn(__VA_ARGS__)
	#define LOG_ERROR(...)         ::Lunar::Log::GetLogger()->error(__VA_ARGS__)
	#define LOG_CRITICAL(...)      ::Lunar::Log::GetLogger()->critical(__VA_ARGS__)
#else // Release mode
	#define LOG_TRACE 				// do nothing
	#define LOG_INFO 				// do nothing
	#define LOG_WARN 				// do nothing
	#define LOG_ERROR 				// do nothing
	#define LOG_CRITICAL 			// do nothing
#endif // DEBUG_MODE

#endif //SCOOP_LOG_H
