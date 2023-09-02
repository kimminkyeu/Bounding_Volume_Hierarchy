//
// Created by USER on 2023-07-22.
//

#ifndef SCOOP_ENTRYPOINT_CPP
#define SCOOP_ENTRYPOINT_CPP

#include "Application.h"
#include "Log.h"

bool g_ApplicationRunning = true;

// Main Wrapping 을 통해 program 재시작 기능을 가능케 함.
namespace Lunar {
	int Main(int argc, char** argv)
	{
		while (g_ApplicationRunning)
		{
			Lunar::Application* app = Lunar::CreateApplication(argc, argv);
			app->Run(); // NOTE: App will restart unless g_Application is set to false.
			delete app;
		}
		return (0);
	}
}

int main(int argc, char** argv)
{
	return Lunar::Main(argc, argv);
}

#endif //SCOOP_ENTRYPOINT_CPP
