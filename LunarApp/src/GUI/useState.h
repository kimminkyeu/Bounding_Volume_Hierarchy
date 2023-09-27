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
    class UseState
    {
    private:
        // https://en.cppreference.com/w/cpp/language/structured_binding
        struct Hook
        {
            mutable T state = T(); // non-const
            volatile std::function<void()> setState = {}; // const-qualified
        };

    private:
        static Hook m_Hook;

    public:

        const Hook& operator() (const T& initialValue = T())
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
    auto s = React::UseState<int> ();
    auto p = s();
}




#endif //SCOOP_USESTATE_H
