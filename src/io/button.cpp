#include "button.hpp"

#include <memory>

sseq::io::button::button(Timer & t, const bool &input, int debounce_ms)
    : t(t), input(input), debounce_ms(debounce_ms) {}

void sseq::io::button::update() {
  switch (state) {
  case states::initial:
    if (input) {
      debounce_end = t.read_ms() + debounce_ms;
      state = states::wait_for_debounce;
    }
    break;
  case states::wait_for_debounce:
    if (t.read_ms() >= debounce_end) {
      if (input) {
        _is_held = true;
        _is_clicked = true;
        state = states::held;
      } else {
        _is_held = false;
        state = states::initial;
      }
    }
    break;
  case states::held:
    if (!input) {
      debounce_end = t.read_ms() + debounce_ms;
      state = states::wait_for_debounce;
    }
    break;
  default: // state most likley uninitialized, initialiaze it
    state = states::initial;
    break;
  }
}