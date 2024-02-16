//
// Created by idhant on 16-02-2024.
//
#ifndef SPEEDFUN_JUMPINGSTATE_H
#define SPEEDFUN_JUMPINGSTATE_H

#include "State.h"
#include <iostream>
#include <PhysicsSystem.h>
#include <PhysicsObject.h>


namespace NCL{
    namespace CSC8503{
        class JumpingState : public State{

        public:
            JumpingState(PhysicsObject* physObj);
            void OnEnter() override;
            void Update(float dt) override;

        protected:
            void FastFalling(PhysicsObject* physObj);

            float fastFallingMultiplier;
            PhysicsObject* playerPhysObj;

        };
    }
}



#endif //SPEEDFUN_JUMPINGSTATE_H
