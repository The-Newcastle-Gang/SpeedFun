#pragma once
#include "State.h"
#include <iostream>

namespace NCL {
    namespace CSC8503 {
        class BusyState : public State
        {
        public:
            BusyState();
            ~BusyState();
            void Update(float dt);

            void OnEnter() {}
            void OnExit() {}
        protected:

        };
    }
}