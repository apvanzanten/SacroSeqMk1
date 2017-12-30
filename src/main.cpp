#include "io/buffered_serial.hpp"
#include "io/encoder.hpp"
#include "io/grid.hpp"
#include "sacroseq.hpp"

using namespace sseq;

DigitalOut board_led(LED1);

io::buffered_serial<128> pc(midi::pins::TX, midi::pins::RX, midi::BAUD_RATE);
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

void step() {
  board_led = 1;
  grid_io.step();
  pc.try_write();
  board_led = 0;
}

int main() {
  pc.try_printf("hello!\n");
  pc.flush();

  Ticker t;
  t.attach_us(&step, MAIN_CLOCK_PERIOD_US);

  const size_t enc_a_index = 1;
  const size_t enc_b_index = 2;

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

  while (true) {
    for (auto &enc : encoders) {
      enc.update();
      pc.try_printf("%d\t", enc.peek());
    }
    pc.try_putc('\n');
    wait(0.01);
  }

  alerts::done();
}
