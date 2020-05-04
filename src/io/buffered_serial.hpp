#ifndef BUFFERED_SERIAL_HPP
#define BUFFERED_SERIAL_HPP

#include "../util/CircularBuffer.hpp"
#include "mbed.h"
#include <array>
#include <cstdarg>
#include <cstdio>
#include <optional>

namespace sseq {
  namespace io {
    template <std::size_t TX_BUFFER_SIZE, std::size_t RX_BUFFER_SIZE = 0> class buffered_serial {
      util::CircularBuffer<char, TX_BUFFER_SIZE> tx_buffer{};
      util::CircularBuffer<char, RX_BUFFER_SIZE> rx_buffer{};
      Serial s;

    public:
      buffered_serial(PinName tx, PinName rx, int baud = 9600)
          : s(tx, rx, baud) {}

      inline bool try_write_from_buffer() {
        if (!tx_buffer.isEmpty() && s.writeable()) {
          s.putc(tx_buffer.pop());
          return true;
        }
        return false;
      }

      inline void flush() {
        while (!tx_buffer.isEmpty()) {
          try_write_from_buffer();
        }
      }

      inline bool try_putc(char c) {
        if (tx_buffer.isFull()) {
          return false;
        }
        tx_buffer.push(c);
        return true;
      }

      inline bool try_puts(const char *s) {
        size_t n = 0;
        while (s[n] != '\0') {
          n++;
        }
        return tx_buffer.tryPushArray(s, n);
      }

      inline void putc(char c) {
        while (tx_buffer.isFull()) {
          try_write_from_buffer();
        }
        tx_buffer.push(c);
      }

      inline void puts(const char *s) {
        size_t i = 0;
        while (s[i] != '\0') {
          putc(s[i++]);
        }
      }

      inline bool try_read_to_buffer() {
        if (!rx_buffer.isFull() && s.readable()) {
          rx_buffer.push(static_cast<char>(s.getc()));
          return true;
        }
        return false;
      }

      inline void read_until_buffer_full() {
        while (!rx_buffer.isFull()) {
          try_read_to_buffer();
        }
      }

      inline bool try_read_until_buffer_full() {
        while(try_read_to_buffer()) { }
        return rx_buffer.isFull();
      }

      inline std::optional<char> try_getc() {
        if(!rx_buffer.isEmpty()){
          return rx_buffer.pop();
        }
        return {};
      }

    };
  } // namespace io
} // namespace sseq

#endif
