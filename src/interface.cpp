#include "interface.hpp"

namespace sseq {
  interface::interface() {
    tmr.start();
  }

  void interface::step() {
    grid_io.step();

    button_reads[button_in_index] = !(button_ins[button_in_index].read());
    if(++button_in_index == button_ins.size()){
      button_in_index = 0;
    }

    buttons[button_index].update();
    if(++button_index == buttons.size()){
      button_index = 0;
    }

    encoders[encoder_index].update();
    if(++encoder_index == encoders.size()){
      encoder_index = 0;
    }
  }

  std::array<bool, interface::NUM_BUTTONS> interface::check_and_reset_all_buttons_clicked() {
    std::array<bool, NUM_BUTTONS> out;
    for (size_t i = 0; i < out.size(); ++i) {
      out[i] = check_and_reset_button_clicked(i);
    }
    return out;
  }

  std::array<int, interface::NUM_ENCODERS> interface::get_and_reset_all_enc_deltas() {
    std::array<int, NUM_ENCODERS> out;
    for (size_t i = 0; i < 8; i++) {
      out[i] = get_and_reset_enc_delta(i);
    }
    return out;
  }
}