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
    // TODO consider moving to sacroseq.hpp?
    static constexpr size_t ENC_A_INDEX = 1;
    static constexpr size_t ENC_B_INDEX = 2;
    static constexpr size_t ENC_BTN_INDEX = 0;
    static constexpr size_t LED_INDEX = 0;
    static constexpr size_t NUM_ENCODERS = 8;
    static constexpr size_t NUM_BUTTONS = 20;

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

    std::array<io::encoder, NUM_ENCODERS> encoders = {
        io::encoder(grid_io.get_ref(0, ENC_A_INDEX), grid_io.get_ref(0, ENC_B_INDEX)),
        io::encoder(grid_io.get_ref(1, ENC_A_INDEX), grid_io.get_ref(1, ENC_B_INDEX)),
        io::encoder(grid_io.get_ref(2, ENC_A_INDEX), grid_io.get_ref(2, ENC_B_INDEX)),
        io::encoder(grid_io.get_ref(3, ENC_A_INDEX), grid_io.get_ref(3, ENC_B_INDEX)),
        io::encoder(grid_io.get_ref(4, ENC_A_INDEX), grid_io.get_ref(4, ENC_B_INDEX)),
        io::encoder(grid_io.get_ref(5, ENC_A_INDEX), grid_io.get_ref(5, ENC_B_INDEX)),
        io::encoder(grid_io.get_ref(6, ENC_A_INDEX), grid_io.get_ref(6, ENC_B_INDEX)),
        io::encoder(grid_io.get_ref(7, ENC_A_INDEX), grid_io.get_ref(7, ENC_B_INDEX)),
    };

    std::array<DigitalIn, 4> button_ins = {
        DigitalIn(buttons::PINS[0], PullUp),
        DigitalIn(buttons::PINS[1], PullUp),
        DigitalIn(buttons::PINS[2], PullUp),
        DigitalIn(buttons::PINS[3], PullUp),
    };
    std::array<bool, 4> button_reads;

    Timer tmr;
    std::array<io::button, NUM_BUTTONS> buttons = {
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
        io::button(tmr, grid_io.get_ref(0, ENC_BTN_INDEX)),
        io::button(tmr, grid_io.get_ref(1, ENC_BTN_INDEX)),
        io::button(tmr, grid_io.get_ref(2, ENC_BTN_INDEX)),
        io::button(tmr, grid_io.get_ref(3, ENC_BTN_INDEX)),
        io::button(tmr, grid_io.get_ref(4, ENC_BTN_INDEX)),
        io::button(tmr, grid_io.get_ref(5, ENC_BTN_INDEX)),
        io::button(tmr, grid_io.get_ref(6, ENC_BTN_INDEX)),
        io::button(tmr, grid_io.get_ref(7, ENC_BTN_INDEX)),
    };

    size_t button_in_index = 0;
    size_t button_index = 0;
    size_t encoder_index = 0;

  public:
    interface();

    void step();

    std::array<bool, NUM_BUTTONS> check_and_reset_all_buttons_clicked();
    std::array<int, NUM_ENCODERS> get_and_reset_all_enc_deltas();

    inline bool check_and_reset_button_clicked(size_t index) {
      const bool ret = buttons[index].is_clicked();
      buttons[index].reset_clicked();
      return ret;
    }

    inline bool is_button_held(size_t index) const { return buttons[index].is_held(); }

    inline int get_enc_delta(size_t index) const { return encoders[index].peek(); }

    inline int get_and_reset_enc_delta(size_t index){ return encoders[index].read(); }

    inline bool get_led_val(size_t index) const {
      return !grid_io.get_desired(index, LED_INDEX);
    }

    inline void set_led_val(size_t index, bool new_val = true) {
      grid_io.set(index, LED_INDEX, !new_val);
    }
  };
} // namespace sseq

#endif // INTERFACE_HPP
