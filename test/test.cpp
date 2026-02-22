#include <iostream>
#include <stdexcept>
#include <optional>
#include <string>
#include <chrono>
#include <filesystem>

#include "Challenge.hpp"

using namespace std;

void game_loop(Challenge& challenge);

int main() {
	Challenge challenge;

	char opt = 'y';
	while (opt == 'y') {
		game_loop(challenge);
		
		cout << "Another game? [y/n]: " << std::flush;
		cin >> opt;
	}

	return 0;
}

void game_loop(Challenge& challenge) {
	cout << "Decide who goes first (0 for player, 1 for AI): ";
	bool is_ai_first;
	cin >> is_ai_first;
	challenge.reset(!is_ai_first);

	optional<Challenge::Status::Turn> winner;
	bool is_deadlock;
	while (!challenge.is_terminal(winner, is_deadlock)) {
		Challenge::Status status = challenge.get_status();
		cout << "AI\t" << "[0]: " << status.ai[0] << "\t[1]: " << status.ai[1]
		     << "\t" << challenge.get_current_position_type() << endl;

		cout << "Player\t" << "[0]: " << status.player[0] << "\t[1]: " << status.player[1] << endl;

		Challenge::Move move;
		if (status.turn == Challenge::Status::Turn::PLAYER) {
			cout << "Your turn: ";
			cin >> move[0] >> move[1];
			bool suc = challenge.make_player_move(move);
			if (!suc) {
				cout << "Invalid move. Try again." << endl;
			}
		}
		else {
			challenge.make_ai_move(move);
			cout << "AI Move: (" << move[0] << ", " << move[1] << ")" << endl;
		}
		cout << "-----------------------------" << endl;
	}

	if (winner.has_value()) {
		if (winner.value() == Challenge::Status::Turn::PLAYER) {
			cout << "Congratulations, you win!" << endl;
		}
		else {
			cout << "AI wins! Better luck next time." << endl;
		}
	}
	else {
		cout << "AI thinks we are in a deadlock." << endl;
	}
}
