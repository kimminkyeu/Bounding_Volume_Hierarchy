//
// Created by Minkyeu Kim on 7/24/23.
//

#ifndef SCOOP_MOUSECODES_H
#define SCOOP_MOUSECODES_H

namespace Lunar
{
	using MouseCode = uint16_t;

	namespace Mouse
	{
		enum : MouseCode
		{
			// From glfw3.h
			Button0                = 0,
			Button1                = 1,
			Button2                = 2,
			Button3                = 3,
			Button4                = 4,
			Button5                = 5,
			Button6                = 6,
			Button7                = 7,

			ButtonLast             = Button7,
			ButtonLeft             = Button0,
			ButtonRight            = Button1,
			ButtonMiddle           = Button2
		};
	}
}

#endif //SCOOP_MOUSECODES_H
