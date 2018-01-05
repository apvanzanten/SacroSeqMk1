#ifndef APP_HPP
#define APP_HPP

#include "interface.hpp"
#include "io/buffered_serial.hpp"
#include "sacroseq.hpp"
#include "sequencer.hpp"

// TODO refactor to NOT use enum classes, they're terrible for use as indices.

namespace sseq {
  class app {
    sequencer seq{};
    interface interf{};
    io::buffered_serial<disp::BUFFER_SIZE> display{disp::pins::TX, disp::pins::RX, disp::BAUD_RATE};
    io::buffered_serial<midi::BUFFER_SIZE> midi{midi::pins::TX, midi::pins::RX, midi::BAUD_RATE};

    std::uint8_t channel = 0;

    enum class modes { note, repetition, gate, global, menu } mode;

    struct buttons {
      enum vals {
        bottom_0 = 0,
        bottom_1 = 1,
        bottom_2 = 2,
        bottom_3 = 3,
        bottom_4 = 4,
        bottom_5 = 5,
        bottom_6 = 6,
        bottom_7 = 7,
        shift = 8,
        top_right = 9,
        bottom_left = 10,
        bottom_right = 11,
        enc_0 = 12,
        enc_1 = 13,
        enc_2 = 14,
        enc_3 = 15,
        enc_4 = 16,
        enc_5 = 17,
        enc_6 = 18,
        enc_7 = 19,
      };
    };

    struct step_index_pair {
      size_t main_button;
      size_t enc_button;
    };

    std::array<step_index_pair, 8> step_index_pairs{{
        {buttons::bottom_0, buttons::enc_0},
        {buttons::bottom_1, buttons::enc_1},
        {buttons::bottom_2, buttons::enc_2},
        {buttons::bottom_3, buttons::enc_3},
        {buttons::bottom_4, buttons::enc_4},
        {buttons::bottom_5, buttons::enc_5},
        {buttons::bottom_6, buttons::enc_6},
        {buttons::bottom_7, buttons::enc_7},
    }};

    struct globals {
      enum vals {
        min_velocity,
        max_velocity,
        velocity_distribution,
        min_step_offset,
        max_step_offset,
        step_offset_distribution,
        gate_time,
        bpm
      };
    };

  public:
    inline void step() {
      interf.step();
      if (seq.update_and_check_buffer()) {
        const auto note_message = seq.get_next_note();
        midi::msg midi_msg;
        if (note_message.velocity) {
          midi_msg = midi::make_note_on(channel, note_message.note, note_message.velocity);
        } else {
          midi_msg = midi::make_note_off(channel, note_message.note);
        }
        midi.try_putc(midi_msg.status);
        midi.try_putc(midi_msg.note);
        midi.try_putc(midi_msg.velocity);
      }
      display.try_write();
      midi.try_write();
    }

    inline void update_mode() {
      if (interf.is_button_held(buttons::shift)) {
        if (interf.check_and_reset_button_clicked(buttons::top_right)) {
          mode = modes::global;
        } else if (interf.check_and_reset_button_clicked(buttons::bottom_right)) {
          mode = modes::menu;
        }
      } else {
        if (interf.check_and_reset_button_clicked(buttons::top_right)) {
          mode = modes::note;
        } else if (interf.check_and_reset_button_clicked(buttons::bottom_right)) {
          mode = modes::repetition;
        } else if (interf.check_and_reset_button_clicked(buttons::bottom_left)) {
          mode = modes::gate;
        }
      }
    }

    inline void update_activities() {
      for (size_t step_index = 0; step_index < step_index_pairs.size(); step_index++) {
        const auto &button_index = step_index_pairs[step_index].main_button;
        if (interf.check_and_reset_button_clicked(button_index)) {
          const auto old_val = seq.get_activity(step_index);
          seq.set_activity(step_index, !old_val);
        }
      }
    }

    inline void update_notes() {
      for (size_t step_index = 0; step_index < step_index_pairs.size(); step_index++) {
        const auto &button_index = step_index_pairs[step_index].enc_button;
        if (interf.check_and_reset_button_clicked(button_index)) {
          seq.set_note(step_index, midi::DEFAULT_NOTE);
          interf.get_and_reset_enc_delta(step_index);
        } else {
          const auto offset = interf.get_and_reset_enc_delta(step_index);
          seq.set_note_relative(step_index, offset);
        }
      }
    }

    inline void update_repetitions() {
      for (size_t step_index = 0; step_index < step_index_pairs.size(); step_index++) {
        const auto &button_index = step_index_pairs[step_index].enc_button;
        if (interf.check_and_reset_button_clicked(button_index)) {
          seq.set_repetitions(step_index, 0);
          interf.get_and_reset_enc_delta(step_index);
        } else {
          const auto offset = interf.get_and_reset_enc_delta(step_index);
          seq.set_repetitions_relative(step_index, offset);
        }
      }
    }

    inline void update_gates() {
      for (size_t step_index = 0; step_index < step_index_pairs.size(); step_index++) {
        const auto &button_index = step_index_pairs[step_index].enc_button;
        if (interf.check_and_reset_button_clicked(button_index)) {
          seq.set_gate_mode(step_index, sequencer::gate_mode::hold);
          interf.get_and_reset_enc_delta(step_index);
        } else {
          const auto offset = interf.get_and_reset_enc_delta(step_index);
          seq.set_gate_mode_relative(step_index, offset);
        }
      }
    }

    inline void update_globals() {
      const auto enc_deltas = interf.get_and_reset_all_enc_deltas();

      if (enc_deltas[globals::min_velocity] != 0) {
        // placeholder
      } else if (enc_deltas[globals::max_velocity] != 0) {
        // placeholder
      } else if (enc_deltas[globals::velocity_distribution] != 0) {
        // placeholder
      } else if (enc_deltas[globals::min_step_offset] != 0) {
        // placeholder
      } else if (enc_deltas[globals::max_step_offset] != 0) {
        // placeholder
      } else if (enc_deltas[globals::step_offset_distribution] != 0) {
        // placeholder
      } else if (enc_deltas[globals::gate_time] != 0) {
        // placeholder
      } else if (enc_deltas[globals::bpm] != 0) {
        // placeholder
      }
    }

    inline void update_menu() {}

    inline void update() {
      update_mode();
      update_activities();

      switch (mode) {
      case modes::note:
        update_notes();
        break;
      case modes::repetition:
        update_repetitions();
        break;
      case modes::gate:
        update_gates();
        break;
      case modes::global:
        update_globals();
        break;
      case modes::menu:
        update_menu();
        break;
      default:
        mode = modes::note;
        break;
      }
    }
  }; // namespace sseq
} // namespace sseq

#endif // APP_HPP
