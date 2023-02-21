#include <boost/sml.hpp>
#include <cstdio>
#include <queue>
#include <string>

namespace sml = boost::sml;

struct next {};
struct quit {};

const auto idle = sml::state<class idle>;
const auto s1 = sml::state<class s1>;
const auto s2 = sml::state<class s2>;

struct Context {
  static int next_id;

  Context() = delete;
  // user-defined constructors
  Context(int data, const std::string& extra) : id(++next_id), data(data), extra(extra) { printf("ctor:   %d\n", id); }

  Context(const Context& c) : id(++next_id), data(c.data), extra(c.extra) { printf("copy c: %d -> %d\n", c.id, id); };
  Context(Context&& c) : id(++next_id), data(c.data), extra(std::move(c.extra)) { printf("move c: %d -> %d\n", c.id, id); };

  Context& operator=(const Context& c) = delete;
  Context& operator=(Context&& c) = delete;

  int id;
  int data;
  std::string extra;
};
int Context::next_id = 0;

int main() {
  struct sub {
    // sub must not have any user-defined constructor
    Context ctx;  // state-local data

    auto operator()() const noexcept {
      using namespace sml;
      // clang-format off
      return make_transition_table(
        * s1   + on_entry<_> / (&sub::method, process(quit{}), process(next{}))
        , s2   + on_entry<_> / (process(quit{}), process(next{}))
        , s1   + event<next> = s2
        , s2   + event<next> = s1
      );
      // clang-format on
    }
    void method() { printf("method: %d, %s (%d)\n", ctx.data, ctx.extra.c_str(), ctx.id); }
  };

  struct parent {
    sml::front::state_sm<sub>::type s;

    auto operator()() noexcept {
      using namespace sml;
      // clang-format off
      return make_transition_table(
        * idle + event<next> = s
        , s + event<quit> / [this] { printf("quitting\n"); } = X
      );
      // clang-format on
    }
  };

  Context c(42, "Hello");  // initialize context with custom ctor
  sub s{c};                // initialize sub-state by passing c via aggregate initialization

  sml::sm<parent, sml::process_queue<std::queue>> sm(s);  // inject initialized sub state into state machine
  sm.process_event(next{});
  sm.process_event(next{});  // + process(e2{})
}