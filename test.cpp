
#include "chainreaction.hpp"
#include "montetree.hpp"
#include <cassert>
#include <queue>

int main(int argc, char *argv[]) {
  if (argc != 3) {
    std::cout << "Need to provide 2 arguements\n";
    return -1;
  }

  int width = std::stoi(argv[1]);
  int height = std::stoi(argv[2]);
  ChainReaction c(width, height, {"Blue", "Red"});
  auto game = MonteTree<ChainReaction>::create(c);
  game->run();
  std::cout << game->get_score().value() << " "
            << game->get_state().get_player() << '\n';
  std::queue<ChainReaction> que;
  int rwin = 0;
  int bwin = 0;
  int mcount = 0;
  que.push(c);
  while (!que.empty()) {
    ChainReaction top = que.front();
    que.pop();
    for (auto move : top.legalMoves(top.get_player())) {
      mcount++;
      ChainReaction nxt = top.nextState(move);
      auto winner = nxt.is_win(top.get_player());
      assert(nxt.is_win(top.get_player()) ==
             (nxt.get_winner() == top.get_player()));
      if (winner) {
        if (top.get_player() == "Red") {
          rwin++;
        } else {
          bwin++;
        }
        continue;
      }
      que.push(nxt);
    }
  }
  std::cout << "Red WIN: " << rwin << " Blue Win: " << bwin << '\n';
  std::cout << mcount << '\n';

  return 0;
}
