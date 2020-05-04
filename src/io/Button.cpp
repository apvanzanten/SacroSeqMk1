#include "Button.hpp"

#include <memory>

sseq::io::Button::Button(Timer & tmr, const bool &input, int debounceMs)
    : tmr(tmr), input(input), debounceMs(debounceMs) {}

void sseq::io::Button::update() {
  switch (state) {
  case States::idle:
    if (input) {
      debounceEnd = tmr.read_ms() + debounceMs;
      state = States::waitForDebounce;
    }
    break;
  case States::waitForDebounce:
    if (tmr.read_ms() >= debounceEnd) {
      if (input) {
        _isHeld = true;
        _isClicked = true;
        state = States::held;
      } else {
        _isHeld = false;
        state = States::idle;
      }
    }
    break;
  case States::held:
    if (!input) {
      debounceEnd = tmr.read_ms() + debounceMs;
      state = States::waitForDebounce;
    }
    break;
  default: // state most likley uninitialized, initialiaze it
    state = States::idle;
    break;
  }
}