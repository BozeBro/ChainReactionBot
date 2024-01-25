
#include "chainreaction.hpp"
#include "mcts.hpp"
#include <cassert>
#include <iostream>
#include <queue>
#include <string_view>
#include <vector>

int main(int argc, char *argv[]) {

  if (argc != 3) {
    std::cout << "Need to provide 2 arguements\n";
    return -1;
  }

  int width = std::stoi(argv[1]);
  int height = std::stoi(argv[2]);
  std::vector<std::string_view> players{"Red", "Blue"};
  auto m = MCTS<ChainReaction>(nullptr, width, height, players);
  ChainReaction c(width, height, players);

  m.run();
  print(&m);
  std::cout << m << '\n';
  std::cout << m.get_score() << '\n';

  // std::cout << game->get_score().value() << " "
  //           << game->get_state().get_player() << '\n';
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
      if (!nxt.get_winner().has_value()) {
        if (nxt.get_winner() == "Red") {
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
