//
// Created by USER on 2023-07-21.
//

#ifndef SCOOP_LOG_H
#define SCOOP_LOG_H

#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"
#include "spdlog/sinks/stdout_color_sinks.h"

// How and Why Logging ??
// https://www.youtube.com/watch?v=dZr-53LAlOw&list=PLlrATfBNZ98dC-V-N3m0Go4deliWHPFwT&index=6

namespace Lunar {
	class Log
	{
	private:
		Log() 							= delete;
		Log(const Log& ref) 			= delete;
		Log& operator=(const Log& ref) 	= delete;
		~Log() 							= delete;

		// WARN! static member의 경우 cpp에 별도 선언이 필요했으나, C++17부터 inline 키워드를 붙여 cpp 별도 선언 없어도
		// 링크가 되도록 함. // 참고: https://stackoverflow.com/questions/9110487/undefined-reference-to-a-static-member
		inline static std::shared_ptr<spdlog::logger> s_Logger;
		// 왜냐면 객체가 생성되지 않았기 때문에, 해당 객체를 헤더가 아닌 CPP 파일에서 instance화 해주는 과정이 필요함...
		// 기존 --> Log.cpp에 별도의 선언 추가해서 개별 변수 instance를 만들었어야 했음. (혹은 class 바깥에서)

	public:
		static std::shared_ptr<spdlog::logger>& GetLogger()
		{
			if (Log::s_Logger == nullptr)
			{
				spdlog::set_pattern("%^[%T] %n: %v%$");
				Log::s_Logger = spdlog::stdout_color_mt("Lunar");
				Log::s_Logger->set_level(spdlog::level::trace);
			}
			return s_Logger;
		}
	};
}

// WARN: distribution 버전의 경우 아래 매크로를 사용하지 않도록 할 것.
// Log macros (Variadic Macros)
// https://en.wikipedia.org/wiki/Variadic_macro_in_the_C_preprocessor
# if (DEBUG_MODE == true)
	#define LOG_TRACE(...)         ::Lunar::Log::GetLogger()->trace(__VA_ARGS__)
	#define LOG_INFO(...)          ::Lunar::Log::GetLogger()->info(__VA_ARGS__)
	#define LOG_WARN(...)          ::Lunar::Log::GetLogger()->warn(__VA_ARGS__)
	#define LOG_ERROR(...)         ::Lunar::Log::GetLogger()->error(__VA_ARGS__)
	#define LOG_CRITICAL(...)      ::Lunar::Log::GetLogger()->critical(__VA_ARGS__)
# else // Release mode
	#define LOG_TRACE 				// do nothing
	#define LOG_INFO 				// do nothing
	#define LOG_WARN 				// do nothing
	#define LOG_ERROR 				// do nothing
	#define LOG_CRITICAL 			// do nothing
# endif // DEBUG_MODE

#endif //SCOOP_LOG_H
