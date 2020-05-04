#include "app.hpp"

namespace sseq {
    void app::step() {
      interf.step();
      if (seq.updateAndCheckBuffer()) {
        const auto note_message = seq.getNextNote();
        midi::msg midi_msg;
        if (note_message.velocity > 0) {
          midi_msg = midi::make_note_on(channel, note_message.note, note_message.velocity);
        } else {
          midi_msg = midi::make_note_off(channel, note_message.note);
        }
        midi.tryPutC(midi_msg.status);
        midi.tryPutC(midi_msg.note);
        midi.tryPutC(midi_msg.velocity);
      }
      display.tryWriteFromBuffer();
      midi.tryWriteFromBuffer();
    }

    void app::update_mode() {
      if (interf.isButtonHeld(buttons::shift)) {
        if (interf.checkAndResetButtonClicked(buttons::top_right)) {
          switch_to_mode(modes::global);
        } else if (interf.checkAndResetButtonClicked(buttons::bottom_right)) {
          switch_to_mode(modes::menu);
        }
      } else {
        if (interf.checkAndResetButtonClicked(buttons::top_right)) {
          switch_to_mode(modes::note);
        } else if (interf.checkAndResetButtonClicked(buttons::bottom_right)) {
          switch_to_mode(modes::repetition);
        } else if (interf.checkAndResetButtonClicked(buttons::bottom_left)) {
          switch_to_mode(modes::gate);
        }
      }
    }

    void app::update_activities() {
      for (size_t step_index = 0; step_index < step_index_pairs.size(); step_index++) {
        const auto &button_index = step_index_pairs[step_index].main_button;
        if (interf.checkAndResetButtonClicked(button_index)) {
          const auto old_val = seq.getActivity(step_index);
          seq.setActivity(step_index, !old_val);
        }
      }
    }

    void app::update_notes() {
      for (size_t step_index = 0; step_index < step_index_pairs.size(); step_index++) {
        const auto &button_index = step_index_pairs[step_index].enc_button;
        if (interf.checkAndResetButtonClicked(button_index)) {
          write_to_display("dflt");
          seq.setNote(step_index, midi::DEFAULT_NOTE);
          interf.getAndResetEncDelta(step_index);
        } else {
          const auto offset = interf.getAndResetEncDelta(step_index);
          seq.setNoteRelative(step_index, offset);
          if(offset != 0){
            write_to_display(static_cast<int>(seq.getNote(step_index)));
          }
        }
      }
    }

    void app::update_repetitions() {
      for (size_t step_index = 0; step_index < step_index_pairs.size(); step_index++) {
        const auto &button_index = step_index_pairs[step_index].enc_button;
        if (interf.checkAndResetButtonClicked(button_index)) {
          write_to_display("0");
          seq.setRepetitions(step_index, 0);
          interf.getAndResetEncDelta(step_index);
        } else {
          const auto offset = interf.getAndResetEncDelta(step_index);
          seq.setRepetitionsRelative(step_index, offset);

          if(offset != 0){
            write_to_display(seq.getRepetitions(step_index));
          }
        }
      }
    }

    void app::update_gates() {
      for (size_t step_index = 0; step_index < step_index_pairs.size(); step_index++) {
        const auto &button_index = step_index_pairs[step_index].enc_button;
        if (interf.checkAndResetButtonClicked(button_index)) {
          seq.setGateMode(step_index, Sequencer::GateMode::hold);
          interf.getAndResetEncDelta(step_index);
        } else {
          const auto offset = interf.getAndResetEncDelta(step_index);
          seq.setGateModeRelative(step_index, offset);

          if(offset != 0){
            write_to_display(static_cast<int>(seq.getGateMode(step_index)));
          }
        }
      }
    }

    void app::update_globals() {
      const auto enc_deltas = interf.getAndResetAllEncDeltas();

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
        seq.setBpmRelative(enc_deltas[globals::bpm]);
        write_to_display(seq.getBpm());
      }
    }

    void app::update_menu() { 
      // TODO 
      }

    void app::update_leds() {
      for(size_t step_index = 0; step_index < 8; ++step_index){
        interf.setLedVal(step_index, 
          seq.getActivity(step_index) ^ (seq.getCurrentStep() == step_index) 
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