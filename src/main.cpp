#include "interface.hpp"

using namespace sseq;

DigitalOut board_led(LED1);

io::buffered_serial<128> pc(midi::pins::TX, midi::pins::RX, midi::BAUD_RATE);
interface interf;

void step() {
  board_led = 1;
  pc.try_write();
  interf.step();
  board_led = 0;
}

int main() {
  pc.try_printf("hello!\n");
  pc.flush();

  Ticker ticker;
  ticker.attach_us(&step, MAIN_CLOCK_PERIOD_US);

  size_t num_buttons = 20;
  size_t num_enc = 8;

  while(true){
    pc.try_putc('C');
    for(size_t i = 0; i < num_buttons; i++){
      pc.try_putc('0' + interf.get_and_reset_button_clicked(i));
    }
    pc.try_putc('\t');
    pc.try_putc('H');
    for(size_t i = 0; i < num_buttons; i++){
      pc.try_putc('0' + interf.get_button_held(i));
    }

    pc.try_putc('\t');
    pc.try_putc('E');
    for(size_t i = 0; i < num_enc; i++){
      pc.try_putc('0' + interf.get_enc_val(i) % 8);
    }
    pc.try_putc('\n');
    wait(0.05);
  }


  alerts::done();
}
