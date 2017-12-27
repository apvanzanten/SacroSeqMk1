#include "sacroseq.hpp"

int main() {
  using namespace sseq;

  Serial pc(midi::pins::TX, midi::pins::RX, midi::BAUD_RATE);
  pc.printf("hello!\n");

  alerts::done();
}
