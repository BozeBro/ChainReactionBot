#pragma once
#include "mcts.hpp"

#include <memory>
template <typename Game> class MonteAgent {
  using Node = MCTS<Game>;
  using M = typename Node::M;

public:
  MonteAgent(Node *agent) : m_agent(agent), m_root(agent) {}
  template <typename... Args>
  MonteAgent(Args&&... args) : m_agent(new Node(std::forward<Args>(args)...)), m_root(m_agent) {}

  void move(M move) {
    m_agent = m_agent->play_move(move);
    assert(m_agent != nullptr);
    if (!m_running) {
      this->run();
    }
  }
  void run() {
    if (!m_running) {
      m_running = true;
      m_agent->run();
      m_running = false;
    }
  }
  typename Node::M get_move() { return m_agent->get_best(); }

public:
  bool m_running = false;
  Node* m_agent;
  std::unique_ptr<Node> m_root;
};