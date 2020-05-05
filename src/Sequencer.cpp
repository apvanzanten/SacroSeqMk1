#include "Sequencer.hpp"
#include "GateSM/HoldOn.hpp"

namespace sseq {
  void Sequencer::updatePeriodAndGateTime() {
    const std::int32_t newPeriod = MAIN_CLOCK_FREQUENCY_PPM / bpm;
    const auto newGateTime = static_cast<int>(gateFraction * newPeriod);

    // NOTE gateTime must be set before period to prevent case in which gateTime > period.
    gateTime = newGateTime;
    period = newPeriod;
  }

  void Sequencer::moveToNextStep() {
    int newStepIndexSignedUnbounded = stepIndex + getStepOffset();

    while (newStepIndexSignedUnbounded < 0) {
      newStepIndexSignedUnbounded += steps.size();
    }
    size_t newStepIndexUnbounded = static_cast<size_t>(newStepIndexSignedUnbounded);
    while (newStepIndexUnbounded >= steps.size()) {
      newStepIndexUnbounded -= steps.size();
    }
    stepIndex = newStepIndexUnbounded;
    currentRepetition = 0;
  }

  void Sequencer::moveToNextRepetition() {
    ++currentRepetition;
    timeDelta = 0;
  }

  bool Sequencer::isNextCycle() {
    return timeDelta >= period;
  }

  int Sequencer::numRepsLeft() {
    return steps[stepIndex].repetitions - currentRepetition;
  }

  bool Sequencer::isGateTimePassed() {
    return timeDelta >= gateTime;  
  }

  void Sequencer::updateGateState() {
    switch (gateState) {
    case GateState::idle:
      if (isNextCycle()) {
        timeDelta = 0;

        noteOn(steps[stepIndex].note);

        switch (steps[stepIndex].gate) {
        case GateMode::hold:
          gateState = GateState::holdOn;
          break;
        case GateMode::pulseOnce:
          gateState = GateState::pulseOnceOn;
          break;
        case GateMode::pulseOnceThenHold:
          gateState = GateState::pulseOnceThenHoldOn;
          break;
        case GateMode::pulseRepeat:
          gateState = GateState::pulseRepeatOn;
          break;
        default: // panic!
          alerts::error();
        }
      }
      break;
    case GateState::holdOn:
      if (isGateTimePassed() && numRepsLeft() <= 0) {
        noteOff();
        moveToNextStep();
        gateState = GateState::idle;
      } else if (isNextCycle()) {
        moveToNextRepetition();
      }
      break;
    case GateState::pulseOnceOn:
      if (isGateTimePassed()) {
        noteOff();
        gateState = GateState::holdOff;
      }
      break;
    case GateState::holdOff:
      if (isNextCycle()) {
        if (numRepsLeft() <= 0) {
          moveToNextStep();
          gateState = GateState::idle;
        } else {
          moveToNextRepetition();
        }
      }
      break;
    case GateState::pulseOnceThenHoldOn:
      if (isGateTimePassed()) {
        noteOff();
        gateState = GateState::pulseOnceThenHoldOff;
      }
      break;
    case GateState::pulseOnceThenHoldOff:
      if (isNextCycle()) {
        if(numRepsLeft() > 0)
        {
          noteOn(steps[stepIndex].note);
          moveToNextRepetition();
          gateState = GateState::holdOn;
        } else {
          moveToNextStep();
          gateState = GateState::idle;
        }
      }
      break;
    case GateState::pulseRepeatOn:
      if (isGateTimePassed()) {
        noteOff();
        gateState = GateState::pulseRepeatOff;
      }
      break;
    case GateState::pulseRepeatOff:
      if (isNextCycle()) {
        if (numRepsLeft() <= 0) {
          moveToNextStep();
          gateState = GateState::idle;
        } else {
          moveToNextRepetition();
          noteOn(steps[stepIndex].note);
          gateState = GateState::pulseRepeatOn;
        }
      }
      break;
    default: // panic!
      alerts::error();
    }
  }

} // namespace sseq
