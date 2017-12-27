#include "mbed.h"

/* Function that indicates to the outside world that we're done doing whatever
 * we were doing, by flashing 'done' on the 7-segment display.
 */
void done() {
  Serial display(D8, D7);
  display.baud(9600);

  while (true) {
    display.putc(0x76); // clear display
    wait(0.5);
    display.printf("done");
    wait(0.5);
  }
}

int main() {

  done();
}
