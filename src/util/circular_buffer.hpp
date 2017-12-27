#ifndef CIRCULAR_BUFFER_HPP
#define CIRCULAR_BUFFER_HPP

#include <array>

namespace sseq {
  namespace util {

    template <typename T, std::size_t N> class circular_buffer {
      std::array<T, N> buffer;
      size_t first = 0;
      size_t last = 0;
      bool isEmpty = true;

      inline void increment(size_t &index) { index = (index + 1) % N; }

    public:
      inline bool empty() const { return isEmpty; }
      inline bool full() const { return (first == last && !isEmpty); }
      inline size_t num_filled() const {
        if (last == first) {
          if (empty()) {
            return 0;
          }
          return N;
        }
        if (last < first) {
          return N - (first - last);
        }
        return last - first;
      }
      inline size_t capacity() const { return N; }
      inline size_t num_free() const { return capacity() - num_filled(); }

      inline void push(const T &e) {
        if (full()) {
          increment(first);
        }
        buffer[last] = e;
        increment(last);
        isEmpty = false;
      }

      inline bool try_push_array(const T *arr, size_t n) {
        if (num_free() >= n) {
          for (size_t i = 0; i < n; i++) {
            buffer[last] = arr[i];
            increment(last);
          }
          isEmpty = false;
          return true;
        }
        return false;
      }

      inline T pop() {
        const T &ret = buffer[first];
        increment(first);
        if (empty()) {
          increment(last);
        }
        isEmpty = (first == last);
        return ret;
      }

      inline void clear() {
        isEmpty = true;
        first = last = 0;
      }
    };
  } // namespace util
} // namespace sseq
#endif
