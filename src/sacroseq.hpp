#ifndef SACROSEQ_HPP
#define SACROSEQ_HPP

#include "mbed.h" // NOTE this file exports std and mbed namespaces to global :(

#include <array>
#include <cstdint>

/* File containing various constants and simple helper functions (and heaven
 * forbid maybe even a few macros!) relevant to the SacroSeqMk1 project.
 */

namespace sseq {
  constexpr std::uint8_t MAIN_CLOCK_PERIOD_US = 50;
  namespace disp { // stuff pertaining to the 7-segment display
    namespace pins {
      constexpr PinName TX = D8;
      constexpr PinName RX = D7;
    } // namespace pins
    constexpr std::uint32_t BAUD_RATE = 9600;
    constexpr std::uint8_t CLEAR_SCREEN = 0x76;
  } // namespace disp
  namespace grid {
    namespace pins {
      // TODO figure out way to automate array sizing
      constexpr std::array<PinName, 8> SOURCES = {D12, D11, D10, D9, D6, D5, D4, D3};
      constexpr PinName ENC_BUTTON = D14;
      constexpr PinName ENC_A = D16;
      constexpr PinName ENC_B = D15;
      constexpr PinName BUTTON = D18;
      constexpr PinName LED = D17;
    } // namespace pins
    constexpr std::size_t NUM_SOURCES = pins::SOURCES.size();
    constexpr std::size_t NUM_INPUTS = 4;
    constexpr std::size_t NUM_OUTPUTS = 1;
  } // namespace grid
  namespace midi {
    namespace pins {
      constexpr PinName TX = D1;
      constexpr PinName RX = D0;
    } // namespace pins

    constexpr std::uint32_t BAUD_RATE = 115200; // put this back to 31250 when we actually run midi

  } // namespace midi
  namespace buttons {
    constexpr std::array<PinName, 4> PINS = {D23, D22, D21, D20};
    constexpr std::uint8_t DEBOUNCE_MS = 10;
  } // namespace buttons
  namespace alerts {
    /* Halt and blink a short message (at most 4 characters) on the 7-segment
     * display. */
    void halt_and_blink(const char *message);

    /* Indicate to the outside world that we're done doing whatever we were
     * doing. */
    inline void done() { halt_and_blink("done"); }

    /* Function that indicates to the outside world that we've encountered an
     * error. */
    inline void error() { halt_and_blink("err"); }
  } // namespace alerts
} // namespace sseq

#endif
