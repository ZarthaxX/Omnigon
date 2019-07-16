#pragma once
#include <iostream>
#include <vector>
#include <random>

#include "Board.h"

namespace BasicAgent {
	class Agent
	{
		public:
		Agent();
		~Agent();

		int number_of_threats(const Board & b, int owner);

		bool helios_dies(const Board & b, int owner);

		bool helios_died(const Board & b, int owner);

		float score_function(const Board& board, int me, int enemy);

		Action find_best_action(Board & board, int me, int enemy);

		std::tuple<int, int, int, int, int> external_find_best_action(Board& board, int me, int enemy);
		void external_start();
		void set_coeffs(std::vector<float> _coeffs);

		std::vector<float>coeffs;

		enum COEFFS {
			KILL_HELIOS = 0,
			MY_HELIOS_DANGER,
			ENEMY_HELIOS_DANGER,
			MY_THREAT,
			ENEMY_THREAT,
			KILL_PIECE,
		};
	};
}


