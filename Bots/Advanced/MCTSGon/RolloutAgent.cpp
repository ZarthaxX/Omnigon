#include "RolloutAgent.h"

namespace MCTSGon {

	RolloutAgent::RolloutAgent()
	{
	}


	RolloutAgent::~RolloutAgent()
	{
	}

	float RolloutAgent::score_function(const GameState& board, int me, int enemy) {

		float temp_score = 0.0;

		std::pair<uint8_t, bool> my_threats = board.Get_threats(me);
		std::pair<uint8_t, bool> enemy_threats = board.Get_threats(enemy);

		temp_score += enemy_threats.second * coeffs[MY_HELIOS_DANGER];
		temp_score += my_threats.second * coeffs[ENEMY_HELIOS_DANGER];
		temp_score += (((float)my_threats.first) / 49.0f) * coeffs[MY_THREAT];
		temp_score += (((float)enemy_threats.first) / 49.0f) * coeffs[ENEMY_THREAT];
		temp_score += (1 - ((float)board.Get_token_amount(enemy)) / 7.0f) * coeffs[KILL_PIECE];

		return temp_score;
	}

	void RolloutAgent::Start()
	{

	}

	void RolloutAgent::Set_coeffs(std::vector<float> _coeffs)
	{
		coeffs = _coeffs;
	}

}