#include "App.hpp"
#include "USBSerial.h"

using namespace sseq;

DigitalOut boardLed(LED1); // LED on the teensy31
DigitalOut updateIndicator(D19); // pin used to indicate update activity

USBSerial pc; // serial over USB port on teensy31
App a; // actual sequencer app

void greet(){
  for (int i = 0; i < 5; i++) {
    boardLed = 1;
    wait(0.1);
    boardLed = 0;
    wait(0.1);
  }

  pc.printf("hello!\n");
}

// function is called every MAIN_CLOCK_PEIOD_US microseconds
void step() {
  // turns led on/off around a.step() so its runtime can be measured
  boardLed = 1;
  a.step();
  boardLed = 0;
}

void update() {
  // turns indicator on/off around a.update() so its runtime can be measured
  updateIndicator = 1;
  a.update();
  updateIndicator = 0;
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
