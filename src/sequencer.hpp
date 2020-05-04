#ifndef SEQUENCER_HPP
#define SEQUENCER_HPP

#include "sacroseq.hpp"
#include "util/CircularBuffer.hpp"
#include <algorithm>

namespace sseq {
  class sequencer {
  public:
    enum class gate_mode { 
      hold, pulse_once, pulse_repeat, pulse_once_then_hold,
      MINIMUM = hold, MAXIMUM = pulse_once_then_hold
    };

    struct note_message {
      midi::note note = midi::note::c4;
      std::uint8_t velocity = midi::VELOCITY_MAX;
    };

  private:
    static constexpr auto MIN_BPM = 1;
    static constexpr auto MAX_BPM = 500;

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
      std::int8_t repetitions = 0;
      bool is_active = false;
    };

    std::array<step, 8> steps;
    std::uint8_t step_index = 0;

    std::int32_t time_delta = 0;
    std::int16_t bpm = 120;
    float gate_fraction = 0.75;
    std::int32_t period = MAIN_CLOCK_FREQUENCY_PPM / bpm;
    std::int32_t gate_time = gate_fraction * period;
    std::int8_t current_repetition = 1;

    midi::note currently_playing = midi::note::NONE;

    util::CircularBuffer<note_message, 4> output_buffer;

    inline std::uint8_t get_velocity() const {
      return (steps[step_index].is_active ? midi::VELOCITY_MAX : 0);
    }

    inline void note_off(midi::note note) {
      output_buffer.push(note_message{note, 0}); 
    }
    inline void note_off() { 
      note_off(currently_playing); 
      currently_playing = midi::note::NONE;
    }

    inline void note_on(midi::note note) { 
      if(currently_playing != midi::note::NONE) {
        note_off();
      }

      output_buffer.push(note_message{note, get_velocity()});
      currently_playing = note;
    }


    inline std::int8_t get_step_offset() { return 1; }

    void move_to_next_step();
    void move_to_next_repetition();

    int num_reps_left();
    
    bool is_next_cycle();
    bool is_gate_time_passed();

    void update_gate_state();

    void update_period_and_gate_time();

  public:
    inline int get_bpm() const { return bpm; }
    inline std::int32_t get_gate_time() const { return gate_time; }
    inline midi::note get_note(size_t index) const { return steps[index].note; }
    inline gate_mode get_gate_mode(size_t index) const { return steps[index].gate; }
    inline std::int8_t get_repetitions(size_t index) const { return steps[index].repetitions; }
    inline bool get_activity(size_t index) const { return steps[index].is_active; }
    inline std::uint8_t get_current_step() const { return step_index; }

    inline void set_bpm(int new_bpm) {
      bpm = std::clamp(new_bpm, MIN_BPM, MAX_BPM);
      update_period_and_gate_time();
    }

    inline void set_bpm_relative(int offset) { set_bpm(get_bpm() + offset); }

    inline void set_gate_time(float new_time_fraction) {
      gate_fraction = std::clamp(new_time_fraction, 0.0f, 1.0f);
      gate_time = static_cast<std::int32_t>(gate_fraction * period);
    }

    inline bool update_and_check_buffer() {
      ++time_delta;

      update_gate_state();
      return !(output_buffer.isEmpty());
    }

    inline note_message get_next_note() { return output_buffer.pop(); }

    inline void set_gate_mode(size_t index, gate_mode new_val) {
      steps[index].gate = new_val;
    }

    inline void set_gate_mode_relative(size_t index, std::int8_t offset){
      const auto old_val = static_cast<std::int8_t>(steps[index].gate);
      const auto new_val = static_cast<gate_mode>(old_val + offset);

      steps[index].gate = std::clamp(new_val, gate_mode::MINIMUM, gate_mode::MAXIMUM);
    }

    inline void set_note(size_t index, midi::note new_val) { steps[index].note = new_val; }

    inline void set_note_relative(size_t index, int offset) {
      const auto old_val = static_cast<int>(steps[index].note);
      const auto new_val = static_cast<midi::note>(old_val + offset);
      set_note(index, std::clamp(new_val, midi::note::MINIMUM, midi::note::MAXIMUM));
    }

    inline void set_repetitions(size_t index, std::int8_t new_val) {
      steps[index].repetitions = new_val;
    }

    inline void set_repetitions_relative(size_t index, std::int8_t offset) {
      const auto new_val = steps[index].repetitions + offset;
      set_repetitions(index, std::clamp(new_val, 0, MAX_REPETITIONS));
    }

    inline void set_activity(size_t index, bool new_val) {
      steps[index].is_active = new_val;
    }
  };
} // namespace sseq

#endif // SEQUENCER_HPP
