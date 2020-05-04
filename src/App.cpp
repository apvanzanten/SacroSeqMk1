#include "App.hpp"

namespace sseq {
    void App::step() {
      interf.step();
      if (seq.updateAndCheckBuffer()) {
        const auto noteMsg = seq.getNextNote();
        midi::msg midiMsg;
        if (noteMsg.velocity > 0) {
          midiMsg = midi::make_note_on(channel, noteMsg.note, noteMsg.velocity);
        } else {
          midiMsg = midi::make_note_off(channel, noteMsg.note);
        }
        midi.tryPutC(midiMsg.status);
        midi.tryPutC(midiMsg.note);
        midi.tryPutC(midiMsg.velocity);
      }
      display.tryWriteFromBuffer();
      midi.tryWriteFromBuffer();
    }

    void App::updateMode() {
      if (interf.isButtonHeld(Buttons::shift)) {
        if (interf.checkAndResetButtonClicked(Buttons::topRight)) {
          switchToMode(Modes::global);
        } else if (interf.checkAndResetButtonClicked(Buttons::bottomRight)) {
          switchToMode(Modes::menu);
        }
      } else {
        if (interf.checkAndResetButtonClicked(Buttons::topRight)) {
          switchToMode(Modes::note);
        } else if (interf.checkAndResetButtonClicked(Buttons::bottomRight)) {
          switchToMode(Modes::repetition);
        } else if (interf.checkAndResetButtonClicked(Buttons::bottomLeft)) {
          switchToMode(Modes::gate);
        }
      }
    }

    void App::updateActivities() {
      for (size_t stepIndex = 0; stepIndex < stepIndexPairs.size(); stepIndex++) {
        const auto &buttonIndex = stepIndexPairs[stepIndex].mainButton;
        if (interf.checkAndResetButtonClicked(buttonIndex)) {
          const auto oldVal = seq.getActivity(stepIndex);
          seq.setActivity(stepIndex, !oldVal);
        }
      }
    }

    void App::updateNotes() {
      for (size_t stepIndex = 0; stepIndex < stepIndexPairs.size(); stepIndex++) {
        const auto &buttonIndex = stepIndexPairs[stepIndex].encButton;
        if (interf.checkAndResetButtonClicked(buttonIndex)) {
          writeToDisplay("dflt");
          seq.setNote(stepIndex, midi::DEFAULT_NOTE);
          interf.getAndResetEncDelta(stepIndex);
        } else {
          const auto offset = interf.getAndResetEncDelta(stepIndex);
          seq.setNoteRelative(stepIndex, offset);
          if(offset != 0){
            writeToDisplay(static_cast<int>(seq.getNote(stepIndex)));
          }
        }
      }
    }

    void App::updateRepetitions() {
      for (size_t stepIndex = 0; stepIndex < stepIndexPairs.size(); stepIndex++) {
        const auto &buttonIndex = stepIndexPairs[stepIndex].encButton;
        if (interf.checkAndResetButtonClicked(buttonIndex)) {
          writeToDisplay("0");
          seq.setRepetitions(stepIndex, 0);
          interf.getAndResetEncDelta(stepIndex);
        } else {
          const auto offset = interf.getAndResetEncDelta(stepIndex);
          seq.setRepetitionsRelative(stepIndex, offset);

          if(offset != 0){
            writeToDisplay(seq.getRepetitions(stepIndex));
          }
        }
      }
    }

    void App::updateGates() {
      for (size_t stepIndex = 0; stepIndex < stepIndexPairs.size(); stepIndex++) {
        const auto &buttonIndex = stepIndexPairs[stepIndex].encButton;
        if (interf.checkAndResetButtonClicked(buttonIndex)) {
          seq.setGateMode(stepIndex, Sequencer::GateMode::hold);
          interf.getAndResetEncDelta(stepIndex);
        } else {
          const auto offset = interf.getAndResetEncDelta(stepIndex);
          seq.setGateModeRelative(stepIndex, offset);

          if(offset != 0){
            writeToDisplay(static_cast<int>(seq.getGateMode(stepIndex)));
          }
        }
      }
    }

    void App::updateGlobals() {
      const auto encDeltas = interf.getAndResetAllEncDeltas();

      if (encDeltas[Globals::minVelocity] != 0) {
        // TODO placeholder
      } else if (encDeltas[Globals::maxVelocity] != 0) {
        // TODO placeholder
      } else if (encDeltas[Globals::velocityDistribution] != 0) {
        // TODO placeholder
      } else if (encDeltas[Globals::minStepOffset] != 0) {
        // TODO placeholder
      } else if (encDeltas[Globals::maxStepOffset] != 0) {
        // TODO placeholder
      } else if (encDeltas[Globals::stepOffsetDistribution] != 0) {
        // TODO placeholder
      } else if (encDeltas[Globals::gateTime] != 0) {
        // TODO placeholder
      } else if (encDeltas[Globals::bpm] != 0) {
        seq.setBpmRelative(encDeltas[Globals::bpm]);
        writeToDisplay(seq.getBpm());
      }
    }

    void App::updateMenu() { 
      // TODO 
      }

    void App::updateLeds() {
      for(size_t stepIndex = 0; stepIndex < 8; ++stepIndex){
        interf.setLedVal(stepIndex, 
          seq.getActivity(stepIndex) ^ (seq.getCurrentStep() == stepIndex) 
        );
      }
    }

    void App::update() {
      updateMode();
      updateActivities();
      updateLeds();

      switch (mode) {
      case Modes::note:
        updateNotes();
        break;
      case Modes::repetition:
        updateRepetitions();
        break;
      case Modes::gate:
        updateGates();
        break;
      case Modes::global:
        updateGlobals();
        break;
      case Modes::menu:
        updateMenu();
        break;
      default:
        switchToMode(Modes::note);
        break;
      }
    }
}