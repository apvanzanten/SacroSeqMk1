#include "Interface.hpp"

namespace sseq {
  Interface::Interface() {
    tmr.start();
  }

  void Interface::step() {
    gridIO.step();

    buttonValues[buttonInputIndex] = !(buttonInputs[buttonInputIndex].read());
    if(++buttonInputIndex == buttonInputs.size()){
      buttonInputIndex = 0;
    }

    buttons[buttonIndex].update();
    if(++buttonIndex == buttons.size()){
      buttonIndex = 0;
    }

    encoders[encoderIndex].update();
    if(++encoderIndex == encoders.size()){
      encoderIndex = 0;
    }
  }

  std::array<bool, Interface::NUM_BUTTONS> Interface::checkAndResetAllButtonsClicked() {
    std::array<bool, NUM_BUTTONS> out;
    for (size_t i = 0; i < out.size(); ++i) {
      out[i] = checkAndResetButtonClicked(i);
    }
    return out;
  }

  std::array<int, Interface::NUM_ENCODERS> Interface::getAndResetAllEncDeltas() {
    std::array<int, NUM_ENCODERS> out;
    for (size_t i = 0; i < 8; i++) {
      out[i] = getAndResetEncDelta(i);
    }
    return out;
  }
}