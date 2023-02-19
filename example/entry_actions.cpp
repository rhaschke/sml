#include <boost/sml.hpp>
#include <iostream>

namespace sml = boost::sml;

// events
struct next {};

// states
struct s1 {
  void on_entry() { std::cout << "entered\n"; }
};
struct s2 {};

class Machine {
 public:
  auto operator()() {
    using namespace sml;
    /* clang-format off */
    return make_transition_table(
      * state<s1>     + on_entry<_> / &s1::on_entry
      , state<s1>     + event<next>                 = state<s2>
      , state<s2>     + event<next>                 = state<s1>
    );
    /* clang-format on */
  }
};

int main(int, char**) {
  Machine m;
  sml::sm<Machine> sm(m, s1{});

  while (true) {
    std::string input;
    std::cout << "Press [Enter] to continue, [q] to quit";
    std::getline(std::cin, input);
    if (input == "q") break;

    sm.process_event(next{});
  }
  return 0;
}