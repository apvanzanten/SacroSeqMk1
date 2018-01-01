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
    std::uint32_t gate_time = period;
    std::uint8_t current_repetition = 1;

    volatile bool is_being_edited = false;

    util::circular_buffer<note_message, 4> output_buffer;

    inline void note_off(midi::note note) { output_buffer.push(note_message{note, 0}); }

    inline std::uint8_t get_velocity() { return (steps[step_index].is_active ? 127 : 0); }

    inline void note_on(midi::note note) {
      output_buffer.push(note_message{note, get_velocity()});
    }

    inline std::int8_t get_step_offset() { return 1; }

    void move_to_next_step();

    inline void move_to_next_repetition() {
      ++current_repetition;
      time_delta = 0;
    }

    void update_gate_state();

  public:
    inline void set_bpm(size_t new_bpm) { period = (60 * MAIN_CLOCK_FREQUENCY_HZ) / new_bpm; }

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

    inline note_message get_note() { return output_buffer.pop(); }

    inline void set_gate_mode(size_t index, gate_mode new_val) {
      is_being_edited = true;
      steps[index].gate = new_val;
      is_being_edited = false;
    }

    inline void set_note(size_t index, midi::note new_val) {
      is_being_edited = true;
      steps[index].note = new_val;
      is_being_edited = false;
    }

    inline void set_repetitions(size_t index, std::uint8_t new_val) {
      is_being_edited = true;
      steps[index].repetitions = new_val;
      is_being_edited = false;
    }

    inline void set_activity(size_t index, bool new_val) {
      is_being_edited = true;
      steps[index].is_active = new_val;
      is_being_edited = false;
    }
  };
} // namespace sseq

#endif // SEQUENCER_HPP
