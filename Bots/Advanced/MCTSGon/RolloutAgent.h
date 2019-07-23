#pragma once

#include "GameState.h"

namespace MCTSGon {

	class RolloutAgent
	{
	public:
		RolloutAgent();
		~RolloutAgent();
	
		float score_function(const GameState& board, int me, int enemy);

		void Start();
		void Set_coeffs(std::vector<float> _coeffs);

		std::vector<float>coeffs;

		enum COEFFS {
			MY_HELIOS_DANGER = 0,
			ENEMY_HELIOS_DANGER,
			MY_THREAT,
			ENEMY_THREAT,
			KILL_PIECE,
		};
	};
}
