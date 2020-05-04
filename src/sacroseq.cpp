#include "sacroseq.hpp"

namespace sseq {
  namespace alerts {
    void haltAndBlink(const char *message) {
      Serial display(disp::pins::TX, disp::pins::RX, disp::BAUD_RATE);

      while (true) {
        display.putc(disp::CLEAR_SCREEN); // clear display
        wait(0.5);
        display.printf(message);
        wait(0.5);
      }
    }
  } // namespace alerts
} // namespace sseq
