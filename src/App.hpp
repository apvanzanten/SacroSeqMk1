#ifndef APP_HPP
#define APP_HPP

#include "Interface.hpp"
#include "io/BufferedSerial.hpp"
#include "sacroseq.hpp"
#include "Sequencer.hpp"
#include <string>

namespace sseq {
  class App {
    Sequencer seq{};
    Interface interf{};
    io::BufferedSerial<disp::BUFFER_SIZE> display{disp::pins::TX, disp::pins::RX, disp::BAUD_RATE};
    io::BufferedSerial<midi::BUFFER_SIZE> midi{midi::pins::TX, midi::pins::RX, midi::BAUD_RATE};

    std::uint8_t channel = 1;

    struct Modes {
      enum val { note, repetition, gate, global, menu };
      static constexpr std::int8_t NUM = 5;
    };
    Modes::val mode;

    static constexpr std::array<char[disp::SCREEN_SIZE + 1], Modes::NUM> modeMsgs = {
        "note", "reps", "gate", "glob", "_enu"};

    struct Buttons {
      enum val {
        bottom0 = 0,
        bottom1 = 1,
        bottom2 = 2,
        bottom3 = 3,
        bottom4 = 4,
        bottom5 = 5,
        bottom6 = 6,
        bottom7 = 7,
        shift = 8,
        topRight = 9,
        bottomLeft = 10,
        bottomRight = 11,
        enc0 = 12,
        enc1 = 13,
        enc2 = 14,
        enc3 = 15,
        enc4 = 16,
        enc5 = 17,
        enc6 = 18,
        enc7 = 19,
      };
    };

    struct StepIndexPair {
      size_t mainButton;
      size_t encButton;
    };

    static constexpr std::array<StepIndexPair, 8> stepIndexPairs{{
        {Buttons::bottom0, Buttons::enc0},
        {Buttons::bottom1, Buttons::enc1},
        {Buttons::bottom2, Buttons::enc2},
        {Buttons::bottom3, Buttons::enc3},
        {Buttons::bottom4, Buttons::enc4},
        {Buttons::bottom5, Buttons::enc5},
        {Buttons::bottom6, Buttons::enc6},
        {Buttons::bottom7, Buttons::enc7},
    }};

    struct Globals {
      enum val {
        minVelocity,
        maxVelocity,
        velocityDistribution,
        minStepOffset,
        maxStepOffset,
        stepOffsetDistribution,
        gateTime,
        bpm
      };
    };

    void updateMode();
    void updateActivities();
    void updateNotes();
    void updateRepetitions();
    void updateGates();
    void updateGlobals();
    void updateMenu();
    void updateLeds();

  public:
    void step();
    void update();
    inline void writeToDisplay(const char * msg){
      display.tryPutC(disp::CLEAR_SCREEN);
      display.tryPutS(msg);
    }
    inline void writeToDisplay(int n) {
      writeToDisplay(
        std::to_string(n)
        .substr(0,4)
        .c_str()
      );
    }
    inline void switchToMode(Modes::val newMode) {
      writeToDisplay(modeMsgs[newMode]);
      mode = newMode;
    }
  }; // namespace sseq
} // namespace sseq

#endif // APP_HPP
