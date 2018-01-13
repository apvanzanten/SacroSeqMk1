#include "app.hpp"
#include "USBSerial.h"

using namespace sseq;

DigitalOut board_led(LED1);

USBSerial pc;
app a;

void greet(){
  for (int i = 0; i < 5; i++) {
    board_led = 1;
    wait(0.1);
    board_led = 0;
    wait(0.1);
  }

  pc.printf("hello!\n");
}

void step() {
  board_led = 1;
  a.step();
  board_led = 0;
}

int main() {
  greet();

  Ticker ticker;
  ticker.attach_us(&step, MAIN_CLOCK_PERIOD_US);


  while(true){
    a.update();
    pc.printf("updated\n");
  }


  alerts::done();
}
