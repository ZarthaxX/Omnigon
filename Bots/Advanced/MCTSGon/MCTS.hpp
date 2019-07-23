#pragma once
#include <bitset>
#include <stdio.h>
#include <queue>
#include <iostream>
#include <algorithm>
#include <vector>
#include <map>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <set>
#include <limits>
#include <chrono>
#include <immintrin.h> //SSE Extensions
#include <ostream>
#include <list>

#include "MCTSGon_constants.h"
#include "Utilities.h"
#include "FixedStack.hpp"
#include "GameState.h"
#include "RolloutAgent.h"

namespace MCTSGon {

	class MCTSAgent {
	public:
		MCTSAgent();
		~MCTSAgent();
		MCTSAgent& operator=(const MCTSAgent& mcts);

		Action find_best_action(const GameState& gamestate,Player me,Player enemy); 
		std::tuple<int, int, int, int, int> external_find_best_action(const GameState & gamestate, Player me, Player enemy);

	private:
		struct Node {
			Node() : visits(0), wins(0), childs(0), status(0), childs_count(0) {}
			inline float UCT(float sqrtLogVisits);
			uint32_t visits;
			uint8_t status;
			float wins;

			uint8_t childs_count;
			std::vector<Node>childs;

			Action action;
			enum {
				IN_PROGRESS = 0,
				FINISHED,
			};
		};

		void selection();
		void expansion();
		void rollout();
		void backpropagation();

		std::vector<Action> actions;
		GameState actual;
		GameState copy;
		float rollout_scores[2];
		RolloutAgent rollout_agent;
		float rollout_agent_score_normalizer;
		int tree_size;
		int max_nodes;
		Node root;
		FixedStack<Node*> path;

		static const int MAX_DEPTH;
		static const float COEFF;
	};

	const float MCTSAgent::COEFF = sqrt(2.0f);
	const int MCTSAgent::MAX_DEPTH = 10000;


	inline float MCTSAgent::Node::UCT(float sqrtLogVisits)
	{
		float invsqrt = 1.0f/sqrtf((float)this->visits);
		return ((wins * invsqrt) + sqrtLogVisits) * invsqrt;
	}

	MCTSAgent::MCTSAgent() : path(MAX_DEPTH) {
		max_nodes = Constants::NODES_POOL_SIZE;
		tree_size = 1;
		actions = std::vector<Action>(255, Action());

		float MY_HELIOS_DANGER = -4.0;
		float ENEMY_HELIOS_DANGER = 0.05;
		float MY_THREAT = 2.5;
		float ENEMY_THREAT = -2.5;
		float KILL_PIECE = 2.5;

		std::vector<float> coeffs = { MY_HELIOS_DANGER,ENEMY_HELIOS_DANGER,MY_THREAT,ENEMY_THREAT,KILL_PIECE };
		rollout_agent_score_normalizer = 0;
		float neg_sum = 0.0f;
		float pos_sum = 0.0f;
		for (float c : coeffs) {
			if (c < 0)neg_sum += -c;
			else pos_sum += c;
		}

		rollout_agent_score_normalizer = std::max(neg_sum, pos_sum);

		rollout_agent.Set_coeffs({ MY_HELIOS_DANGER,ENEMY_HELIOS_DANGER,MY_THREAT,ENEMY_THREAT,KILL_PIECE });

	}

	MCTSAgent::~MCTSAgent() {
	}

	MCTSAgent& MCTSAgent::operator=(const MCTSAgent& mcts) {
		max_nodes = mcts.max_nodes;

		return *this;
	}

	inline Action MCTSAgent::find_best_action(const GameState & gamestate, Player me, Player enemy)
	{
		stopwatch.Start(Constants::TIMEOUT_MICRO_SECONDS);

		actual = gamestate;
		copy = gamestate;

		root.childs.clear();

		tree_size = 1;

		uint32_t iterations = 0;

		while (iterations < 1000000) {
			selection();
			expansion();
			rollout();
			backpropagation();
			iterations++;
		}

		std::cerr << "CREATED " << tree_size << " NODES " << std::endl;

		Node best_child;
		uint32_t best_visits = 0;
		for (Node& n : root.childs) {
			//If the uct of the actual child is better than the best child found so far, we update best child
			if (n.visits > best_visits) {
				best_visits = n.visits;
				best_child = n;
			}
		}

		Action best_action = best_child.action;

		if (best_action.from.y > 3)
			best_action.from.x -= (best_action.from.y - 3);

		if (best_action.to.y > 3)
			best_action.to.x -= (best_action.to.y - 3);

		std::cerr << "TREE SIZE " << tree_size << std::endl;

		return best_action;
	}

	inline std::tuple<int, int, int, int, int> MCTSAgent::external_find_best_action(const GameState & gamestate, Player me, Player enemy)
	{
		Action best_action = find_best_action(gamestate, me, enemy);
		return { best_action.from.x,best_action.from.y,best_action.to.x,best_action.to.y,best_action.final_orientation };
	}

	inline void MCTSAgent::selection()
	{
		Node* actual_node = &root;//Start from root

		path.clear();//Reset path for selection
		path.push(&root);//We put the root as the first node of the path

		Node* best_child;
		float best_uct;
		float parent_visits_sqrtLog;

		while (actual_node->childs.size() > 0 &&
			actual_node->childs.size() == actual_node->childs_count) {

			best_child = &actual_node->childs.front();
			parent_visits_sqrtLog = sqrtf(logf((float)actual_node->visits));
			best_uct = actual_node->childs.front().UCT(parent_visits_sqrtLog);

			for (Node& n : actual_node->childs) {

				float uct = n.UCT(parent_visits_sqrtLog);

				//If the uct of the actual child is better than the best child found so far, we update best child
				if (uct > best_uct) {
					best_uct = uct;
					best_child = &n;
				}
			}

			//We do the action that corresponds to the last node we saw
			actual.Do_action(best_child->action);

			actual_node = best_child;
			path.push(actual_node);
		}

	}

	inline void MCTSAgent::expansion()
	{
		Node* expansion_node = path.top();

		if (expansion_node->status == Node::IN_PROGRESS) {
			if (expansion_node->childs.size() == 0) {
				//If the game has finished set node status to finished and don't expand further
				if (!actual.Is_in_progress()) {
					expansion_node->status = Node::FINISHED;
					return;
				}

				expansion_node->childs = std::vector<Node>(actual.Get_moves(actions),Node());
				expansion_node->childs_count = 0;

				for (uint8_t i = 0; i < expansion_node->childs.size(); i++) {
					expansion_node->childs[i].action = actions[i];
				}
			}
			//Find the next child index in nodes_pool
			Node* next_child = &expansion_node->childs[expansion_node->childs_count];

			//Execute the nth action in the gamestate
			actual.Do_action(next_child->action);
			//Increment in 1 the amount of childs expanded from this node
			expansion_node->childs_count++;
			tree_size++;
			//push the new child to the path of nodes
			path.push(next_child);

		}
	}

	inline void MCTSAgent::rollout()
	{
		Player player_turn = actual.Get_player_turn();

		if (actual.get_winner() != -1) {
			rollout_scores[player_turn] = -1.0f;
			rollout_scores[!player_turn] = 1.0f;
		}
		else {
			float score = rollout_agent.score_function(actual, !player_turn, player_turn) / rollout_agent_score_normalizer;

			rollout_scores[player_turn] = -score;
			rollout_scores[!player_turn] = score;
		}

		/*
		Player player_turn = actual.Get_player_turn();

		copy = actual;

		while (copy.Is_in_progress()) {
			uint8_t amount = copy.Get_moves(actions);
			copy.Do_action(actions[rnd(amount)]);
		}

		if(copy.get_winner() == -1) {
			rollout_scores[player_turn] = 0.0f;
			rollout_scores[!player_turn] = 0.0f;
		}else if (copy.get_winner() == !player_turn) {
			rollout_scores[player_turn] = -1.0f;
			rollout_scores[!player_turn] = 1.0f;
		}
		else {
			rollout_scores[player_turn] = 1.0f;
			rollout_scores[!player_turn] = -1.0f;
		}
		*/
		/*
		Player player_turn = actual.Get_player_turn();

		if (actual.get_winner() == !player_turn) {
			rollout_scores[player_turn] = -1.0f;
			rollout_scores[!player_turn] = 1.0f;
		}
		else {
			rollout_scores[player_turn] = (actual.Get_token_amount(player_turn) - actual.Get_token_amount(!player_turn)) / 7.0f;
			rollout_scores[!player_turn] = -rollout_scores[player_turn];
		}*/
	}

	inline void MCTSAgent::backpropagation()
	{
		Player score_i = !actual.Get_player_turn();
		while (!path.empty()) {
			path.top()->wins += rollout_scores[score_i];
			path.top()->visits++;
			score_i = !score_i;
			path.pop();

			if(path.size() != 0)
				actual.Undo_action();
		}
	}

}