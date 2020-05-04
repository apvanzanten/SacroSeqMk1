#ifndef GRID_HPP
#define GRID_HPP

#include "mbed.h"
#include <array>
#include <algorithm>

namespace sseq {
  namespace io {
    template <int NUM_SOURCES, int NUM_INPUTS, int NUM_OUTPUTS> class Grid {
      std::array<DigitalOut, NUM_SOURCES> sources;
      std::array<DigitalIn, NUM_INPUTS> inputs;
      std::array<DigitalOut, NUM_OUTPUTS> outputs;

      std::array<std::array<bool, NUM_INPUTS>, NUM_SOURCES> inValues;
      std::array<std::array<bool, NUM_OUTPUTS>, NUM_SOURCES> desiredOutValues;

      size_t xIndex;

      void readInputs() {
        std::transform(inputs.begin(), inputs.end(), inValues[xIndex].begin(), 
                        [](auto & e){
                          return e.read();
                        });
      }

      void switchToNextVertical() {
        sources[xIndex] = 0;
        if (++xIndex == NUM_SOURCES) {
          xIndex = 0;
        }
        sources[xIndex] = 1;
      }

      void writeOutputs() {
        std::copy(desiredOutValues[xIndex].begin(), 
                  desiredOutValues[xIndex].end(), 
                  outputs.begin());
      }

    public:
      Grid(std::array<DigitalOut, NUM_SOURCES> sources, 
           std::array<DigitalIn, NUM_INPUTS> inputs,
           std::array<DigitalOut, NUM_OUTPUTS> outputs)
          : sources(sources), inputs(inputs), outputs(outputs), inValues(), desiredOutValues(),
            xIndex(0) {}
      

      void step() {
        readInputs();
        switchToNextVertical();
        writeOutputs();
      }

      inline bool get(size_t x, size_t y) const { return inValues[x][y]; }

      inline void set(size_t x, size_t y, bool newVal = true) { desiredOutValues[x][y] = newVal; }
      inline bool getDesired(size_t x, size_t y) const { return desiredOutValues[x][y]; }

      inline void clear(size_t x, size_t y) { desiredOutValues[x][y] = false; }

      inline const bool &getRef(size_t x, size_t y) const { return inValues[x][y]; }
    };
  } // namespace io
} // namespace sseq
#endif
