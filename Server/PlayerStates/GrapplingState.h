//
// Created by idhant on 16-02-2024.
//

#ifndef SPEEDFUN_GRAPPLINGSTATE_H
#define SPEEDFUN_GRAPPLINGSTATE_H

#include "State.h"
#include <iostream>
#include "PhysicsSystem.h"
#include "PhysicsObject.h"

namespace NCL{
    namespace CSC8503 {
        class GrapplingState : public State{
        public:
            GrapplingState(PhysicsObject* physObj);
            void Update(float dt) override;
        protected:
            bool HasGrappleCompleted();

        };
    }
}

#endif //SPEEDFUN_GRAPPLINGSTATE_H
