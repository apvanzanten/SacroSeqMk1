#include "io/buffered_serial.hpp"
#include "sacroseq.hpp"
#include "sequencer.hpp"

using namespace sseq;

DigitalOut board_led(LED1);

io::buffered_serial<128> pc(midi::pins::TX, midi::pins::RX, midi::BAUD_RATE);
sequencer seq;

midi::note current_note = midi::note::c4;
std::uint8_t current_velocity = 0;
volatile size_t m_count = 0;

void step() {
  board_led = 1;
  pc.try_write();
  if (seq.update_and_check_buffer()) {
    const auto m = seq.get_note();
    current_note = m.note;
    current_velocity = m.velocity;
    ++m_count;
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

  size_t printed_m_count = 0;
  while (true) {
    while (m_count > printed_m_count) {
      if (current_velocity > 0) {
        const auto m = midi::make_note_on(0, current_note, current_velocity);
        pc.try_printf("%d ON :\t%x\t%x\t%x\n", tmr.read_ms(), m.status, m.note, m.velocity);
      } else {
        const auto m = midi::make_note_off(0, current_note);
        pc.try_printf("%d OFF:\t%x\t%x\t%x\n", tmr.read_ms(), m.status, m.note, m.velocity);
      }
      printed_m_count++;
    }
  }

  alerts::done();
}
