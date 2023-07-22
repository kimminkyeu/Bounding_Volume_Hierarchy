//
// Created by USER on 2023-07-22.
//

#include "Lunar/Application.h"
#include "Lunar/EntryPoint.h" // main code here

Lunar::Application* Lunar::CreateApplication(int argc, char** argv) noexcept
{
	Lunar::ApplicationSpecification spec;
	spec.Name = "Scoop";

	auto* app = new Lunar::Application(spec);

	return (app);
}