#include "app.hpp"
#include "USBSerial.h"

using namespace sseq;

DigitalOut board_led(LED1);

USBSerial pc;
app a;

void step() {
  board_led = 1;
  a.step();
  board_led = 0;
}

int main() {
  pc.printf("hello!\n");

  Ticker ticker;
  ticker.attach_us(&step, MAIN_CLOCK_PERIOD_US);


  while(true){
    a.update();
    pc.printf("updated\n");
  }


  alerts::done();
}
