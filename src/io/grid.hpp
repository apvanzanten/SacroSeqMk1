#ifndef GRID_HPP
#define GRID_HPP

#include "mbed.h"
#include <array>
#include <algorithm>

namespace sseq {
  namespace io {
    template <int NUM_SOURCES, int NUM_INPUTS, int NUM_OUTPUTS> class grid {
      std::array<DigitalOut, NUM_SOURCES> sources;
      std::array<DigitalIn, NUM_INPUTS> inputs;
      std::array<DigitalOut, NUM_OUTPUTS> outputs;

      std::array<std::array<bool, NUM_INPUTS>, NUM_SOURCES> in_values;
      std::array<std::array<bool, NUM_OUTPUTS>, NUM_SOURCES> desired_out_values;

      size_t x_index;

      void read_inputs() {
        std::transform(inputs.begin(), inputs.end(), in_values[x_index].begin(), 
                        [](auto & e){
                          return e.read();
                        });
      }

      void switch_to_next_vertical() {
        sources[x_index] = 0;
        if (++x_index == NUM_SOURCES) {
          x_index = 0;
        }
        sources[x_index] = 1;
      }

      void write_outputs() {
        std::copy(desired_out_values[x_index].begin(), 
                  desired_out_values[x_index].end(), 
                  outputs.begin());
      }

    public:
      grid(std::array<DigitalOut, NUM_SOURCES> sources, 
           std::array<DigitalIn, NUM_INPUTS> inputs,
           std::array<DigitalOut, NUM_OUTPUTS> outputs)
          : sources(sources), inputs(inputs), outputs(outputs), in_values(), desired_out_values(),
            x_index(0) {}
      

      void step() {
        read_inputs();
        switch_to_next_vertical();
        write_outputs();
      }

      inline bool get(size_t x, size_t y) const { return in_values[x][y]; }

      inline void set(size_t x, size_t y, bool newVal = true) { desired_out_values[x][y] = newVal; }
      inline bool get_desired(size_t x, size_t y) const { return desired_out_values[x][y]; }

      inline void clear(size_t x, size_t y) { desired_out_values[x][y] = false; }

      inline const bool &get_ref(size_t x, size_t y) const { return in_values[x][y]; }
    };
  } // namespace io
} // namespace sseq
#endif
