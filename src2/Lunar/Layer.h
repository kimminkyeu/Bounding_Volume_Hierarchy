//
// Created by Minkyeu Kim on 7/21/23.
//

#ifndef SCOOP_LAYER_H
#define SCOOP_LAYER_H

// Reference of Code
// https://github.com/StudioCherno/Walnut/blob/3b8e414fdecfc6c8b58816106fe8d912bd172e31/Walnut/src/Walnut/Layer.h

namespace Lunar {
	class Layer
	{
	public:
		virtual ~Layer() = default;

		virtual void OnAttach() = 0;
		virtual void OnDetach() = 0;

		virtual void OnUpdate(float ts) = 0;
		virtual void OnUIRender() = 0;
	};
}

#endif //SCOOP_LAYER_H
