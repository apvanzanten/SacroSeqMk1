#include "sequencer.hpp"

namespace sseq {
  void sequencer::move_to_next_step() {
    int new_step_index_signed_unbounded = step_index + get_step_offset();

    while (new_step_index_signed_unbounded < 0) {
      new_step_index_signed_unbounded += steps.size();
    }
    size_t new_step_index_unbounded = static_cast<size_t>(new_step_index_signed_unbounded);
    while (new_step_index_unbounded >= steps.size()) {
      new_step_index_unbounded -= steps.size();
    }
    step_index = new_step_index_unbounded;
    current_repetition = 0;
  }

  void sequencer::move_to_next_repetition() {
    ++current_repetition;
    time_delta = 0;
  }

  bool sequencer::is_next_cycle() {
    return time_delta >= period;
  }

  int sequencer::num_reps_left() {
    return steps[step_index].repetitions - current_repetition;
  }

  bool sequencer::is_gate_time_passed() {
    return time_delta >= gate_time;  
  }

  void sequencer::update_gate_state() {
    switch (gate_state) {
    case gate_states::initial:
      if (is_next_cycle()) {
        time_delta = 0;

        note_on(steps[step_index].note);

        switch (steps[step_index].gate) {
        case gate_mode::hold:
          gate_state = gate_states::hold_on;
          break;
        case gate_mode::pulse_once:
          gate_state = gate_states::pulse_once_on;
          break;
        case gate_mode::pulse_once_then_hold:
          gate_state = gate_states::pulse_once_then_hold_on;
          break;
        case gate_mode::pulse_repeat:
          gate_state = gate_states::pulse_repeat_on;
          break;
        default: // panic!
          alerts::error();
        }
      }
      break;
    case gate_states::hold_on:
      if (is_gate_time_passed() && num_reps_left() <= 0) {
        note_off();
        move_to_next_step();
        gate_state = gate_states::initial;
      } else if (is_next_cycle()) {
        move_to_next_repetition();
      }
      break;
    case gate_states::pulse_once_on:
      if (is_gate_time_passed()) {
        note_off();
        gate_state = gate_states::hold_off;
      }
      break;
    case gate_states::hold_off:
      if (is_next_cycle()) {
        if (num_reps_left() <= 0) {
          move_to_next_step();
          gate_state = gate_states::initial;
        } else {
          move_to_next_repetition();
        }
      }
      break;
    case gate_states::pulse_once_then_hold_on:
      if (is_gate_time_passed()) {
        note_off();
        gate_state = gate_states::pulse_once_then_hold_off;
      }
      break;
    case gate_states::pulse_once_then_hold_off:
      if (is_next_cycle()) {
        if(num_reps_left() > 0)
        {
          note_on(steps[step_index].note);
          move_to_next_repetition();
          gate_state = gate_states::hold_on;
        } else {
          move_to_next_step();
          gate_state = gate_states::initial;
        }
      }
      break;
    case gate_states::pulse_repeat_on:
      if (is_gate_time_passed()) {
        note_off();
        gate_state = gate_states::pulse_repeat_off;
      }
      break;
    case gate_states::pulse_repeat_off:
      if (is_next_cycle()) {
        if (num_reps_left() <= 0) {
          move_to_next_step();
          gate_state = gate_states::initial;
        } else {
          move_to_next_repetition();
          note_on(steps[step_index].note);
          gate_state = gate_states::pulse_repeat_on;
        }
      }
      break;
    default: // panic!
      alerts::error();
    }
  }

} // namespace sseq
