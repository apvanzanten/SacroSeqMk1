#ifndef ENCODER_HPP
#define ENCODER_HPP
namespace sseq {
  namespace io {
    class encoder {
      enum class states {
        initial,
        clockwise_1,
        clockwise_2,
        counter_clockwise_1,
        counter_clockwise_2,
        error
      } state = states::initial;

      int delta = 0;
      bool prev_a = false;
      bool prev_b = false;

    public:
      inline int read() {
        auto ret = delta;
        delta = 0;
        return ret;
      }

      inline int peek() const { return delta; }
      void update(bool a, bool b);
    };
  } // namespace io
} // namespace sseq
#endif
