#include "util/StateMachine.hpp"

#include <iostream>

namespace Guards {
bool unitGuard() {
  std::cout << "called " << __func__ << std::endl;
  return true;
}
} // namespace Guards

namespace Actions {
void unitAction() { std::cout << "called " << __func__ << std::endl; }
} // namespace Actions

enum class States {
  idle,
  on,
  NUM_STATES,
  INITIAL = idle,
};

enum class Events { nextCycle, gateTimeOver };

using StateMachine = sseq::util::StateMachine<States, Events>;
using Transition = StateMachine::Transition;
using TransitionList = StateMachine::TransitionList;
using TransitionMatrix = StateMachine::TransitionMatrix;

StateMachine m{TransitionMatrix{
    TransitionList{
        // transitions starting at state idle
        {
            Events::nextCycle,   // transition in response to nextCycle event
            {Guards::unitGuard}, // transition only if unitGuard return true
            {Actions::unitAction,
             Actions::unitAction}, // on transition, call unitAction twice
            States::on             // transition to state on
        },
    },
    TransitionList{
        // transitions starting at state on
        {
            Events::gateTimeOver,  // transition in response to gateTimeOver
                                   // event
            {Guards::unitGuard},   // transition only if unitGuard returns true
            {Actions::unitAction}, // on transition, call unitAction
            States::idle           // transition to state idle
        },
        {
            Events::nextCycle,     // transition in response to nextCycle event
            {},                    // transition always
            {Actions::unitAction}, // on transition, call unitAction
            States::on             // transition to state on
        }}}};

int main() {
  std::cout << "StateMachineTest start!" << std::endl;

  std::cout << "starting at state: " << static_cast<int>(m.getState())
            << std::endl;
  std::cout << std::endl;

  std::cout << "triggering Events::nextCycle at state #"
            << static_cast<int>(m.getState()) << std::endl;
  m.trigger(Events::nextCycle);
  std::cout << "after event at state #" << static_cast<int>(m.getState())
            << std::endl;
  std::cout << std::endl;

  std::cout << "triggering Events::nextCycle at state #"
            << static_cast<int>(m.getState()) << std::endl;
  m.trigger(Events::nextCycle);
  std::cout << "after event at state #" << static_cast<int>(m.getState())
            << std::endl;
  std::cout << std::endl;

  std::cout << "triggering Events::gateTimeOver at state #"
            << static_cast<int>(m.getState()) << std::endl;
  m.trigger(Events::gateTimeOver);
  std::cout << "after event at state #" << static_cast<int>(m.getState())
            << std::endl;
  std::cout << std::endl;

  std::cout << "triggering Events::gateTimeOver at state #"
            << static_cast<int>(m.getState()) << std::endl;
  m.trigger(Events::gateTimeOver);
  std::cout << "after event at state #" << static_cast<int>(m.getState())
            << std::endl;
  std::cout << std::endl;

  return 0;
}