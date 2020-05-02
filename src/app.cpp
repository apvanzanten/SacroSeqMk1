#include "app.hpp"

namespace sseq {
    void app::step() {
      interf.step();
      if (seq.update_and_check_buffer()) {
        const auto note_message = seq.get_next_note();
        midi::msg midi_msg;
        if (note_message.velocity > 0) {
          midi_msg = midi::make_note_on(channel, note_message.note, note_message.velocity);
        } else {
          midi_msg = midi::make_note_off(channel, note_message.note);
        }
        midi.try_putc(midi_msg.status);
        midi.try_putc(midi_msg.note);
        midi.try_putc(midi_msg.velocity);
      }
      display.try_write_from_buffer();
      midi.try_write_from_buffer();
    }

    void app::update_mode() {
      if (interf.is_button_held(buttons::shift)) {
        if (interf.check_and_reset_button_clicked(buttons::top_right)) {
          switch_to_mode(modes::global);
        } else if (interf.check_and_reset_button_clicked(buttons::bottom_right)) {
          switch_to_mode(modes::menu);
        }
      } else {
        if (interf.check_and_reset_button_clicked(buttons::top_right)) {
          switch_to_mode(modes::note);
        } else if (interf.check_and_reset_button_clicked(buttons::bottom_right)) {
          switch_to_mode(modes::repetition);
        } else if (interf.check_and_reset_button_clicked(buttons::bottom_left)) {
          switch_to_mode(modes::gate);
        }
      }
    }

    void app::update_activities() {
      for (size_t step_index = 0; step_index < step_index_pairs.size(); step_index++) {
        const auto &button_index = step_index_pairs[step_index].main_button;
        if (interf.check_and_reset_button_clicked(button_index)) {
          const auto old_val = seq.get_activity(step_index);
          seq.set_activity(step_index, !old_val);
        }
      }
    }

    void app::update_notes() {
      for (size_t step_index = 0; step_index < step_index_pairs.size(); step_index++) {
        const auto &button_index = step_index_pairs[step_index].enc_button;
        if (interf.check_and_reset_button_clicked(button_index)) {
          write_to_display("dflt");
          seq.set_note(step_index, midi::DEFAULT_NOTE);
          interf.get_and_reset_enc_delta(step_index);
        } else {
          const auto offset = interf.get_and_reset_enc_delta(step_index);
          seq.set_note_relative(step_index, offset);
          if(offset != 0){
            write_to_display(static_cast<int>(seq.get_note(step_index)));
          }
        }
      }
    }

    void app::update_repetitions() {
      for (size_t step_index = 0; step_index < step_index_pairs.size(); step_index++) {
        const auto &button_index = step_index_pairs[step_index].enc_button;
        if (interf.check_and_reset_button_clicked(button_index)) {
          write_to_display("0");
          seq.set_repetitions(step_index, 0);
          interf.get_and_reset_enc_delta(step_index);
        } else {
          const auto offset = interf.get_and_reset_enc_delta(step_index);
          seq.set_repetitions_relative(step_index, offset);

          if(offset != 0){
            write_to_display(seq.get_repetitions(step_index));
          }
        }
      }
    }

    void app::update_gates() {
      for (size_t step_index = 0; step_index < step_index_pairs.size(); step_index++) {
        const auto &button_index = step_index_pairs[step_index].enc_button;
        if (interf.check_and_reset_button_clicked(button_index)) {
          seq.set_gate_mode(step_index, sequencer::gate_mode::hold);
          interf.get_and_reset_enc_delta(step_index);
        } else {
          const auto offset = interf.get_and_reset_enc_delta(step_index);
          seq.set_gate_mode_relative(step_index, offset);

          if(offset != 0){
            write_to_display(static_cast<int>(seq.get_gate_mode(step_index)));
          }
        }
      }
    }

    void app::update_globals() {
      const auto enc_deltas = interf.get_and_reset_all_enc_deltas();

      if (enc_deltas[globals::min_velocity] != 0) {
        // TODO placeholder
      } else if (enc_deltas[globals::max_velocity] != 0) {
        // TODO placeholder
      } else if (enc_deltas[globals::velocity_distribution] != 0) {
        // TODO placeholder
      } else if (enc_deltas[globals::min_step_offset] != 0) {
        // TODO placeholder
      } else if (enc_deltas[globals::max_step_offset] != 0) {
        // TODO placeholder
      } else if (enc_deltas[globals::step_offset_distribution] != 0) {
        // TODO placeholder
      } else if (enc_deltas[globals::gate_time] != 0) {
        // TODO placeholder
      } else if (enc_deltas[globals::bpm] != 0) {
        seq.set_bpm_relative(enc_deltas[globals::bpm]);
        write_to_display(seq.get_bpm());
      }
    }

    void app::update_menu() { 
      // TODO 
      }

    void app::update_leds() {
      for(size_t step_index = 0; step_index < 8; ++step_index){
        interf.set_led_val(step_index, 
          seq.get_activity(step_index) ^ (seq.get_current_step() == step_index) 
        );
      }
    }

    void app::update() {
      update_mode();
      update_activities();
      update_leds();

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
        switch_to_mode(modes::note);
        break;
      }
    }
}