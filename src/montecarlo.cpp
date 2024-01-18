#include "montecarlo.hpp"

#include <array>
#include <assert.h>
#include <iostream>
#include <ostream>
#include <queue>
#include <sstream>
#include <string_view>
#include <vector>

void ChainReaction::append_if_explode(std::queue<int> &que,
                                      std::vector<State> &grid, int ind,
                                      std::string_view color) {
  if (!handle_sq(grid, ind, color)) {
    grid[ind].reset();
    que.push(ind);
  }
}
bool ChainReaction::handle_sq(std::vector<State> &grid, int ind,
                              std::string_view color) {
  State &sq = grid[ind];
  sq.circles++;
  sq.color = color;
  if (ind == 0 || ind == m_grid.size() - 1 || ind == m_width - 1 ||
      ind == m_grid.size() - m_width) {
    return sq.circles < 2;
  } else if (0 < ind && ind < m_width || ind % m_width == 0 ||
             ind + m_width > m_grid.size()) {
    return sq.circles < 3;
  } else {
    return sq.circles < 4;
  }
}

bool is_adjacent(int src, int dst, int width, int height) {
  if (!(0 <= dst && dst < width * height))
    return false;
  // down
  if (src + width == dst)
    return true;
  // up
  if (src - width == dst)
    return true;
  // right - handle if wrap around forward
  if (src + 1 == dst && (src + 1) % width)
    return true;
  // left - handle if wrap around backward
  if (src - 1 == dst && src % width)
    return true;
  return false;
}

ChainReaction ChainReaction::nextState(Move move) {

  assert(0 <= move.x && move.x < m_width);
  assert(0 <= move.y && move.y < m_height);
  std::vector<State> nextGrid(m_grid);
  std::queue<int> que;
  std::string_view player_color = move.color;
  int sq = move.x + move.y * m_width;
  append_if_explode(que, nextGrid, sq, player_color);
  std::array<int, 4> move_delta{1, -1, m_width, -1 * m_width};

  // explosion
  while (!que.empty()) {
    std::queue<int> nextQue;
    while (!que.empty()) {
      int sq = que.front();
      que.pop();
      for (int delta : move_delta) {
        if (is_adjacent(sq, sq + delta, m_width, m_height)) {
          append_if_explode(nextQue, nextGrid, sq + delta, player_color);
        }
      }
    }
    que = nextQue;
  }
  return ChainReaction(m_width, m_height, nextGrid);
}

std::ostream &operator<<(std::ostream &stream, const ChainReaction &chain) {
  for (int i = 0; i < chain.m_height; i++) {
    for (int j = 0; j < chain.m_width; j++) {
      stream << chain.m_grid[i * chain.m_width + j] << " ";
    }
    stream << std::endl;
  }
  return stream;
}

std::ostream &operator<<(std::ostream &stream, const State &state) {
  std::stringstream ss;
  char m = !state.color.empty() ? state.color[0] : '0';
  ss << "(" << (m) << state.circles << ")";
  stream << ss.str();
  return stream;
}
