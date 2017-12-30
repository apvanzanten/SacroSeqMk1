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

  io::encoder enc;
  const size_t enc_index = 0;
  const size_t enc_a_index = 1;
  const size_t enc_b_index = 2;
  
  const bool &enc_a_val = grid_io.get_ref(enc_index, enc_a_index);
  const bool &enc_b_val = grid_io.get_ref(enc_index, enc_b_index);

  while (true) {
    enc.update(enc_a_val, enc_b_val);
    pc.try_printf("%d%d: %d", enc_a_val, enc_b_val, enc.peek());
    pc.try_putc('\n');
    wait(0.001);
  }

  alerts::done();
}
