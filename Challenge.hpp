#pragma once

#include "Position.hpp"

#include "GameGraphSolver/GameGraphSolver.h"

#include <array>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>
#include <iostream>

class Challenge {
public:
	struct Status {
		using PlayerState = std::array<int, 2>;

		enum class Turn {
			PLAYER,
			AI
		};

		PlayerState	player;
		PlayerState	ai;
		Turn		turn;

		Status() = default;
		Status(const PlayerState& player, const PlayerState& ai, Turn t)
			: player(player), ai(ai), turn(t) {}

		friend std::ostream& operator<<(std::ostream& os, const Turn& turn) {
			switch (turn) {
				case Turn::PLAYER:
					os << "PLAYER";
					break;
				case Turn::AI:
					os << "AI";
					break;
				default:
					break;
			}
			return os;
		}

		friend std::ostream& operator<<(std::ostream& os, const Status& status) {
			os << '{'
			   << "Player{" << status.player[0] << ", " << status.player[1] << "}, "
			   << "AI{" << status.ai[0] << ", " << status.ai[1] << "}, "
			   << status.turn
			   << '}';
			return os;
		}
	};

	using Move = std::array<bool, 2>;
public:
	Challenge() : Challenge(true) {}

	Challenge(bool is_player_first) : game_solver(), current_status() {
		game_solver.build_graph(Position::get_starting_positions());
		game_solver.color_graph();
		
		reset(is_player_first);
	}

	void reset(bool is_player_first = true) {
		current_status = Status{{1, 1}, {1, 1}, is_player_first ? Status::Turn::PLAYER : Status::Turn::AI};
	}

	void set_status(const Status& status) {
		this->current_status = status;
	}

	const Status& get_status() const {
		return current_status;
	}

	bool make_player_move(const Move& move) {
		if (current_status.turn != Status::Turn::PLAYER) {
			throw std::logic_error("It's not the player's turn.");
		}

		bool suc = try_make_move(move, current_status);

		return suc;
	}

	bool make_ai_move(Move& move) {
		if (current_status.turn != Status::Turn::AI) {
			throw std::logic_error("It's not the AI's turn.");
		}

		auto next_positions = get_sorted_next_positions();
		if (next_positions.empty()) {
			return false;
		}
		else {
			move = next_positions.front().second;
			try_make_move(move, current_status);
			return true;
		}
	}

	std::optional<Status::Turn> check_winner() const {
		if (current_status.player == std::array<int, 2>{0, 0}) {
			return Status::Turn::PLAYER;
		}
		else if (current_status.ai == std::array<int, 2>{0, 0}) {
			return Status::Turn::AI;
		}
		else {
			return std::nullopt;
		}
	}

	bool is_deadlock() const {
		return game_solver.is_draw(get_current_position());
	}

	bool is_terminal() const {
		return check_winner().has_value() || is_deadlock();
	}

	bool is_terminal(std::optional<Status::Turn>& winner, bool& is_deadlock) const {
		auto w = check_winner();
		if (w.has_value()) {
			winner = w;
			is_deadlock = false;
			return true;
		}
		else if (this->is_deadlock()) {
			winner = std::nullopt;
			is_deadlock = true;
			return true;
		}
		else {
			return false;
		}
	}

	Position get_current_position() const {
		return status_to_position(current_status);
	}

	GameGraphSolver::PositionType get_current_position_type() const {
		return game_solver.get_position_type(get_current_position());
	}

	std::vector<std::pair<Position, Move>> get_sorted_next_positions() const {
		std::vector<std::pair<Position, Move>> results;
		results.reserve(4);

		for (bool p : {0, 1}) {
			for (bool o : {0, 1}) {
				Status next_status = current_status;
				bool suc = try_make_move({p, o}, next_status);
				if (suc) {
					results.emplace_back(std::make_pair(status_to_position(next_status), Move{p, o}));
				}
			}
		}

		std::sort(results.begin(), results.end(), [this](const std::pair<Position, Move>& p1, const std::pair<Position, Move>& p2) {
			return worse(&p1.first, &p2.first);
		});

		return results;
	}

	static Position status_to_position(const Status& status) {
		Position::PlayerState player_state{status.player[0], status.player[1]};
		Position::PlayerState ai_state{status.ai[0], status.ai[1]};

		if (status.turn == Status::Turn::PLAYER) {
			return Position{player_state, ai_state};
		}
		else {
			return Position{ai_state, player_state};
		}
	}

	static bool try_make_move(const Move& move, Status& status) {
		Status::PlayerState& p = (status.turn == Status::Turn::PLAYER) ? status.player : status.ai;
		Status::PlayerState& o = (status.turn == Status::Turn::PLAYER) ? status.ai : status.player;

		int& p_val = p[move[0]];
		int  o_val = o[move[1]];
		
		if (p_val == 0 || o_val == 0) {
			return false;
		}
		else {
			p_val = (p_val + o_val) % 10;
			status.turn = shift_turn(status.turn);
			return true;
		}
	}

private:
	bool worse(const Position* pos1, const Position* pos2) const {
		using PT = GameGraphSolver::PositionType;
		
		PT pt1 = game_solver.get_position_type(pos1);
		PT pt2 = game_solver.get_position_type(pos2);

		if (pt1 == pt2 && pt2 == PT::T_POSITION) {
			return !game_solver.is_draw(pos1) && game_solver.is_draw(pos2);
		}
		else {
			return pt1 < pt2;
		}
	}

	static Status::Turn shift_turn(Status::Turn turn) {
		return (turn == Status::Turn::PLAYER) ? Status::Turn::AI : Status::Turn::PLAYER;
	}
private:
	GameGraphSolver		game_solver;
	Status				current_status;
};
