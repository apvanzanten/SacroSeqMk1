#ifndef BUTTON_HPP
#define BUTTON_HPP

#include "../sacroseq.hpp"

namespace sseq {
  namespace io {
    class button {
      enum class states { initial, wait_for_debounce, held } state;
      bool _is_held = false;
      bool _is_clicked = false;
      bool prev_input = false;
      int debounce_end = 0;
      Timer & t;
      const bool &input;
      int debounce_ms;


    public:
      button(Timer & t, const bool &input, int debounce_ms = buttons::DEBOUNCE_MS);

      void update();

      inline bool is_held() const { return _is_held; }
      inline bool is_clicked() const { return _is_clicked; }
      inline void reset_clicked() { _is_clicked = false; }
    };
  } // namespace io
} // namespace sseq

#endif
