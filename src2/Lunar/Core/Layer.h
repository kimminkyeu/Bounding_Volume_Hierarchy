//
// Created by Minkyeu Kim on 7/21/23.
//

#ifndef SCOOP_LAYER_H
#define SCOOP_LAYER_H

#include "Log.h"
// Reference of Code
// https://github.com/StudioCherno/Walnut/blob/3b8e414fdecfc6c8b58816106fe8d912bd172e31/Walnut/src/Walnut/Layer.h
// What is Layer? why?
// https://youtu.be/_Kj6BSfM6P4

namespace Lunar {
	class Layer
	{
	public:
		virtual ~Layer() = default;

		virtual void OnAttach()
        {  // Default action if [derived Layer] does not implement OnAttach function.
            LOG_TRACE(" --------- New Layer has been attached");
        };

		virtual void OnDetach()
        {  // Default action if [derived Layer] does not implement OnDetach function.
            LOG_TRACE(" --------- New Layer has been detached");
        };

		virtual void OnUpdate(float ts)
        {};

		virtual void OnUIRender()
        {};
	};
}

#endif //SCOOP_LAYER_H
