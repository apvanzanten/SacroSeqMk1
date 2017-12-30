#include "io/buffered_serial.hpp"
#include "io/button.hpp"
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

  Timer tmr;
  tmr.start();

  std::array<io::button, 8> buttons = {
      io::button(tmr, grid_io.get_ref(0, 3)), io::button(tmr, grid_io.get_ref(1, 3)),
      io::button(tmr, grid_io.get_ref(2, 3)), io::button(tmr, grid_io.get_ref(3, 3)),
      io::button(tmr, grid_io.get_ref(4, 3)), io::button(tmr, grid_io.get_ref(5, 3)),
      io::button(tmr, grid_io.get_ref(6, 3)), io::button(tmr, grid_io.get_ref(7, 3)),
  };

  std::array<size_t, 8> clicks;
  for (auto &e : clicks) {
    e = 0;
  }

  while (true) {
    for (size_t i = 0; i < buttons.size(); i++) {
      buttons[i].update();
      clicks[i] += buttons[i].is_clicked();
      buttons[i].reset_clicked();
      pc.try_printf("%d,%d\t", buttons[i].is_held(), clicks[i]);
    }

    pc.try_putc('\n');
    wait(0.005);
  }

  alerts::done();
}
