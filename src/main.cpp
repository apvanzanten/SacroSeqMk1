#include "app.hpp"
#include "USBSerial.h"

using namespace sseq;

DigitalOut board_led(LED1); // LED on the teensy31
DigitalOut update_indicator(D19); // pin used to indicate update activity

USBSerial pc; // serial over USB port on teensy31
app a; // actual sequencer app

void greet(){
  for (int i = 0; i < 5; i++) {
    board_led = 1;
    wait(0.1);
    board_led = 0;
    wait(0.1);
  }

  pc.printf("hello!\n");
}

// function is called every MAIN_CLOCK_PEIOD_US microseconds
void step() {
  // turns led on/off around a.step() so its runtime can be measured
  board_led = 1;
  a.step();
  board_led = 0;
}

void update() {
  // turns indicator on/off around a.update() so its runtime can be measured
  update_indicator = 1;
  a.update();
  update_indicator = 0;
}

int main() {
  greet();

  Ticker ticker;
  ticker.attach_us(&step, MAIN_CLOCK_PERIOD_US);

  while(true){
    update();
  }


  alerts::done();
}
