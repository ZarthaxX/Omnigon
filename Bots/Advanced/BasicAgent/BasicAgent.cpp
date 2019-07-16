#include "BasicAgent.h"

namespace BasicAgent {

	Agent::Agent()
	{

	}


	Agent::~Agent()
	{
	}

	int Agent::number_of_threats(const Board& b, int owner) {

		auto temp_actions = b.get_moves(owner);

		int score = 0;

		auto pieces = b.get_type_piece_data(1 - owner);

		for (Action a : temp_actions) {
			for (Piece& p : pieces) {
				if (a.to == p.location) {
					score++;
				}
			}
		}

		return score;
	}

	bool Agent::helios_dies(const Board& b, int owner) {
		auto temp_actions = b.get_moves(1 - owner);

		auto pieces = b.get_type_piece_data(owner);

		for (Action a : temp_actions) {
			for (Piece& p : pieces) {
				if (a.to == p.location && p.type == HELIOS)
					return true;
			}
		}
		return false;
	}

	bool Agent::helios_died(const Board& b, int owner) {
		for (Piece p : b.get_type_piece_data(owner)) {
			if (p.type == HELIOS)
				return false;
		}
		return true;
	}

	float Agent::score_function(const Board& board, int me, int enemy) {

		float temp_score = 0.0;

		temp_score += helios_died(board, enemy) * coeffs[KILL_HELIOS];
		temp_score += helios_dies(board, me) * coeffs[MY_HELIOS_DANGER];
		temp_score += helios_dies(board, enemy) * coeffs[ENEMY_HELIOS_DANGER];
		temp_score += (((float)number_of_threats(board, me)) / 49.0f) * coeffs[MY_THREAT];
		temp_score += (((float)number_of_threats(board, enemy)) / 49.0f) * coeffs[ENEMY_THREAT];
		temp_score += (1 - ((float)board.get_type_piece_count(enemy)) / 7.0f) * coeffs[KILL_PIECE];

		return temp_score;
	}

	Action Agent::find_best_action(Board& _board, int me, int enemy) {

		Board board = _board;

		auto actions = board.get_moves(me);

		if (actions.size() == 0) {
			std::cerr << _board << std::endl;
		}

		std::random_shuffle(actions.begin(), actions.end());


		Action best_action = actions[0];

		float best_score = -100000;

		for (Action action : actions) {
			//std::cerr << "----------------BOARD STATE---------------" << std::endl;
			//std::cerr << board << std::endl;
			board.do_action(action);
			//std::cerr << "----------------BOARD STATE---------------" << std::endl;
			//std::cerr << board << std::endl;
			float score = score_function(board, me, enemy);

			if (best_score < score) {
				best_score = score;
				best_action = action;
			}

			board.undo_action();
		}

		return best_action;
	}

	void Agent::external_start()
	{

	}

	void Agent::set_coeffs(std::vector<float> _coeffs)
	{
		coeffs = _coeffs;
	}

	std::tuple<int, int, int, int, int> Agent::external_find_best_action(Board& board, int me, int enemy)
	{
		Action best_action = find_best_action(board, me, enemy);
		return { best_action.from.x,best_action.from.y,best_action.to.x,best_action.to.y,best_action.orientation};
	}

}
