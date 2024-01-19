
#include "chainreaction.hpp"
#include "montetree.hpp"

int main(int argc, char *argv[]) {
  if (argc != 3) {
    std::cout << "Need to provide 2 arguements\n";
    return -1;
  }

  int width = std::stoi(argv[1]);
  int height = std::stoi(argv[2]);
  ChainReaction c(width, height, {"Red", "Blue"});
  auto game = MonteTree<ChainReaction>::create(c, "Red");
  game->run();

  return 0;
}
