#pragma once
#include <cstdint>
#include <iostream>
#include <optional>
#include <queue>
#include <string_view>
#include <vector>

struct State {
  std::string_view color;
  int circles;
  friend std::ostream &operator<<(std::ostream &, const State &);
  void reset() {
    color = "";
    circles = 0;
  }
};

struct Move {
  int x;
  int y;
  std::string_view color;
  friend std::ostream &operator<<(std::ostream &, const Move &);
};

class ChainReaction {
public:
  friend std::ostream &operator<<(std::ostream &, const ChainReaction &);

  ChainReaction(int width, int height, std::vector<std::string_view> players,
                int turn = 0, uint32_t cnt = 0,
                std::optional<std::string_view> winner = std::nullopt)
      : m_width(width), m_height(height), m_grid(width * height, {"", 0}),
        m_players(players), m_turn(turn), m_cnt(cnt), m_winner(winner) {}

  ChainReaction(int width, int height, std::vector<std::string_view> players,
                std::vector<State> grid, int turn = 0, uint32_t cnt = 0,
                std::optional<std::string_view> winner = std::nullopt)
      : m_width(width), m_height(height), m_grid(grid), m_players(players),
        m_turn(turn), m_cnt(cnt), m_winner(winner) {}

  ~ChainReaction() = default;
  ChainReaction(const ChainReaction &) = default;

public:
  inline int next_turn() const { return (m_turn + 1) % m_players.size(); }
  ChainReaction nextState(Move move) const;
  bool is_win(std::string_view color) const;
  std::string_view get_winner() const;
  std::vector<Move> legalMoves(std::string_view color) const;
  std::string_view get_player() const { return m_players[m_turn]; }

private:
  int8_t m_width;
  int8_t m_height;
  std::vector<State> m_grid;
  std::vector<std::string_view> m_players;
  int m_turn;
  uint32_t m_cnt;
  std::optional<std::string_view> m_winner;

private:
  bool handle_sq(std::vector<State> &grid, int ind,
                 std::string_view color) const;
  void append_if_explode(std::queue<int> &que, std::vector<State> &grid,
                         int ind, std::string_view color) const;
};
