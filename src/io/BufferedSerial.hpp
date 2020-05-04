#ifndef BUFFEREDSERIAL_HPP
#define BUFFEREDSERIAL_HPP

#include "../util/CircularBuffer.hpp"
#include "mbed.h"
#include <array>
#include <cstdarg>
#include <cstdio>
#include <optional>

namespace sseq {
  namespace io {
    template <std::size_t TX_BUFFER_SIZE, std::size_t RX_BUFFER_SIZE = 0> class BufferedSerial {
      util::CircularBuffer<char, TX_BUFFER_SIZE> txBuffer{};
      util::CircularBuffer<char, RX_BUFFER_SIZE> rxBuffer{};
      Serial serial;

    public:
      BufferedSerial(PinName tx, PinName rx, int baud = 9600)
          : serial(tx, rx, baud) {}

      inline bool tryWriteFromBuffer() {
        if (!txBuffer.isEmpty() && serial.writeable()) {
          serial.putc(txBuffer.pop());
          return true;
        }
        return false;
      }

      inline void flush() {
        while (!txBuffer.isEmpty()) {
          tryWriteFromBuffer();
        }
      }

      inline bool tryPutC(char c) {
        if (txBuffer.isFull()) {
          return false;
        }
        txBuffer.push(c);
        return true;
      }

      inline bool tryPutS(const char *s) {
        size_t n = 0;
        while (s[n] != '\0') {
          n++;
        }
        return txBuffer.tryPushArray(s, n);
      }

      inline void putC(char c) {
        while (txBuffer.isFull()) {
          tryWriteFromBuffer();
        }
        txBuffer.push(c);
      }

      inline void putS(const char *s) {
        size_t i = 0;
        while (s[i] != '\0') {
          putC(s[i++]);
        }
      }

      inline bool tryReadToBuffer() {
        if (!rxBuffer.isFull() && serial.readable()) {
          rxBuffer.push(static_cast<char>(serial.getc()));
          return true;
        }
        return false;
      }

      inline void readUntilBufferFull() {
        while (!rxBuffer.isFull()) {
          tryReadToBuffer();
        }
      }

      inline bool tryReadUntilBufferFull() {
        while(tryReadToBuffer()) { }
        return rxBuffer.isFull();
      }

      inline std::optional<char> tryGetC() {
        if(!rxBuffer.isEmpty()){
          return rxBuffer.pop();
        }
        return {};
      }

    };
  } // namespace io
} // namespace sseq

#endif
