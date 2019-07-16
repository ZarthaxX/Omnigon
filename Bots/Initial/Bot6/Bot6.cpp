#include <iostream>
#include <string>
#include <algorithm>

#include "..\StarterKit\Board.h"

int number_of_threats(const Board& b, int owner) {

	auto temp_actions = b.get_moves(owner);

	int score = 0;

	for (Action a : temp_actions) {
		for (Piece p : b.get_type_piece_data(1 - owner)) {
			if (a.to == p.location) {
				score++;
			}
		}
	}

	return score;
}

bool helios_dies(const Board& b, int owner) {
	auto temp_actions = b.get_moves(1 - owner);

	for (Action a : temp_actions) {
		for (Piece p : b.get_type_piece_data(owner)) {
			if (a.to == p.location && p.type == "HELIOS")
				return true;
		}
	}
	return false;
}

bool helios_died(const Board& b, int owner) {
	for (Piece p : b.get_type_piece_data(owner)) {
		if (p.type == "HELIOS")
			return false;
	}
	return true;
}

int main()
{
	int my_id;//A number indicating which tokens correspond to you, 0 for gold pieces and 1 for white pieces.
	int number_of_games;//A number indicating how many games will be played on this match.

	std::cin >> my_id >> number_of_games;

	int enemy = 1 - my_id;

	while (number_of_games > 0) {

		int last_winner;//A number indicating who won last game,the value will be:
						//-1 for first turn
						//0 if gold player won
						//1 if white player won
						//2 if it was a draw

		std::cin >> last_winner;

		while (true) {

			int token_count;//A number indicating how many pieces are on the board in this turn.
			std::cin >> token_count;

			if (token_count == -1)
				break;

			Board board;

			for (int i = 0; i < token_count; i++) {
				// Coordinates system for Omnigon board
				//		   (0,0) (1,0) (2,0) (3,0)
				//		(0,1) (1,1) (2,1) (3,1) (4,1)
				//	 (0,2) (1,2) (2,2) (3,2) (4,2) (5,2)
				//(0,3) (1,3) (2,3) (3,3) (4,3) (5,3) (6,3)
				//	 (0,4) (1,4) (2,4) (3,4) (4,4) (5,4)
				//		(0,5) (1,5) (2,5) (3,5) (4,5)
				//		   (0,6) (1,6) (2,6) (3,6)
				int player;//A number indicting who is the owner of this piece,it can either be 0 or 1.
				std::string type;//A string indicating what type of pieces it is,the values can be "HELIOS","BIGON","TRIGON" and "STARGON".
				int x;//A number indicating the x coordinate of the piece, refer to the board image above to understand coordinates better.
				int y;//A number indicating the y coordinate of the piece, refer to the board image above to understand coordinates better.
				int orientation;//A number indicating the orientation of the piece, refer to the pieces images to understand how many orientations each piece has.
				std::cin >> player >> type >> x >> y >> orientation;

				board.add_token(player, type, x, y, orientation);
			}

			auto actions = board.get_moves(my_id);

			std::random_shuffle(actions.begin(), actions.end());

			Action best_action = board.get_moves(my_id)[0];

			int best_threat = INT_MAX;
			int best_enemy_amount = 14;

			for (Action action : actions) {
				Board temp_board = board;
				temp_board.move_token(action);

				int temp_enemy_amount = temp_board.get_type_piece_data(enemy).size();

				if (helios_died(temp_board, enemy) || !helios_dies(temp_board, my_id)) {

					if (temp_enemy_amount < best_enemy_amount || helios_died(temp_board, enemy)) {
						best_action = action;
						best_threat = number_of_threats(temp_board, enemy);
						best_enemy_amount = temp_enemy_amount - helios_died(temp_board, enemy) * 100;
					}
					else if (temp_enemy_amount == best_enemy_amount) {
						int temp_threat = number_of_threats(temp_board, enemy) + helios_dies(temp_board, my_id)*100;

						if (temp_threat < best_threat) {
							best_threat = temp_threat;
							best_action = action;
						}
					}
				}
			}

			std::cout << best_action << std::endl;
		}

		number_of_games -= 1;
	}
}