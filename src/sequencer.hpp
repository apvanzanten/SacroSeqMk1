#ifndef SEQUENCER_HPP
#define SEQUENCER_HPP

#include "sacroseq.hpp"
#include "util/circular_buffer.hpp"

namespace sseq {
  class sequencer {
  public:
    enum class gate_mode { hold, pulse_once, pulse_repeat, pulse_once_then_hold };

    enum class probability_mode { uniform, bell, left_bell, right_bell };

    struct note_message {
      midi::note note = midi::note::c4;
      std::uint8_t velocity = 127;
    };

  private:
    enum class gate_states {
      initial,
      hold_on,
      hold_off,
      pulse_once_on,
      pulse_once_then_hold_on,
      pulse_once_then_hold_off,
      pulse_repeat_on,
      pulse_repeat_off
    } gate_state;

    struct step {
      gate_mode gate = gate_mode::hold;
      midi::note note = midi::note::c4;
      std::uint8_t repetitions = 0;
      bool is_active = false;
    };

    std::array<step, 8> steps;
    size_t step_index = 0;

    std::uint32_t time_delta = 0;
    std::uint32_t period = (60 * MAIN_CLOCK_FREQUENCY_HZ) / 120;
    std::uint32_t gate_time = 0.75 * period;
    std::uint8_t current_repetition = 1;

    volatile bool is_being_edited = false;

    util::circular_buffer<note_message, 4> output_buffer;

    inline std::uint8_t get_velocity() const { return (steps[step_index].is_active ? 127 : 0); }

    inline void note_off(midi::note note) {output_buffer.push(note_message{note, 0}); }

    inline void note_on(midi::note note) { output_buffer.push(note_message{note, get_velocity()}); }


    inline std::int8_t get_step_offset() { return 1; }

    void move_to_next_step();

    inline void move_to_next_repetition() {
      ++current_repetition;
      time_delta = 0;
    }

    void update_gate_state();

  public:
    inline size_t get_bpm() const { return (60 * MAIN_CLOCK_FREQUENCY_HZ) / period; }
    inline std::uint32_t get_gate_time() const { return gate_time; }
    inline midi::note get_note(size_t index) const { return steps[index].note; }
    inline gate_mode get_gate_mode(size_t index) const { return steps[index].gate; }
    inline std::uint32_t get_repetitions(size_t index) const { return steps[index].repetitions; }
    inline bool get_activity(size_t index) const { return steps[index].is_active; }
    inline size_t get_current_step() const { return step_index; }

    inline void set_bpm(size_t new_bpm) { 
      auto new_period = (60 * MAIN_CLOCK_FREQUENCY_HZ) / new_bpm; 
      float gate_fraction =  static_cast<float>(gate_time) / static_cast<float>(period);

      // TODO come up with better solution to race condition problem.
      if(new_period < period){
        gate_time = static_cast<std::uint32_t>(gate_fraction * new_period);
        period = new_period;
      } else {
        period = new_period;
        gate_time = static_cast<std::uint32_t>(gate_fraction * new_period);
      }
    }

    inline void set_bpm_relative(int offset) { set_bpm(get_bpm() + offset); }

    inline void set_gate_time(float new_time_fraction) {
      gate_time = static_cast<std::uint32_t>(new_time_fraction * period);
    }

    inline bool update_and_check_buffer() {
      ++time_delta;

      if (is_being_edited) {
        return false;
      }

      update_gate_state();
      return !(output_buffer.empty());
    }

    inline note_message get_next_note() { return output_buffer.pop(); }

    inline void set_gate_mode(size_t index, gate_mode new_val) {
      is_being_edited = true;
      steps[index].gate = new_val;
      is_being_edited = false;
    }

    inline void set_gate_mode_relative(size_t index, int offset){
      const int old_val = static_cast<int>(steps[index].gate);
      if (offset > 0 && steps[index].gate != gate_mode::pulse_once_then_hold){
        steps[index].gate = static_cast<gate_mode>(old_val + 1);
      } else if(offset < 0 && steps[index].gate != gate_mode::hold){
        steps[index].gate = static_cast<gate_mode>(old_val - 1);
      }
    }


    inline void set_note(size_t index, midi::note new_val) {
      is_being_edited = true;
      steps[index].note = new_val;
      is_being_edited = false;
    }

    inline void set_note_relative(size_t index, int offset) {
      int new_val = static_cast<int>(steps[index].note);
      new_val += offset;

      // TODO replace this with a proper clamp
      if (new_val < static_cast<int>(midi::note::MINIMUM)) {
        set_note(index, midi::note::MINIMUM);
      } else if (new_val > static_cast<int>(midi::note::MAXIMUM)) {
        set_note(index, midi::note::MAXIMUM);
      } else {
        set_note(index, static_cast<midi::note>(new_val));
      }
    }

    inline void set_repetitions(size_t index, std::uint8_t new_val) {
      is_being_edited = true;
      steps[index].repetitions = new_val;
      is_being_edited = false;
    }

    inline void set_repetitions_relative(size_t index, int offset) {
      auto new_val = steps[index].repetitions + offset;
      if(new_val < 0){
        set_repetitions(index, 0);
      } else if(new_val > MAX_REPETITIONS) {
        set_repetitions(index, MAX_REPETITIONS);
      } else {
        set_repetitions(index, new_val);
      }
    }

    inline void set_activity(size_t index, bool new_val) {
      is_being_edited = true;
      steps[index].is_active = new_val;
      is_being_edited = false;
    }
  };
} // namespace sseq

#endif // SEQUENCER_HPP
