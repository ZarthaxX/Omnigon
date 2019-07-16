#include "MinMax.h"

namespace MinMaxGon {
	Agent::Agent()
	{
		for (uint8_t i = 0; i < Constants::MAX_DEPTH + 1; i++)
			actions[i] = std::vector<Action>(Constants::MAX_MOVES,Action());
	}

	Agent::~Agent()
	{
	}

	Action Agent::find_best_action(Board & board, int me, int enemy)
	{
		float best = minmax(board, me, Constants::MAX_DEPTH, -1000000.0f, 1000000.0f);
		//std::cerr << best << std::endl;
		//std::cerr << board << std::endl;
		//std::cerr << (int)board.Get_token_amount(0) << " " << (int)board.Get_token_amount(1) << std::endl;
		if (best_action.from.y > 3)
			best_action.from.x -= (best_action.from.y - 3);

		if (best_action.to.y > 3)
			best_action.to.x -= (best_action.to.y - 3);

		return best_action;
	}

	std::tuple<int, int, int, int, int> Agent::external_find_best_action(Board& board, int me, int enemy)
	{
		float best = minmax(board, me, Constants::MAX_DEPTH, -1000000.0f, 1000000.0f);
		
		if (best_action.from.y > 3)
			best_action.from.x -= (best_action.from.y - 3);

		if (best_action.to.y > 3)
			best_action.to.x -= (best_action.to.y - 3);

		return { best_action.from.x,best_action.from.y,best_action.to.x,best_action.to.y,best_action.final_orientation };
	}

	float Agent::score_function(const Board & board,Player player_turn,uint8_t depth)
	{
		if (board.get_winner() == player_turn)
			return 1000.0f * (depth+1);
		if (board.get_winner() == !player_turn)
			return -1000.0f * (depth+1);

		return (board.Get_token_amount(player_turn) - board.Get_token_amount(!player_turn));
	}

	float Agent::minmax(Board & board, Player player_turn, uint8_t depth, float alpha, float beta)
	{
		if (depth == 0 || !board.Is_in_progress()) {
			return score_function(board,player_turn,depth);
		}

		uint8_t move_count = board.Get_moves(actions[depth]);

		for (uint8_t i = 0; i < move_count; i++) {
			board.Do_action(actions[depth][i]);

			float actual_score = -minmax(board, !player_turn, depth - 1, -beta, -alpha);

			if (depth == Constants::MAX_DEPTH) {
				int a = 23;
			}

			if (actual_score > alpha) {
				alpha = actual_score;
				if (actual_score > beta) {
					board.Undo_action();
					return beta;
				}

				if (depth == Constants::MAX_DEPTH)
					best_action = actions[depth][i];
			}

			board.Undo_action();
		}

		return alpha;
	}

	void Agent::set_coeffs(std::vector<float> _coeffs)
	{
		coeffs = _coeffs;
	}

}
