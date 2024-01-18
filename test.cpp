
#include "montecarlo.hpp"
#include <iostream>
#include <string>
int main(int argc, char *argv[]) {
  if (argc != 3) {
    return -1;
  }
  int width = std::stoi(argv[1]);
  int height = std::stoi(argv[2]);
  ChainReaction chain(width, height);
  auto grid = chain.get_grid();
  Move move = {"Red", 0, 0};
  std::cout << (chain.nextState(move)
                    .nextState(move)
                    .nextState(move)
                    .nextState(move)
                    .nextState(move)
                    .nextState(move)
                    .nextState(move)
                    .nextState(move)
                    .nextState(move)
                    .nextState(move));

  return 0;
}
