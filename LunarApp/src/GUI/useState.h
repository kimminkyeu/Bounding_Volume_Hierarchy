//
// Created by Minkyeu Kim on 9/27/23.
//

#ifndef SCOOP_USESTATE_H
#define SCOOP_USESTATE_H

// https://stackoverflow.com/questions/31387238/c-function-returning-functionc
#include <functional>

namespace React
{
    // const [state, setState] = React::UseState<int>();

	template <typename T>
	struct Hook
	{
		mutable T state = T(); // non-const
		std::function < void (const T&) > setState = {}; // const-qualified ?
	};

    template <typename T>
    class UseState
    {
    private:
        // https://en.cppreference.com/w/cpp/language/structured_binding

    private:
        static Hook<T> m_Hook;

    public:

        static const Hook<T>& operator() (const T& initialValue = T())
        {
            m_Hook.state = initialValue;

            m_Hook.setState = [&](T value) -> void
            {
                m_Hook.state = value;
            };

            return m_Hook;
        }
    };
}

void test()
{
    const React::Hook<int>& s = React::UseState<int>::operator()(3);
//	Class s = s.function(3);
//    const React::Hook<int>& h = s();
}




#endif //SCOOP_USESTATE_H
