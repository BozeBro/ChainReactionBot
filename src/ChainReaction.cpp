
#include "chainreaction.hpp"

#include <assert.h>
#include <cstdint>
#include <optional>
#include <ostream>
#include <sstream>
#include <string_view>
#include <vector>

namespace util {
bool is_win(std::vector<State> grid, std::string_view color) {
  int enemy_cnt = 0;
  int my_cnt = 0;
  for (const State &sq : grid) {
    if (color == sq.color) {
      my_cnt += sq.circles;
    } else {
      enemy_cnt += sq.circles;
    }
  }
  return enemy_cnt == 0;
}
int count_circles(std::vector<State> grid) {
  int cnt = 0;
  for (const State &sq : grid) {
    cnt += sq.circles;
  }
  return cnt;
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
} // namespace util

void ChainReaction::append_if_explode(std::queue<int> &que,
                                      std::vector<State> &grid, int ind,
                                      std::string_view color) const {
  if (!handle_sq(grid, ind, color)) {
    grid[ind].reset();
    que.push(ind);
  }
}
bool ChainReaction::handle_sq(std::vector<State> &grid, int ind,
                              std::string_view color) const {
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

const ChainReaction ChainReaction::nextState(Move move) const {

  assert(0 <= move.x && move.x < m_width);
  assert(0 <= move.y && move.y < m_height);
  const uint32_t cnt = m_cnt + 1;
  std::vector<State> next_grid(m_grid);
  std::queue<int> que;
  std::string_view player_color = move.color;
  int sq = move.x + move.y * m_width;
  append_if_explode(que, next_grid, sq, player_color);
  std::array<int, 4> move_delta{1, -1, m_width, -1 * m_width};

  // explosion
  while (!que.empty()) {
    std::queue<int> nextQue;
    while (!que.empty()) {
      int sq = que.front();
      que.pop();
      for (int delta : move_delta) {
        if (util::is_adjacent(sq, sq + delta, m_width, m_height)) {
          append_if_explode(nextQue, next_grid, sq + delta, player_color);
        }
      }
      if (cnt > 2 && util::is_win(next_grid, player_color)) {
        return ChainReaction(m_width, m_height, m_players, next_grid,
                             next_turn(), cnt, player_color);
      }
    }
    que = nextQue;
  }
  return ChainReaction(m_width, m_height, m_players, next_grid, next_turn(),
                       cnt);
}

std::optional<std::string_view> ChainReaction::get_winner() const {
  for (auto player : m_players) {
    if (is_win(player))
      return player;
  }
  return std::nullopt;
}

bool ChainReaction::is_win(std::string_view color) const {
  return m_winner.has_value() && m_winner.value() == color;
}

std::vector<Move> ChainReaction::legalMoves() const {
  return legalMoves(get_player());
}
std::vector<Move> ChainReaction::legalMoves(std::string_view color) const {
  if (get_winner().has_value())
    return {};
  std::vector<Move> moves;
  for (int i = 0; i < m_grid.size(); i++) {
    const State &sq = m_grid[i];
    if (sq.color == "" || sq.color == color)
      moves.push_back({i % m_width, i / m_width, color});
  }
  return moves;
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
  ss << "(" << m << state.circles << ")";
  stream << ss.str();
  return stream;
}

std::ostream &operator<<(std::ostream &stream, const Move &move) {
  stream << "(" << move.x << " " << move.y << ")";
  return stream;
}

bool operator==(const Move &a, const Move &b) {
  return a.x == b.x && a.y == b.y && a.color == b.color;
}
