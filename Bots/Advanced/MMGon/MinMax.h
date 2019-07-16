#pragma once

#include "OptimizedBoard.h"

namespace MinMaxGon {
	class Agent
	{
		public:
		Agent();
		~Agent();

		Action find_best_action(Board& board, int me, int enemy);

		std::tuple<int,int,int,int,int> external_find_best_action(Board& board, int me, int enemy);

		void set_coeffs(std::vector<float> _coeffs);

	private:

		float score_function(const Board& board,Player player_turn,uint8_t depth);
		float minmax(Board& board, Player player_turn,uint8_t depth, float alpha, float beta);
		
		std::vector<float>coeffs;

		enum COEFFS {
			KILL_HELIOS = 0,
			MY_HELIOS_DANGER,
			ENEMY_HELIOS_DANGER,
			MY_THREAT,
			ENEMY_THREAT,
			KILL_PIECE,
		};

		Action best_action;
		std::vector<Action>actions[Constants::MAX_DEPTH + 1];

	};

}
