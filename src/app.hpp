#ifndef APP_HPP
#define APP_HPP

#include "interface.hpp"
#include "io/BufferedSerial.hpp"
#include "sacroseq.hpp"
#include "sequencer.hpp"
#include <string>

namespace sseq {
  class app {
    sequencer seq{};
    interface interf{};
    io::BufferedSerial<disp::BUFFER_SIZE> display{disp::pins::TX, disp::pins::RX, disp::BAUD_RATE};
    io::BufferedSerial<midi::BUFFER_SIZE> midi{midi::pins::TX, midi::pins::RX, midi::BAUD_RATE};

    std::uint8_t channel = 1;

    struct modes {
      enum val { note, repetition, gate, global, menu };
      static constexpr std::int8_t NUM = 5;
    };
    modes::val mode;

    static constexpr std::array<char[disp::SCREEN_SIZE + 1], modes::NUM> mode_msgs = {
        "note", "reps", "gate", "glob", "_enu"};

    struct buttons {
      enum vals {
        bottom_0 = 0,
        bottom_1 = 1,
        bottom_2 = 2,
        bottom_3 = 3,
        bottom_4 = 4,
        bottom_5 = 5,
        bottom_6 = 6,
        bottom_7 = 7,
        shift = 8,
        top_right = 9,
        bottom_left = 10,
        bottom_right = 11,
        enc_0 = 12,
        enc_1 = 13,
        enc_2 = 14,
        enc_3 = 15,
        enc_4 = 16,
        enc_5 = 17,
        enc_6 = 18,
        enc_7 = 19,
      };
    };

    struct step_index_pair {
      size_t main_button;
      size_t enc_button;
    };

    static constexpr std::array<step_index_pair, 8> step_index_pairs{{
        {buttons::bottom_0, buttons::enc_0},
        {buttons::bottom_1, buttons::enc_1},
        {buttons::bottom_2, buttons::enc_2},
        {buttons::bottom_3, buttons::enc_3},
        {buttons::bottom_4, buttons::enc_4},
        {buttons::bottom_5, buttons::enc_5},
        {buttons::bottom_6, buttons::enc_6},
        {buttons::bottom_7, buttons::enc_7},
    }};

    struct globals {
      enum vals {
        min_velocity,
        max_velocity,
        velocity_distribution,
        min_step_offset,
        max_step_offset,
        step_offset_distribution,
        gate_time,
        bpm
      };
    };

    void update_mode();
    void update_activities();
    void update_notes();
    void update_repetitions();
    void update_gates();
    void update_globals();
    void update_menu();
    void update_leds();

  public:
    void step();
    void update();

    inline void write_to_display(const char * msg){
      display.tryPutC(disp::CLEAR_SCREEN);
      display.tryPutS(msg);
    }

    inline void write_to_display(int n) {
      write_to_display(
        std::to_string(n)
        .substr(0,4)
        .c_str()
      );
    }

    inline void switch_to_mode(modes::val new_mode) {
      write_to_display(mode_msgs[new_mode]);
      mode = new_mode;
    }


  }; // namespace sseq
} // namespace sseq

#endif // APP_HPP
