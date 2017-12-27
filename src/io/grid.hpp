#ifndef GRID_HPP
#define GRID_HPP

#include "mbed.h"
#include <array>

namespace sseq {
  namespace io {
    template <std::size_t NUM_SOURCES, std::size_t NUM_INPUTS, std::size_t NUM_OUTPUTS> class grid {
      std::array<DigitalOut, NUM_SOURCES> sources;
      std::array<DigitalIn, NUM_INPUTS> inputs;
      std::array<DigitalOut, NUM_OUTPUTS> outputs;

      std::array<std::array<bool, NUM_INPUTS>, NUM_SOURCES> in_values;
      std::array<std::array<bool, NUM_OUTPUTS>, NUM_SOURCES> desired_out_values;

      size_t x_index;

    public:
      grid(std::array<DigitalOut, NUM_SOURCES> sources, std::array<DigitalIn, NUM_INPUTS> inputs,
           std::array<DigitalOut, NUM_OUTPUTS> outputs)
          : sources(sources), inputs(inputs), outputs(outputs), in_values(), desired_out_values(),
            x_index(0) {}

      inline void step() {
        // read inputs
        for (size_t i = 0; i < NUM_INPUTS; i++) {
          in_values[x_index][i] = inputs[i].read();
        }

        // switch to next vertical
        sources[x_index] = 0;
        if (++x_index == NUM_SOURCES) {
          x_index = 0;
        }
        sources[x_index] = 1;

        // write outputs
        for (size_t i = 0; i < NUM_OUTPUTS; i++) {
          outputs[i] = desired_out_values[x_index][i];
        }
      }

      inline bool get(size_t x, size_t y) const { return in_values[x][y]; }

      inline void set(size_t x, size_t y, bool newVal = true) { desired_out_values[x][y] = newVal; }

      inline void clear(size_t x, size_t y) { desired_out_values[x][y] = false; }
    };
  } // namespace io
} // namespace sseq
#endif
