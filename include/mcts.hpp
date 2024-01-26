#pragma once

#include <algorithm>
#include <assert.h>
#include <cmath>
#include <cstdint>
#include <functional>
#include <iostream>
#include <optional>
#include <queue>
#include <random>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

static constexpr double C = 2;

/* Need parent to have
 * legalMoves -> vector
 * get_winner
 * get_player
 * nextState
 */
template <typename Game> class MCTS : public Game {

public:
  using Node = MCTS<Game>;
  using M = typename std::decay_t<
      decltype(*std::declval<Game>().legalMoves().begin())>;

  static_assert(
      std::is_same_v<const Game, decltype(std::declval<Game>().nextState(
                                     std::declval<M>()))>,
      "Next state must return a state with the same state as the caller");

  template <typename... Args>
  MCTS(MCTS *parent, Args &&...args)
      : Game(std::forward<Args>(args)...), m_parent(parent),
        m_untried_moves(static_cast<Game *>(this)->legalMoves()) {}

  ~MCTS() {
    // TODO: Consider making children a shared_ptr so we aren't accidently
    // deleting children
    for (auto child : m_children)
      delete child;
  }

public:
  void run() {
    for (int i = 0; i < 1000 && !m_final_val.has_value(); i++) {
      auto leaf = selection();
      auto winner = leaf->simulate();
      leaf->backpropagate(winner);
    }
    return;
  }

private:
  MCTS *selection() {
    MCTS *cur = this;
    while (!cur->is_final_state() && !cur->is_leaf()) {
      cur->incr_simul();
      if (!cur->m_untried_moves.empty()) {

        cur = cur->expand();
        break;
      }

      MCTS *nxt_state = nullptr;
      std::optional<double> score;
      Game *const game = static_cast<Game *>(cur);
      auto cmp = std::less<double>{};

      assert(!cur->m_children.empty());

      // we've tried all states but children are not final.
      for (auto child : cur->m_children) {
        assert(child);
        // final_val state should be backpropagate if child confirms loss
        assert(!child->m_final_val.has_value() ||
               child->m_final_val.value() != cur->get_player());
        if (!score.has_value() || cmp(child->get_score(), score.value())) {
          nxt_state = child;
          score = nxt_state->get_score();
        }
      }
      assert(nxt_state);
      cur = nxt_state;
    }
    assert(cur);
    cur->incr_simul();
    return cur;
  }
  MCTS *expand(int ind) {
    M move = m_untried_moves[ind];
    Game const *const game = static_cast<Game *>(this);
    MCTS *child = new MCTS(this, game->nextState(move));
    assert(child->get_winner() != child->get_player());
    if (child->get_winner().has_value()) {
      child->set_final_val(child->get_winner().value());
    }

    m_children.push_back(child);
    m_moves.push_back(move);
    m_untried_moves.erase(m_untried_moves.begin() + ind,
                          m_untried_moves.begin() + ind + 1);
    assert(child);
    return child;
  }
  MCTS *expand() { return expand(m_untried_moves.size() - 1); }
  std::string_view simulate() {
    if (this->is_final_state()) {
      return m_final_val.value();
    }
    std::random_device rand_dev;
    std::mt19937 generator(rand_dev());
    Game cur = static_cast<Game>(*this);
    assert(!cur.get_winner().has_value() && !this->is_final_state());
    while (!cur.get_winner().has_value()) {
      auto moves = cur.legalMoves();
      std::uniform_int_distribution<std::size_t> distribution(0,
                                                              moves.size() - 1);
      M move = moves[distribution(generator)];
      cur = cur.nextState(move);
    }
    return cur.get_winner().value();
  }
  void backpropagate(std::string_view winner) {
    for (MCTS *cur = this; cur != nullptr; cur = cur->m_parent) {
      if (winner != "") {
        bool win_state = winner == cur->get_player();
        cur->add_score(win_state ? 1 : -1);
      }
      MCTS *parent = cur->m_parent;
      if (!parent)
        continue;
      if (!cur->is_final_state())
        continue;

      if (cur->m_final_val == parent->get_player()) {
        parent->m_final_val = parent->get_player();
        continue;
      }
      bool tie;
      auto get_child_finals = [&tie](MCTS *child) {
        if (child->is_final_state() && child->get_final_val().value().empty())
          tie = true;
        return child->is_final_state();
      };
      bool all_final = std::all_of(parent->m_children.begin(),
                                   parent->m_children.end(), get_child_finals);
      if (parent->m_untried_moves.empty() && all_final) {
        parent->set_final_val(tie ? "" : cur->get_final_val().value());
      }
    }
  }

public:
  friend void print(Node const *const tree) {
    std::queue<Node const *> que;
    que.push(tree);
    while (!que.empty()) {
      std::queue<Node const *> nxt;
      std::cout << "********\n";
      while (!que.empty()) {
        Node const *top = que.front();
        que.pop();
        std::cout << (*top) << " ";
        for (Node *child : top->m_children) {
          nxt.push(child);
        }
      }
      std::cout << "********\n";
      que = nxt;
    }
  }

  friend std::ostream &operator<<(std::ostream &stream,
                                  const MCTS<Game> &tree) {

    Game game = static_cast<Game>(tree);
    stream << tree.get_score() << " " << tree.m_score << " "
           << (tree.m_final_val.has_value() ? tree.m_final_val.value() : "NONE")
           << " " << game.get_player() << "\n"
           << game << '\n';
    return stream;
  }

public:
  MCTS *play_move(M move) {
    for (int i = 0; i < m_moves.size(); i++) {
      if (m_moves[i] == move) {
        return m_children[i];
      }
    }
    for (int i = 0; i < m_untried_moves.size(); i++) {
      if (m_untried_moves[i] == move) {
        return expand(i);
      }
    }
    return nullptr;
  }
  M get_best() {
    if (m_children.size() == 0) {
      assert(!m_untried_moves.empty());
      M move = m_untried_moves[0];
      this->expand();
      return move;
    }
    M move = m_moves[0];
    int ind = 0;
    double score = m_children[0]->get_score();
    if (m_children[0]->is_final_state() &&
        m_children[0]->get_winner() == this->get_player()) {
      return m_moves[0];
    }
    for (int i = 1; i < m_children.size(); i++) {
      if (m_children[i]->is_final_state() &&
          m_children[i]->get_winner() == this->get_player()) {
        return m_moves[i];
      }
      if (m_children[i]->get_score() < score) {
        ind = i;
        move = m_moves[i];
        score = m_children[i]->get_score();
      }
    }
    return move;
  }

private:
  double get_score() const {
    double wins = static_cast<double>(m_score);
    double simuls = static_cast<double>(m_simuls);
    if (!m_parent)
      return wins / simuls;
    return wins / simuls - C * sqrt(log(m_parent->m_simuls) / simuls);
  }
  bool is_leaf() { return static_cast<Game *>(this)->get_winner().has_value(); }
  bool is_final_state() const { return m_final_val.has_value(); }
  void set_final_val(std::string_view val) { m_final_val = val; }
  std::optional<std::string_view> get_final_val() const { return m_final_val; }
  void add_score(int score) { m_score += score; }
  void incr_simul() { m_simuls++; }

private:
  MCTS *m_parent;

  std::vector<MCTS *> m_children;
  std::vector<M> m_moves;
  std::vector<M> m_untried_moves;
  int m_score = 0;
  uint32_t m_simuls = 0;
  std::optional<std::string_view> m_final_val;
};

template <typename Game> class MonteAgent {
  using Node = MCTS<Game>;
  using M = typename Node::M;

public:
  MonteAgent(Node *agent) : m_agent(agent) {}

  void move(M move) {
    m_agent = m_agent->play_move(move);
    m_agent->run();
  }
  typename Node::M get_move() { return m_agent->get_best(); }

private:
  Node *m_agent;
};
