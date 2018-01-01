#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include "io/buffered_serial.hpp"
#include "io/button.hpp"
#include "io/encoder.hpp"
#include "io/grid.hpp"
#include "mbed.h"
#include "sacroseq.hpp"

namespace sseq {
  class interface {
    io::grid<grid::NUM_SOURCES, grid::NUM_INPUTS, grid::NUM_OUTPUTS> grid_io{
        {
            DigitalOut(grid::pins::SOURCES[0]),
            DigitalOut(grid::pins::SOURCES[1]),
            DigitalOut(grid::pins::SOURCES[2]),
            DigitalOut(grid::pins::SOURCES[3]),
            DigitalOut(grid::pins::SOURCES[4]),
            DigitalOut(grid::pins::SOURCES[5]),
            DigitalOut(grid::pins::SOURCES[6]),
            DigitalOut(grid::pins::SOURCES[7]),
        },
        {DigitalIn(grid::pins::ENC_BUTTON, PullDown), DigitalIn(grid::pins::ENC_A, PullDown),
         DigitalIn(grid::pins::ENC_B, PullDown), DigitalIn(grid::pins::BUTTON, PullDown)},
        {DigitalOut(grid::pins::LED)}};

    static const size_t enc_a_index = 1;
    static const size_t enc_b_index = 2;
    static const size_t enc_btn_index = 0;

    std::array<io::encoder, 8> encoders = {
        io::encoder(grid_io.get_ref(0, enc_a_index), grid_io.get_ref(0, enc_b_index)),
        io::encoder(grid_io.get_ref(1, enc_a_index), grid_io.get_ref(1, enc_b_index)),
        io::encoder(grid_io.get_ref(2, enc_a_index), grid_io.get_ref(2, enc_b_index)),
        io::encoder(grid_io.get_ref(3, enc_a_index), grid_io.get_ref(3, enc_b_index)),
        io::encoder(grid_io.get_ref(4, enc_a_index), grid_io.get_ref(4, enc_b_index)),
        io::encoder(grid_io.get_ref(5, enc_a_index), grid_io.get_ref(5, enc_b_index)),
        io::encoder(grid_io.get_ref(6, enc_a_index), grid_io.get_ref(6, enc_b_index)),
        io::encoder(grid_io.get_ref(7, enc_a_index), grid_io.get_ref(7, enc_b_index)),
    };

    std::array<DigitalIn, 4> button_ins = {
        DigitalIn(buttons::PINS[0], PullUp),
        DigitalIn(buttons::PINS[1], PullUp),
        DigitalIn(buttons::PINS[2], PullUp),
        DigitalIn(buttons::PINS[3], PullUp),
    };
    std::array<bool, 4> button_reads;

    Timer tmr;
    std::array<io::button, 20> buttons = {
        io::button(tmr, grid_io.get_ref(0, 3)),
        io::button(tmr, grid_io.get_ref(1, 3)),
        io::button(tmr, grid_io.get_ref(2, 3)),
        io::button(tmr, grid_io.get_ref(3, 3)),
        io::button(tmr, grid_io.get_ref(4, 3)),
        io::button(tmr, grid_io.get_ref(5, 3)),
        io::button(tmr, grid_io.get_ref(6, 3)),
        io::button(tmr, grid_io.get_ref(7, 3)),
        io::button(tmr, button_reads[0]),
        io::button(tmr, button_reads[1]),
        io::button(tmr, button_reads[2]),
        io::button(tmr, button_reads[3]),
        io::button(tmr, grid_io.get_ref(0, enc_btn_index)),
        io::button(tmr, grid_io.get_ref(1, enc_btn_index)),
        io::button(tmr, grid_io.get_ref(2, enc_btn_index)),
        io::button(tmr, grid_io.get_ref(3, enc_btn_index)),
        io::button(tmr, grid_io.get_ref(4, enc_btn_index)),
        io::button(tmr, grid_io.get_ref(5, enc_btn_index)),
        io::button(tmr, grid_io.get_ref(6, enc_btn_index)),
        io::button(tmr, grid_io.get_ref(7, enc_btn_index)),
    };

    size_t button_in_index = 0;
    size_t button_index = 0;
    size_t encoder_index = 0;


  public:
    interface() {
      tmr.start();
    }

    inline void step() {
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

    inline bool get_and_reset_button_clicked(size_t index) {
      bool ret = buttons[index].is_clicked();
      buttons[index].reset_clicked();
      return ret;
    }

    inline bool get_button_held(size_t index) { return buttons[index].is_held(); }

    inline size_t get_enc_val(size_t index) { return encoders[index].peek(); }
  };
} // namespace sseq

#endif // INTERFACE_HPP
