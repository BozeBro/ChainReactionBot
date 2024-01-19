
#include "chainreaction.hpp"
#include <cassert>
#include <vector>
using std::vector;
bool test_chain() {
  ChainReaction c(2, 2, {"Red", "Blue"});
  vector<vector<Move>> tests = {
      {{{0, 0, "Red"}, {0, 1, "Blue"}, {0, 0, "Red"}},
       {{1, 1, "Red"}, {1, 0, "Blue"}, {1, 1, "Red"}}}};
  for (auto game : tests) {
    ChainReaction instance(c);
    for (auto move : game) {
      assert(!c.is_win(move.color) && "Expected to not win yet");
      c = c.nextState(move);
    }
    Move m = *game.rbegin();
    assert(c.is_win(game.rbegin()->color) && "Game should have been won!");
  }

  return true;
}
int main() {
  test_chain();
  return 0;
}
