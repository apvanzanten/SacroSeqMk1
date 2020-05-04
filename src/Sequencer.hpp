#ifndef SEQUENCER_HPP
#define SEQUENCER_HPP

#include "sacroseq.hpp"
#include "util/CircularBuffer.hpp"
#include <algorithm>

namespace sseq {
  class Sequencer {
  public:
    enum class GateMode { 
      hold, pulseOnce, pulseRepeat, pulseOnceThenHold,
      MINIMUM = hold, MAXIMUM = pulseOnceThenHold
    };

    struct NoteMessage {
      midi::Note note = midi::Note::c4;
      std::uint8_t velocity = midi::VELOCITY_MAX;
    };

  private:
    static constexpr auto MIN_BPM = 1;
    static constexpr auto MAX_BPM = 500;

    enum class GateState {
      idle,
      holdOn,
      holdOff,
      pulseOnceOn,
      pulseOnceThenHoldOn,
      pulseOnceThenHoldOff,
      pulseRepeatOn,
      pulseRepeatOff
    } gateState;

    struct Step {
      GateMode gate = GateMode::hold;
      midi::Note note = midi::Note::c4;
      std::int8_t repetitions = 0;
      bool isActive = false;
    };

    std::array<Step, 8> steps;
    std::uint8_t stepIndex = 0;

    std::int32_t timeDelta = 0;
    std::int16_t bpm = 120;
    float gateFraction = 0.75;
    std::int32_t period = MAIN_CLOCK_FREQUENCY_PPM / bpm;
    std::int32_t gateTime = gateFraction * period;
    std::int8_t currentRepetition = 1;

    midi::Note noteCurrentlyPlaying = midi::Note::NONE;

    util::CircularBuffer<NoteMessage, 4> outputBuffer;

    inline std::uint8_t getVelocity() const {
      return (steps[stepIndex].isActive ? midi::VELOCITY_MAX : 0);
    }

    inline void noteOff(midi::Note note) {
      outputBuffer.push(NoteMessage{note, 0}); 
    }
    inline void noteOff() { 
      noteOff(noteCurrentlyPlaying); 
      noteCurrentlyPlaying = midi::Note::NONE;
    }

    inline void noteOn(midi::Note note) { 
      if(noteCurrentlyPlaying != midi::Note::NONE) {
        noteOff();
      }

      outputBuffer.push(NoteMessage{note, getVelocity()});
      noteCurrentlyPlaying = note;
    }


    inline std::int8_t getStepOffset() { return 1; }

    void moveToNextStep();
    void moveToNextRepetition();

    int numRepsLeft();
    
    bool isNextCycle();
    bool isGateTimePassed();

    void updateGateState();

    void updatePeriodAndGateTime();

  public:
    inline int getBpm() const { return bpm; }
    inline std::int32_t getGateTime() const { return gateTime; }
    inline midi::Note getNote(size_t index) const { return steps[index].note; }
    inline GateMode getGateMode(size_t index) const { return steps[index].gate; }
    inline std::int8_t getRepetitions(size_t index) const { return steps[index].repetitions; }
    inline bool getActivity(size_t index) const { return steps[index].isActive; }
    inline std::uint8_t getCurrentStep() const { return stepIndex; }

    inline void setBpm(int newBpm) {
      bpm = std::clamp(newBpm, MIN_BPM, MAX_BPM);
      updatePeriodAndGateTime();
    }

    inline void setBpmRelative(int offset) { setBpm(getBpm() + offset); }

    inline void setGateTime(float newGateFraction) {
      gateFraction = std::clamp(newGateFraction, 0.0f, 1.0f);
      gateTime = static_cast<std::int32_t>(gateFraction * period);
    }

    inline bool updateAndCheckBuffer() {
      ++timeDelta;

      updateGateState();
      return !(outputBuffer.isEmpty());
    }

    inline NoteMessage getNextNote() { return outputBuffer.pop(); }

    inline void setGateMode(size_t index, GateMode newVal) {
      steps[index].gate = newVal;
    }

    inline void setGateModeRelative(size_t index, std::int8_t offset){
      const auto oldVal = static_cast<std::int8_t>(steps[index].gate);
      const auto newVal = static_cast<GateMode>(oldVal + offset);

      steps[index].gate = std::clamp(newVal, GateMode::MINIMUM, GateMode::MAXIMUM);
    }

    inline void setNote(size_t index, midi::Note newVal) { steps[index].note = newVal; }

    inline void setNoteRelative(size_t index, int offset) {
      const auto old_val = static_cast<int>(steps[index].note);
      const auto new_val = static_cast<midi::Note>(old_val + offset);
      setNote(index, std::clamp(new_val, midi::Note::MINIMUM, midi::Note::MAXIMUM));
    }

    inline void setRepetitions(size_t index, std::int8_t newVal) {
      steps[index].repetitions = newVal;
    }

    inline void setRepetitionsRelative(size_t index, std::int8_t offset) {
      const auto new_val = steps[index].repetitions + offset;
      setRepetitions(index, std::clamp(new_val, 0, MAX_REPETITIONS));
    }

    inline void setActivity(size_t index, bool newVal) {
      steps[index].isActive = newVal;
    }
  };
} // namespace sseq

#endif // SEQUENCER_HPP
