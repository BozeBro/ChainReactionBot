
#include "chainreaction.hpp"
#include "mcts.hpp"
#include <iostream>
#include <string_view>
#include <vector>

int main(int argc, char *argv[]) {

  if (argc != 3) {
    std::cout << "Need to provide 2 arguements\n";
    return -1;
  }
  srand(time(NULL));

  int width = std::stoi(argv[1]);
  int height = std::stoi(argv[2]);
  std::vector<std::string_view> players{"Red", "Blue"};
  auto agent_game = new MCTS<ChainReaction>(nullptr, width, height, players);
  agent_game->run();
  std::cout << (*agent_game) << '\n';

  auto m = MonteAgent<ChainReaction>(agent_game);
  bool turn = true;
  ChainReaction game(width, height, players);
  std::cout << game << "\n";
  while (!game.get_winner().has_value()) {
    if (turn) {
      std::cout << "It's Your turn!\n";
      int x;
      int y;
      std::cin >> x;
      std::cin >> y;
      Move move{x, y, game.get_player()};
      std::cout << move << '\n';
      m.move(move);
      game = game.nextState(move);
    } else {
      std::cout << "It's the bot's turn!\n";
      auto move = m.get_move();
      std::cout << "BOT MOVE IS: " << move << '\n';
      m.move(move);
      game = game.nextState(move);
    }
    std::cout << "THE STATE IS NOW \n";
    std::cout << game << "\n";
    turn = !turn;
  }
  std::cout << "THE WINNER IS " << game.get_winner().value() << '\n';

  return 0;
}
