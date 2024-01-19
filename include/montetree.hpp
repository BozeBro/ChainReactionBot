#pragma once
#include <cassert>
#include <cmath>
#include <ctime>
#include <memory>
#include <optional>
#include <random>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

static constexpr double C = 1.4142135;

// Rationale
// https://stackoverflow.com/questions/45507041/how-to-check-if-weak-ptr-is-empty-non-assigned
template <typename T> bool is_uninitialized(std::weak_ptr<T> const &weak) {
  using wt = std::weak_ptr<T>;
  return !weak.owner_before(wt{}) && !wt{}.owner_before(weak);
}

template <typename Game>
class MonteTree : public std::enable_shared_from_this<MonteTree<Game>> {
  friend Game;
  using Node = MonteTree<Game>;
  using M =
      typename std::decay_t<decltype(*std::declval<Game>()
                                          .legalMoves(
                                              std::declval<Game>().get_player())
                                          .begin())>;

public:
  template <typename... Args>
  static std::shared_ptr<Node> create(Args &&...args) {
    return std::make_shared<Node>(std::forward<Args>(args)...);
  }

  std::string_view simulate() {
    srand(time(NULL));
    std::random_device rand_dev;
    std::mt19937 generator(rand_dev());
    std::optional<std::string_view> won = std::nullopt;
    Game cur = m_state;
    std::string_view me = cur.get_player();
    while (!won.has_value()) {
      std::string_view player = cur.get_player();
      std::vector<M> moves = cur.legalMoves(player);

      std::uniform_int_distribution<std::size_t> distribution(0,
                                                              moves.size() - 1);
      M move = moves[distribution(generator)];
      cur = cur.nextState(move);
      if (cur.is_win(player)) {
        won = player;
      }
    }
    return won.value();
  }
  void backpropogate(std::string_view winner) {
    for (std::weak_ptr<MonteTree> cur = this->weak_from_this();
         !is_uninitialized(cur) && !cur.expired(); cur = cur.lock()->m_parent) {
      auto shared_cur = cur.lock();
      if (shared_cur->m_player == winner) {
        shared_cur->m_wins++;
      }
      shared_cur->m_score = shared_cur->generate_score();
    }
  }
  std::weak_ptr<Node> select() {
    auto cur = this->shared_from_this();
    while (!cur->is_leaf()) {
      if (!cur->m_children[0]->is_leaf())
        return std::weak_ptr<Node>(cur->m_children[0]);

      double score = cur->m_children[0]->m_score.value();
      int ind = 0;
      for (int i = 1; i < cur->m_children.size(); i++) {
        if (!cur->m_children[i]->is_leaf())
          return std::weak_ptr<Node>(cur->m_children[i]);
        double cur_score = cur->m_children[i]->m_score.value();
        if (cur_score > score) {
          score = cur_score;
          ind = i;
        }
      }
      cur = cur->m_children[ind];
    }
    return cur;
  }
  void expand() {
    std::string_view me = m_state.get_player();
    std::vector<M> moves = m_state.legalMoves(me);
    m_children.reserve(moves.size());
    for (M move : moves) {
      const Game &game = m_state.nextState(move);
      m_children.push_back(std::make_shared<Node>(game, game.get_player(),
                                                  this->weak_from_this()));
    }
  }

  void run() {
    for (int i = 0; i < 2; i++) {
      std::weak_ptr<Node> weak_leaf = select();
      assert(!weak_leaf.expired());
      std::shared_ptr<Node> leaf = weak_leaf.lock();
      if (leaf->m_num_simuls == 0) {
        auto winner = leaf->simulate();
        leaf->backpropogate(winner);
      } else {
        expand();
        auto child = leaf->m_children[0];
        assert(child != nullptr);
        auto winner = child->simulate();
        child->backpropogate(winner);
      }
    }
  }

private:
  double generate_score() {
    double wins = static_cast<double>(m_wins);
    double num_visit = static_cast<double>(m_num_simuls);
    if (is_uninitialized(m_parent)) {
      return wins / num_visit;
    }
    return wins / num_visit +
           C * sqrt(log(m_parent.lock()->m_num_simuls) / num_visit);
  }
  bool is_leaf() { return m_children.empty(); }
  MonteTree(const Game &state, std::string_view player)
      : m_state(state), m_player(player), m_parent() {}
  MonteTree(const Game &state, std::string_view player,
            std::weak_ptr<Node> parent)
      : m_state(state), m_player(player), m_parent(parent) {}

  int m_wins = 0;
  int m_num_simuls = 0;
  const Game m_state;
  std::string_view m_player;
  std::weak_ptr<MonteTree> m_parent;
  std::optional<double> m_score = std::nullopt;
  std::vector<std::shared_ptr<MonteTree>> m_children;
};
