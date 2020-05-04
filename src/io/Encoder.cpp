#include "Encoder.hpp"

sseq::io::Encoder::Encoder(const bool &a, const bool &b) : a(a), b(b) {}

void sseq::io::Encoder::update() {
  if (a != prevA || b != prevB) {
    prevA = a;
    prevB = b;
    switch (state) {
    case States::idle:
      if (a && b) {
        state = States::clockwise1;
      } else if (a && !b) {
        state = States::counterClockwise1;
      } else {
        state = States::error;
      }
      break;
    case States::clockwise1:
      if (a && !b) {
        state = States::clockwise2;
      } else if (!a && !b) {
        state = States::idle;
      } else {
        state = States::error;
      }
      break;
    case States::clockwise2:
      if (!a && !b) {
        state = States::idle;
        delta += 1;
      } else if (a && b) {
        state = States::idle;
      } else {
        state = States::error;
      }
      break;
    case States::counterClockwise1:
      if (a && b) {
        state = States::counterClockwise2;
      } else if (!a && !b) {
        state = States::idle;
      } else {
        state = States::error;
      }
      break;
    case States::counterClockwise2:
      if (!a && !b) {
        state = States::idle;
        delta -= 1;
      } else if (a && !b) {
        state = States::counterClockwise1;
      } else {
        state = States::error;
      }
      break;
    case States::error: // error state, wait for 0,0 to reset
      if (!a && !b) {
        state = States::idle;
      }
      break;
    default: // probably means our state variable is uninitialized; initialize it
      state = States::idle;
      break;
    }
  }
}
