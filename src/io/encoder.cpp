#include "encoder.hpp"

sseq::io::encoder::encoder(const bool &a, const bool &b) : a(a), b(b) {}

void sseq::io::encoder::update() {
  if (a != prev_a || b != prev_b) {
    prev_a = a;
    prev_b = b;
    switch (state) {
    case states::initial:
      if (a && b) {
        state = states::clockwise_1;
      } else if (a && !b) {
        state = states::counter_clockwise_1;
      } else {
        state = states::error;
      }
      break;
    case states::clockwise_1:
      if (a && !b) {
        state = states::clockwise_2;
      } else if (!a && !b) {
        state = states::initial;
      } else {
        state = states::error;
      }
      break;
    case states::clockwise_2:
      if (!a && !b) {
        state = states::initial;
        delta += 1;
      } else if (a && b) {
        state = states::initial;
      } else {
        state = states::error;
      }
      break;
    case states::counter_clockwise_1:
      if (a && b) {
        state = states::counter_clockwise_2;
      } else if (!a && !b) {
        state = states::initial;
      } else {
        state = states::error;
      }
      break;
    case states::counter_clockwise_2:
      if (!a && !b) {
        state = states::initial;
        delta -= 1;
      } else if (a && !b) {
        state = states::counter_clockwise_1;
      } else {
        state = states::error;
      }
      break;
    case states::error: // error state, wait for 0,0 to reset
      if (!a && !b) {
        state = states::initial;
      }
      break;
    default: // probably means our state variable is uninitialized; initialize it
      state = states::initial;
      break;
    }
  }
}
