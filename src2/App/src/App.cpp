
#include "Lunar/Core/Application.h"
#include "Lunar/Core/EntryPoint.h" // main code here

class ExampleLayer : public Lunar::Layer
{
public:
    void OnUIRender() override
    {
        // ....
    }

    void OnUpdate(float ts) override
    {
        // ....
    }
    // OnAttach (LayerStack)
    // OnDetach (LayerStack)
};

Lunar::Application* Lunar::CreateApplication(int argc, char** argv) noexcept
{
	Lunar::ApplicationSpecification spec;
	spec.Name = "Scoop";

	auto* app = new Lunar::Application(spec);
    app->PushLayer<ExampleLayer>();
	return (app);
}