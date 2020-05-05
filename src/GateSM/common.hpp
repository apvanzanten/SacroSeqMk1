#ifndef GATESTATEMACHINECOMMON_HPP
#define GATESTATEMACHINECOMMON_HPP

#include <susml.hpp>

namespace sseq::GateSM {
  struct Actions {
    const susml::Action decrementReps, resetReps, noteOn, noteOff;
  };
  struct Guards {
    const susml::Guard isRepsLeft, isDoneWithReps;
  };
  enum class Events { nextCycle, gateTimeOver };
} // namespace sseq::GateSM

#endif
