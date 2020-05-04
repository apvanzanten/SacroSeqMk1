#ifndef BUTTON_HPP
#define BUTTON_HPP

#include "../sacroseq.hpp"

namespace sseq {
  namespace io {
    class Button {
      enum class States { idle, waitForDebounce, held } state;
      bool _isHeld = false;
      bool _isClicked = false;
      bool prevInput = false;
      int debounceEnd = 0;
      Timer & tmr;
      const bool &input;
      int debounceMs;


    public:
      Button(Timer & t, const bool &input, int debounceMs = buttons::DEBOUNCE_MS);

      void update();

      inline bool isHeld() const { return _isHeld; }
      inline bool isClicked() const { return _isClicked; }
      inline void resetClicked() { _isClicked = false; }
    };
  } // namespace io
} // namespace sseq

#endif
