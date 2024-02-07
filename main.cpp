/*
 * Copyright (c) 2016, Peter Thorson. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the WebSocket++ Project nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL PETER THORSON BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include "chainreaction.hpp"
#include "client.hpp"
#include "agent.hpp"

#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <string_view>

static constexpr char TYPE[] = "type";
static constexpr char X[] = "x";
static constexpr char Y[] = "y";

// For now: Red == US, Blue == Enemy
class ChainGameClient {
  using AgentTree = MCTS<ChainReaction>;
  using AgentBot = MonteAgent<ChainReaction>;

public:
  ChainGameClient(int width, int height, std::vector<std::string_view> players,
                  const char *host, const char *id) 
        : m_gamebot(std::make_unique<AgentBot>(nullptr, width, height, players))
        , m_bot(std::make_unique<BotClient>(
        host, id, [this](json srv_data) { return this->on_message(srv_data); })) 
        {}
  void start() { m_bot->connect(); }

private:
  Payload on_message(json srv_data) {
    const std::string data_type = srv_data[TYPE];
    if (data_type == "color") {
      m_color = srv_data["color"];
    } else if (data_type == "update") {
      // TODO: handle when player leaves, and when handling the bot case
    } else if (data_type == "start") {
      m_turn = srv_data["turn"];
      m_username = srv_data["username"];
      if (m_turn == m_color) {
        m_game_color = "Red";
        m_enemy_color = "Blue";
        // get_move
        m_gamebot->run();
        json res;
        auto move = m_gamebot->get_move();
        res["x"] = move.x;
        res["y"] = move.y;
        res["type"] = "move";
        return {res, true};
      } else {
        m_game_color = "Blue";
        m_enemy_color = "Red";
      }
    } else if (data_type == "winner") {
      std::cout << "THERE IS A WINNER\n";
    } else if (data_type == "move") {
      const int x = srv_data[X];
      const int y = srv_data[Y];
      auto player = srv_data["color"] == m_color ? m_game_color : m_enemy_color;
      m_gamebot->move({x, y, player});
      m_turn = srv_data["turn"];
      if (m_turn == m_color) {
        m_gamebot->run();
        json res;
        auto move = m_gamebot->get_move();
        res[X] = move.x;
        res[Y] = move.y;
        res["type"] = "move";
        return {res, true};
      }
    }

    return {srv_data, false};
  }
  std::unique_ptr<AgentBot> m_gamebot;
  std::unique_ptr<BotClient> m_bot;
  std::string m_color;
  std::string m_turn;
  std::string m_username;
  std::string m_game_color;
  std::string m_enemy_color;
};

int main(int argc, char *argv[]) {

  // Create a client endpoint
  if (argc != 3) {
    std::cout << "Need 3 arguments, ./main host id";
    return 1;
  }

  ChainGameClient bot(6, 9, {"Red", "Blue"}, argv[1], argv[2]);
  bot.start();
  // BotClient bot(argv[1], argv[2], [](json a) -> Payload { return {a,
  // false}; }); std::string err = bot.connect();
  // std::cout << err;
}
