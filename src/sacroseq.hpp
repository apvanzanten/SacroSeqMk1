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
  constexpr std::size_t MAIN_CLOCK_FREQUENCY_HZ = (1 * 1000 * 1000) / MAIN_CLOCK_PERIOD_US;
  constexpr std::uint32_t MAX_REPETITIONS = 7;
  namespace disp { // stuff pertaining to the 7-segment display
    namespace pins {
      constexpr PinName TX = D8;
      constexpr PinName RX = D7;
    } // namespace pins
    constexpr std::uint32_t BAUD_RATE = 9600;
    constexpr std::uint8_t CLEAR_SCREEN = 0x76;
    constexpr std::size_t SCREEN_SIZE = 4;
    constexpr std::size_t BUFFER_SIZE = 32;
    constexpr char BLANK_SPACE = ' ';
    constexpr char UNKNOWN_CHAR = '_';
    constexpr bool is_valid(char c) { return !(c < '0' || c > 'u' || c == 'k' || c == 'm'); }
    constexpr char sanitize(char c) { return (is_valid(c) ? c : UNKNOWN_CHAR); }
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

    constexpr std::uint32_t BAUD_RATE = 31250; // put this back to 31250 when we actually run midi
    constexpr std::size_t BUFFER_SIZE = 32;
    constexpr std::uint8_t NOTE_ON = 0x90;
    constexpr std::uint8_t NOTE_OFF = 0x80;
    constexpr std::uint8_t CHANNEL_MASK = 0xf;
    constexpr std::uint8_t VELOCITY_MASK = 0x7f;

    enum class note {
      MINIMUM = 33,
      a1 = 33,
      as1 = 34,
      b1 = 35,
      c2 = 36,
      cs2 = 37,
      d2 = 38,
      ds2 = 39,
      e2 = 40,
      f2 = 41,
      fs2 = 42,
      g2 = 43,
      gs2 = 44,
      a2 = 45,
      as2 = 46,
      b2 = 47,
      c3 = 48,
      cs3 = 49,
      d3 = 50,
      ds3 = 51,
      e3 = 52,
      f3 = 53,
      fs3 = 54,
      g3 = 55,
      gs3 = 56,
      a3 = 57,
      as3 = 58,
      b3 = 59,
      c4 = 60,
      cs4 = 61,
      d4 = 62,
      ds4 = 63,
      e4 = 64,
      f4 = 65,
      fs4 = 66,
      g4 = 67,
      gs4 = 68,
      a4 = 69,
      as4 = 70,
      b4 = 71,
      c5 = 72,
      cs5 = 73,
      d5 = 74,
      ds5 = 75,
      e5 = 76,
      f5 = 77,
      fs5 = 78,
      g5 = 79,
      gs5 = 80,
      MAXIMUM = 80
    };

    constexpr note DEFAULT_NOTE = note::c4;

    struct msg {
      std::uint8_t status = NOTE_OFF;
      std::uint8_t note = static_cast<std::uint8_t>(note::c4);
      std::uint8_t velocity = 64;
    };

    constexpr msg make_note_on(std::uint8_t channel, note n, std::uint8_t velocity) {
      msg m{};
      m.status = (NOTE_ON | (channel & CHANNEL_MASK));
      m.note = static_cast<std::uint8_t>(n);
      m.velocity = (velocity & VELOCITY_MASK);
      return m;
    }
    constexpr msg make_note_off(std::uint8_t channel, note n){
      msg m{};
      m.status = (NOTE_OFF | (channel & CHANNEL_MASK));
      m.note = static_cast<std::uint8_t>(n);
      m.velocity = 0;
      return m;
    }

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
