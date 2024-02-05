#include "StateMachine.h"
#include "State.h"
#include "StateTransition.h"

using namespace NCL::CSC8503;

StateMachine::StateMachine()	{
	activeState = nullptr;
}

StateMachine::~StateMachine()	{
	for (auto& i : allStates) {
		delete i;
	}
	for (auto& i : allTransitions) {
		delete i.second;
	}
}

void StateMachine::AddState(State* s) {
	allStates.emplace_back(s);
	if (activeState == nullptr) {
		activeState = s;
        activeState->OnEnter();
	}
}

void StateMachine::Transition(State *destinationState) {
    State* newState = destinationState;
    activeState->OnExit();
    activeState = newState;
    newState->OnEnter();
}

void StateMachine::AddTransition(StateTransition* t) {
	allTransitions.insert(std::make_pair(t->GetSourceState(), t));
}

void StateMachine::Update(float dt) {
	if (activeState) {
		activeState->Update(dt);

		std::pair<TransitionIterator, TransitionIterator> range = allTransitions.equal_range(activeState);

		for (auto& i = range.first; i != range.second; ++i) {
			if (i->second->CanTransition()) {
                Transition(i->second->GetDestinationState());
			}
		}
	}
}

void StateMachine::ReceivePacket(int type, GamePacket *payload, int source) {
    if (activeState) {
        activeState->ReceivePacket(type, payload, source);
        return;
    }
    std::cerr << "No state to handle packet" << std::endl;
}