#include "io/buffered_serial.hpp"
#include "sacroseq.hpp"

using namespace sseq;

DigitalOut board_led(LED1);

io::buffered_serial<128> pc(midi::pins::TX, midi::pins::RX, midi::BAUD_RATE);
io::buffered_serial<disp::BUFFER_SIZE> displ(disp::pins::TX, disp::pins::RX, disp::BAUD_RATE);

void step() {
  board_led = 1;
  displ.try_write();
  pc.try_write();
  board_led = 0;
}

int main() {
  pc.try_printf("hello!\n");
  pc.flush();

  displ.try_putc(disp::CLEAR_SCREEN);
  displ.try_printf("hii");
  displ.flush();

  wait(1);

  Ticker t;
  t.attach_us(&step, MAIN_CLOCK_PERIOD_US);


  while (true) {
    wait(0.5);
  }
  // size_t i = 0;
  // while (true) {
  //   i++;
  //   disply.put_msg("")
  //   pc.try_putc('\n');
  //   wait(0.005);
  // }

  alerts::done();
}
