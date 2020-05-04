#ifndef STATEMACHINE_HPP
#define STATEMACHINE_HPP

#include <array>
#include <functional>
#include <optional>
#include <vector>

namespace sseq::util {
  using Guard = std::function<bool()>;
  using Action = std::function<void()>;

  template <typename StateEnum, typename EventEnum> class StateMachine {
  public:
    struct Transition {
      EventEnum event;
      std::vector<Guard> guards;
      std::vector<Action> actions;
      StateEnum target;
    };

    using TransitionList = std::vector<Transition>;
    using TransitionMatrix =
        std::array<TransitionList, static_cast<std::size_t>(StateEnum::NUM_STATES)>;

  private:
    TransitionMatrix transitionMatrix;
    StateEnum current_state = StateEnum::INITIAL;

    std::size_t getCurrentStateIndex() {
      return static_cast<std::size_t>(current_state);
    }

    std::optional<Transition> find_relevant_transition(EventEnum event) {
      const auto begin = transitionMatrix[getCurrentStateIndex()].begin();
      const auto end = transitionMatrix[getCurrentStateIndex()].end();

      const auto it = std::find_if(begin, end, [&event](const auto &t) {
        return (t.event == event // the correct event
                && std::all_of(t.guards.begin(), t.guards.end(),
                               [](const auto &g) { return g(); }) // all guards hold
        );
      });

      if (it != end) {
        return *it;
      }
      return {};
    }

  public:
    StateMachine(TransitionMatrix transitionMatrix) : transitionMatrix(transitionMatrix) {}

    void trigger(EventEnum event) {
      const auto t_opt = find_relevant_transition(event);

      if (t_opt.has_value()) {
        const auto &t = t_opt.value();
        for (const auto &a : t.actions) {
          a();
        }

        current_state = t.target;
      }
    }

    StateEnum getState() const { return current_state; }
  };
} // namespace sseq::util

#endif