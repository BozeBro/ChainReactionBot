#pragma once
#include <algorithm>
#include <cassert>
#include <cmath>
#include <ctime>
#include <iostream>
#include <memory>
#include <optional>
#include <random>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

static constexpr double C = 2;
static constexpr double D = 1;
static int n = 0;

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
  // obtaining the move type from Game so we don't have to
  using M =
      typename std::decay_t<decltype(*std::declval<Game>()
                                          .legalMoves(
                                              std::declval<Game>().get_player())
                                          .begin())>;

private:
  MonteTree(const Game &state) : m_state(state) {}
  MonteTree(const Game &state, std::weak_ptr<Node> parent)
      : m_state(state), m_parent(parent) {}

public:
  template <typename... Args>
  static std::shared_ptr<Node> create(Args &&...args) {
    return std::make_shared<Node>(Node(std::forward<Args>(args)...));
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
      if (moves.empty())
        return cur.get_winner();

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
      shared_cur->bt++;
      if (shared_cur->m_state.get_player() == winner) {
        shared_cur->m_wins++;
      }
      shared_cur->set_score(shared_cur->generate_score());
      shared_cur->set_select_score(shared_cur->generate_select_score());
    }
  }
  std::weak_ptr<Node> select() {
    auto cur = this->shared_from_this();
    while (!cur->is_leaf()) {
      cur->m_num_simuls++;
      if (cur->m_children[0]->is_leaf() &&
          cur->m_children[0]->m_num_simuls == 0) {
        cur = cur->m_children[0];
        cur->m_num_simuls++;
        return cur;
      }

      double score = cur->m_children[0]->get_select_score().value();
      int ind = 0;
      for (int i = 1; i < cur->m_children.size(); i++) {
        if (cur->m_children[i]->is_leaf() &&
            cur->m_children[i]->m_num_simuls == 0) {
          cur = cur->m_children[i];
          cur->m_num_simuls++;
          return cur;
        }
        double cur_score = cur->m_children[i]->get_select_score().value();
        if (cur_score > score) {
          score = cur_score;
          ind = i;
        }
      }
      cur = cur->m_children[ind];
    }
    cur->m_num_simuls++;
    return cur;
  }
  void expand() {
    std::string_view me = m_state.get_player();
    std::vector<M> moves = m_state.legalMoves(me);
    m_children.reserve(moves.size());
    for (M move : moves) {
      const Game &game = m_state.nextState(move);
      auto child = Node::create(game, this->weak_from_this());
      assert(child->is_leaf() && !child->m_select_score.has_value());
      m_children.push_back(child);
    }
  }

  void run() {
    for (int i = 0; i < 100000; i++) {
      std::weak_ptr<Node> weak_leaf = select();
      assert(!weak_leaf.expired());
      std::shared_ptr<Node> leaf = weak_leaf.lock();
      assert(leaf->is_leaf());
      assert(leaf->m_num_simuls > 0);
      // in terminal state
      if (!leaf->m_state.get_winner().empty()) {
        leaf->backpropogate(leaf->m_state.get_winner());
        continue;
      }
      if (leaf->m_num_simuls == 1) {
        leaf->backpropogate(leaf->simulate());
        continue;
      }
      assert(leaf->m_select_score.has_value());
      assert(leaf->m_state.get_winner().empty());
      assert(leaf->m_children.size() == 0);
      leaf->expand();
      if (leaf->m_children.size() > 0) {
        auto child = leaf->m_children[0];
        assert(child != nullptr);
        auto winner = child->simulate();
        child->m_num_simuls++;
        child->backpropogate(winner);
      } else {
        assert(false);
      }
    }
    return;
  }
  std::optional<double> get_score() const { return m_score; }
  std::optional<double> get_select_score() const { return m_select_score; }
  Game get_state() const { return m_state; }
  void set_score(double score) { m_score = score; }
  void set_select_score(double score) { m_select_score = score; }

private:
  double generate_score() const {
    double wins = static_cast<double>(m_wins);
    if (m_children.size() == 0) {
      assert(m_wins == 1 || m_wins == 0);
      return m_wins;
    }

    std::optional<double> ratio = std::nullopt;
    bool is_init = is_uninitialized(m_parent);
    for (int i = 0; i < m_children.size(); i++) {
      auto child = m_children[i];

      if (!child->get_score().has_value()) {
        return 1 - ratio.value();
      }
      if (!ratio.has_value())
        ratio = child->get_score().value();
      else
        ratio = std::min(ratio.value(), child->get_score().value());
    }
    return 1 - ratio.value();
    // if (is_uninitialized(m_parent)) {
    //   return wins / num_visit;
    // }
    // return wins / num_visit +
    //        C * sqrt(log(m_parent.lock()->m_num_simuls) / num_visit);
  }
  double generate_select_score() const {
    if (is_uninitialized(m_parent)) {
      assert(m_score.has_value());
      return D * m_score.value();
    }
    double wins = static_cast<double>(m_wins);
    double num_visit = static_cast<double>(m_num_simuls);
    assert(num_visit > 0);
    assert(m_score.has_value());
    double simul = log(m_parent.lock()->m_num_simuls);
    return D * m_score.value() + C * sqrt(simul / num_visit);
  }

  bool is_leaf() { return m_children.empty(); }

  int m_wins = 0;
  int m_num_simuls = 0;
  const Game m_state;
  std::weak_ptr<MonteTree> m_parent;
  std::optional<double> m_select_score = std::nullopt;
  std::optional<double> m_score = std::nullopt;
  std::vector<std::shared_ptr<MonteTree>> m_children;
  int bt = 0;
};
