#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include "io/BufferedSerial.hpp"
#include "io/Button.hpp"
#include "io/Encoder.hpp"
#include "io/Grid.hpp"
#include "mbed.h"
#include "sacroseq.hpp"

namespace sseq {
  class Interface {
    // TODO consider moving to sacroseq.hpp?
    static constexpr size_t ENC_A_INDEX = 1;
    static constexpr size_t ENC_B_INDEX = 2;
    static constexpr size_t ENC_BTN_INDEX = 0;
    static constexpr size_t LED_INDEX = 0;
    static constexpr size_t NUM_ENCODERS = 8;
    static constexpr size_t NUM_BUTTONS = 20;

    io::Grid<grid::NUM_SOURCES, grid::NUM_INPUTS, grid::NUM_OUTPUTS> gridIO{
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

    std::array<io::Encoder, NUM_ENCODERS> encoders = {
        io::Encoder(gridIO.getRef(0, ENC_A_INDEX), gridIO.getRef(0, ENC_B_INDEX)),
        io::Encoder(gridIO.getRef(1, ENC_A_INDEX), gridIO.getRef(1, ENC_B_INDEX)),
        io::Encoder(gridIO.getRef(2, ENC_A_INDEX), gridIO.getRef(2, ENC_B_INDEX)),
        io::Encoder(gridIO.getRef(3, ENC_A_INDEX), gridIO.getRef(3, ENC_B_INDEX)),
        io::Encoder(gridIO.getRef(4, ENC_A_INDEX), gridIO.getRef(4, ENC_B_INDEX)),
        io::Encoder(gridIO.getRef(5, ENC_A_INDEX), gridIO.getRef(5, ENC_B_INDEX)),
        io::Encoder(gridIO.getRef(6, ENC_A_INDEX), gridIO.getRef(6, ENC_B_INDEX)),
        io::Encoder(gridIO.getRef(7, ENC_A_INDEX), gridIO.getRef(7, ENC_B_INDEX)),
    };

    std::array<DigitalIn, 4> buttonInputs = {
        DigitalIn(buttons::PINS[0], PullUp),
        DigitalIn(buttons::PINS[1], PullUp),
        DigitalIn(buttons::PINS[2], PullUp),
        DigitalIn(buttons::PINS[3], PullUp),
    };
    std::array<bool, 4> buttonValues;

    Timer tmr;
    std::array<io::Button, NUM_BUTTONS> buttons = {
        io::Button(tmr, gridIO.getRef(0, 3)),
        io::Button(tmr, gridIO.getRef(1, 3)),
        io::Button(tmr, gridIO.getRef(2, 3)),
        io::Button(tmr, gridIO.getRef(3, 3)),
        io::Button(tmr, gridIO.getRef(4, 3)),
        io::Button(tmr, gridIO.getRef(5, 3)),
        io::Button(tmr, gridIO.getRef(6, 3)),
        io::Button(tmr, gridIO.getRef(7, 3)),
        io::Button(tmr, buttonValues[0]),
        io::Button(tmr, buttonValues[1]),
        io::Button(tmr, buttonValues[2]),
        io::Button(tmr, buttonValues[3]),
        io::Button(tmr, gridIO.getRef(0, ENC_BTN_INDEX)),
        io::Button(tmr, gridIO.getRef(1, ENC_BTN_INDEX)),
        io::Button(tmr, gridIO.getRef(2, ENC_BTN_INDEX)),
        io::Button(tmr, gridIO.getRef(3, ENC_BTN_INDEX)),
        io::Button(tmr, gridIO.getRef(4, ENC_BTN_INDEX)),
        io::Button(tmr, gridIO.getRef(5, ENC_BTN_INDEX)),
        io::Button(tmr, gridIO.getRef(6, ENC_BTN_INDEX)),
        io::Button(tmr, gridIO.getRef(7, ENC_BTN_INDEX)),
    };

    size_t buttonInputIndex = 0;
    size_t buttonIndex = 0;
    size_t encoderIndex = 0;

  public:
    Interface();

    void step();

    std::array<bool, NUM_BUTTONS> checkAndResetAllButtonsClicked();
    std::array<int, NUM_ENCODERS> getAndResetAllEncDeltas();

    inline bool checkAndResetButtonClicked(size_t index) {
      const bool ret = buttons[index].isClicked();
      buttons[index].resetClicked();
      return ret;
    }

    inline bool isButtonHeld(size_t index) const { return buttons[index].isHeld(); }

    inline int getEncDelta(size_t index) const { return encoders[index].peek(); }

    inline int getAndResetEncDelta(size_t index){ return encoders[index].read(); }

    inline bool getLedVal(size_t index) const {
      return !gridIO.getDesired(index, LED_INDEX);
    }

    inline void setLedVal(size_t index, bool newVal = true) {
      gridIO.set(index, LED_INDEX, !newVal);
    }
  };
} // namespace sseq

#endif // INTERFACE_HPP
