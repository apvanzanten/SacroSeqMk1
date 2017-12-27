#include "sacroseq.hpp"
#include "io/buffered_serial.hpp"

int main() {
  using namespace sseq;

  io::buffered_serial<32> pc(midi::pins::TX, midi::pins::RX, midi::BAUD_RATE);
  pc.try_printf("hello!\n");
  pc.flush();

  alerts::done();
}
