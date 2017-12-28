#include "io/buffered_serial.hpp"
#include "io/encoder.hpp"
#include "io/grid.hpp"
#include "sacroseq.hpp"

int main() {
  using namespace sseq;

  io::buffered_serial<128> pc(midi::pins::TX, midi::pins::RX, midi::BAUD_RATE);
  pc.try_printf("hello!\n");
  pc.flush();

  io::grid<grid::NUM_SOURCES, grid::NUM_INPUTS, grid::NUM_OUTPUTS> grid_io(
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
      {DigitalOut(grid::pins::LED)});

  io::encoder encoder;

  while (true) {
    grid_io.step();
    for (size_t i = 0; i < grid::NUM_SOURCES; i++) {
      grid_io.set(i, 0, grid_io.get(i, 0));

      pc.try_printf("%d%d%d%d\t", grid_io.get(i, 0), grid_io.get(i, 1), grid_io.get(i, 2),
                    grid_io.get(i, 3));
    }
    encoder.update(grid_io.get(0, 1), grid_io.get(0, 2));
    pc.try_printf("%d", encoder.peek());
    pc.try_putc('\n');
    pc.flush();
  }

  alerts::done();
}
