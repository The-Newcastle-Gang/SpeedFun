//
// Created by idhant on 16-02-2024.
//

#ifndef SPEEDFUN_GROUNDEDSTATE_H
#define SPEEDFUN_GROUNDEDSTATE_H

#include "State.h"
#include <iostream>
#include "PhysicsSystem.h"
#include "PhysicsObject.h"

namespace NCL{
    namespace CSC8503 {
        class GroundedState : public State{
        public:
            GroundedState(PhysicsObject* physObj);
            void OnEnter() override;
            void Update(float dt) override;
        protected:
            void MinimizePlayerSlide (PhysicsObject* phys);
            void ClampPlayerVelocity (PhysicsObject* phys);

            PhysicsObject* playerPhysObj;
            float maxVelocity;
            float groundDrag;
        };
    }
}



#endif //SPEEDFUN_GROUNDEDSTATE_H
