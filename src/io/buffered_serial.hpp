#ifndef BUFFERED_SERIAL_HPP
#define BUFFERED_SERIAL_HPP

#include "../util/circular_buffer.hpp"
#include "mbed.h"
#include <array>
#include <cstdarg>
#include <cstdio>

namespace sseq {
  namespace io {
    template <std::size_t N> class buffered_serial {
      util::circular_buffer<char, N> buffer{};
      Serial s;

    public:
      buffered_serial(PinName tx, PinName rx, int baud = 9600)
          : s(tx, rx, baud) {}

      inline bool try_write() {
        if (!buffer.empty() && s.writeable()) {
          s.putc(buffer.pop());
          return true;
        }
        return false;
      }

      inline void flush() {
        while (!buffer.empty()) {
          try_write();
        }
      }

      inline void clear() { buffer.clear(); }

      inline size_t num_free(){
        return buffer.num_free();
      }

      inline bool try_putc(char c) {
        if (buffer.full()) {
          return false;
        }
        buffer.push(c);
        return true;
      }

      inline bool try_puts(const char *s) {
        size_t n = 0;
        while (s[n] != '\0') {
          n++;
        }
        return buffer.try_push_array(s, n);
      }

      inline bool try_put_array(const char * s, size_t n){
        return buffer.try_push_array(s, n);
      }

      inline void putc(char c) {
        while (buffer.full()) {
          try_write();
        }
        buffer.push(c);
      }

      inline void puts(const char *s) {
        size_t i = 0;
        while (s[i] != '\0') {
          putc(s[i++]);
        }
      }

      inline bool try_printf(const char *format, ...) {
        std::va_list args;
        va_start(args, format);
        std::array<char, N> arr;
        size_t n = std::vsnprintf(arr.data(), buffer.num_free(), format, args);
        if (n >= 0 && n < buffer.num_free()) {
          return buffer.try_push_array(arr.data(), n);
        }
        return false;
      }
    };
  } // namespace io
} // namespace sseq

#endif
