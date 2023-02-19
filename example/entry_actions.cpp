#include <boost/sml.hpp>
#include <iostream>

namespace sml = boost::sml;

// events
struct next {};

// states
struct s1 {};
struct s2 {};

/* clang-format on */

auto print = []() { std::cout << "lambda triggered\n"; };

void free_print() { std::cout << "free_print triggered\n"; }

class Machine {
  using Self = Machine;
  int data{0};

 public:
  auto operator()() {
    using namespace sml;
    /* clang-format off */
    return make_transition_table(
      * "idle"_s      + on_entry<_> / print  // lambda expression works
      // free functions and static members need to be wrapped
      , state<s1>     + on_entry<_> / wrap(Self::static_print)
      , state<s2>     + on_entry<_> / wrap(free_print)
      , "idle"_s      + event<next> = state<s1>
      , state<s1>     + event<next> = state<s2>
      , state<s2>     + event<next> = state<s1>
    );
    /* clang-format on */
  }

 private:
  static void static_print() { std::cout << "static_print triggered\n"; };
};

int main(int, char**) {
  Machine m;
  sml::sm<Machine> sm(m);

  while (true) {
    std::string input;
    std::cout << "Press [Enter] to continue, [q] to quit";
    std::getline(std::cin, input);
    if (input == "q") break;

    sm.process_event(next{});
  }
  return 0;
}