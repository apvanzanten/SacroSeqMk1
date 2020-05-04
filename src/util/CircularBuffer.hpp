#ifndef CIRCULARBUFFER_HPP
#define CIRCULARBUFFER_HPP

#include <array>
#include <optional>

namespace sseq {
  namespace util {

    template <typename T, std::size_t N> class CircularBuffer {
      std::array<T, N> buffer;
      size_t first = 0;
      size_t last = 0;
      bool _isEmpty = true;

      inline void increment(size_t &index) { index = (index + 1) % N; }

      inline size_t getNumFilled() const {
        return (last == first) ? (isEmpty() ? 0 : N)
                               : ((last > first) ? (last - first) : (N - (first - last)));
      }

    public:
      inline bool isEmpty() const { return _isEmpty; }
      inline bool isFull() const { return (first == last && !isEmpty()); }
      inline size_t getCapacity() const { return N; }
      inline size_t getNumFree() const { return getCapacity() - getNumFilled(); }

      inline void push(const T &e) {
        if (isFull()) {
          increment(first);
        }
        buffer[last] = e;
        increment(last);
        _isEmpty = false;
      }

      inline bool tryPush(const T &e) {
        if(!isFull()){
          push(e);
          return true;
        }
        return false;
      }

      inline bool tryPushArray(const T *arr, size_t n) {
        if (getNumFree() >= n) {
          for (size_t i = 0; i < n; i++) {
            buffer[last] = arr[i];
            increment(last);
          }
          _isEmpty = false;
          return true;
        }
        return false;
      }

      inline T pop() {
        const T &ret = buffer[first];
        increment(first);
        if (isEmpty()) {
          increment(last);
        }
        _isEmpty = (first == last);
        return ret;
      }

      inline std::optional<T> tryPop() {
        return isEmpty() ? std::nullopt : pop();
      }

      inline void clear() {
        _isEmpty = true;
        first = last = 0;
      }
    };
  } // namespace util
} // namespace sseq
#endif
