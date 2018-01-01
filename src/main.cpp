#include "io/buffered_serial.hpp"
#include "sacroseq.hpp"
#include "sequencer.hpp"

using namespace sseq;

DigitalOut board_led(LED1);

io::buffered_serial<128> pc(midi::pins::TX, midi::pins::RX, midi::BAUD_RATE);
sequencer seq;

midi::note current_note = midi::note::c4;
std::uint8_t current_velocity = 0;
volatile size_t msg_count = 0;

void step() {
  board_led = 1;
  pc.try_write();
  if (seq.update_and_check_buffer()) {
    const auto msg = seq.get_note();
    current_note = msg.note;
    current_velocity = msg.velocity;
    ++msg_count;
  }

  board_led = 0;
}

int main() {
  pc.try_printf("hello!\n");
  pc.flush();

  seq.set_bpm(120);
  seq.set_gate_time(0.9);

  seq.set_note(0, midi::note::a2);
  seq.set_repetitions(0, 3);
  seq.set_gate_mode(0, sequencer::gate_mode::pulse_once);
  seq.set_activity(0, true);
  seq.set_note(1, midi::note::b2);
  seq.set_activity(1, true);
  seq.set_note(2, midi::note::c3);
  seq.set_activity(2, true);
  seq.set_note(3, midi::note::a3);
  seq.set_activity(3, true);
  seq.set_note(4, midi::note::b3);
  seq.set_activity(4, true);
  seq.set_note(5, midi::note::c4);
  seq.set_activity(5, true);
  seq.set_note(6, midi::note::a4);
  seq.set_activity(6, true);
  seq.set_note(7, midi::note::b4);
  seq.set_activity(7, true);

  Timer tmr;
  tmr.start();

  Ticker t;
  t.attach_us(&step, MAIN_CLOCK_PERIOD_US);


  size_t printed_msg_count = 0;
  while (true) {
    while (msg_count > printed_msg_count) {
      pc.try_printf("%d,%d:%d,%d\n", tmr.read_ms(), msg_count, current_note, current_velocity);
      printed_msg_count++;
    }
  }

  alerts::done();
}
