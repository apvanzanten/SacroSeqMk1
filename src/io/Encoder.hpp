#ifndef ENCODER_HPP
#define ENCODER_HPP
namespace sseq {
  namespace io {
    class Encoder {
      enum class States {
        idle,
        clockwise1,
        clockwise2,
        counterClockwise1,
        counterClockwise2,
        error
      } state = States::idle;

      int delta = 0;
      bool prevA = false;
      bool prevB = false;
      const bool &a;
      const bool &b;

    public:
      Encoder(const bool &a, const bool &b);

      inline int read() {
        auto ret = delta;
        delta = 0;
        return ret;
      }

      inline int peek() const { return delta; }
      void update();
    };
  } // namespace io
} // namespace sseq
#endif
