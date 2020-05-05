#ifndef HOLDONSTATEMACHINE_HPP
#define HOLDONSTATEMACHINE_HPP

#include "common.hpp"

// state machine that holds the gate on for the full duration (initial pulse + all reps)

namespace sseq {
  namespace GateSM {
    enum class HoldOnStates {
      off,
      on,
      NUM_STATES,
      INITIAL = off,
    };

    using HoldOnBase = susml::StateMachine<HoldOnStates, Events>;

    class HoldOn : public HoldOnBase {
      using States = GateSM::HoldOnStates;

      Actions actions;
      Guards guards;

    public:
      HoldOn(Actions actions, Guards guards)
          : HoldOnBase(TransitionMatrix{
                TransitionList{
                    // transitions from state off
                    {Events::nextCycle, {}, {actions.resetReps, actions.noteOn}, States::on},
                },
                TransitionList{
                    // transitions from state on
                    {Events::nextCycle, {guards.isRepsLeft}, {actions.decrementReps}, States::on},
                    {Events::gateTimeOver,
                     {guards.isDoneWithReps},
                     {actions.noteOff},
                     States::off}}}),
            actions(actions), guards(guards) {}
    };
  } // namespace GateSM
} // namespace sseq

#endif
