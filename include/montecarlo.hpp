#include <cstdint>
#include <queue>
#include <string_view>
#include <vector>
template <typename Game> class MonteTree {
  friend Game;

  MonteTree() = default;
  std::vector<MonteTree> m_children;
};

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
  std::string_view color;
  int x;
  int y;
};
// Need functions for simulation
// nextState, isWin
// legalMoves
class ChainReaction : MonteTree<ChainReaction> {
public:
  ChainReaction(int width, int height)
      : m_width(width), m_height(height), m_grid(width * height, {"", 0}) {}
  ChainReaction(int width, int height, const std::vector<State> &grid)
      : m_width(width), m_height(height), m_grid(grid) {}
  ChainReaction(ChainReaction &&) = default;
  ChainReaction(const ChainReaction &) = default;
  ChainReaction &operator=(ChainReaction src) {
    this->m_width = src.m_width;
    this->m_height = src.m_height;
    this->m_grid = src.m_grid;
    return *this;
  }
  ChainReaction nextState(Move move);
  friend std::ostream &operator<<(std::ostream &, const ChainReaction &);
  std::vector<State> get_grid() const { return m_grid; }

private:
  int8_t m_width;
  int8_t m_height;
  std::vector<State> m_grid;

private:
  bool handle_sq(std::vector<State> &grid, int ind, std::string_view color);
  void append_if_explode(std::queue<int> &que, std::vector<State> &grid,
                         int ind, std::string_view color);
};
