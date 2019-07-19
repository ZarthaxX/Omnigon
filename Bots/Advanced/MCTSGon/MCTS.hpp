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

#include "Constants.h"
#include "Utilities.h"
#include "FixedStack.hpp"
#include "GameState.h"

namespace MCTSGon {

	class MCTSAgent {
	public:
		MCTSAgent();
		~MCTSAgent();
		MCTSAgent& operator=(const MCTSAgent& mcts);

		Action find_best_action(const GameState& gamestate,Player me,Player enemy);
	private:
		struct Node {
			Node() : visits(0), wins(0), childs(0), childs_count(0), status(0), first_child(UINT32_MAX) {}
			inline float UCT(float sqrtLogVisits, float* invSqrt);
			uint32_t visits;
			uint8_t status;
			float wins;
			uint8_t childs;
			uint8_t childs_count;
			uint32_t first_child;
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

		int tree_size;
		int max_nodes;
		Node* nodes_pool;
		FixedStack<int> path;

		float* sqrtLog;
		float* invSqrt;
		float* inv;

		static const int MAX_DEPTH;
		static const int CACHE;
		static const float COEFF;
	};

	const int MCTSAgent::CACHE = 1000000;
	const float MCTSAgent::COEFF = sqrt(2.0f);
	const int MCTSAgent::MAX_DEPTH = 10000;


	inline float MCTSAgent::Node::UCT(float sqrtLogVisits,float* invSqrt)
	{
		float invsqrt = invSqrt[this->visits];
		return ((wins * invsqrt) + sqrtLogVisits) * invsqrt;
	}

	MCTSAgent::MCTSAgent() : path(MAX_DEPTH) {
		nodes_pool = new Node[Constants::NODES_POOL_SIZE];
		max_nodes = Constants::NODES_POOL_SIZE;
		tree_size = 1;
		actions = std::vector<Action>(255, Action());

		sqrtLog = new float[CACHE];
		invSqrt = new float[CACHE];

		sqrtLog[0] = 0;
		invSqrt[0] = 1;

		for (uint32_t i = 1; i < CACHE; i++)
		{
			sqrtLog[i] = COEFF * sqrtf(logf((float)(i)));
			invSqrt[i] = 1.0f / sqrtf((float)(i));
		}

		inv = new float[9 * 9];

		for (int i = 0; i < 81; i++) {
			inv[i] = 1.0f / (i + 1);
		}
	}

	MCTSAgent::~MCTSAgent() {
		delete[] nodes_pool;
		delete[] sqrtLog;
		delete[] invSqrt;
		delete[] inv;
	}

	MCTSAgent& MCTSAgent::operator=(const MCTSAgent& mcts) {
		delete[] nodes_pool;
		nodes_pool = new Node[mcts.max_nodes];
		max_nodes = mcts.max_nodes;

		memcpy(nodes_pool, mcts.nodes_pool, max_nodes * sizeof(Node));

		return *this;
	}

	inline Action MCTSAgent::find_best_action(const GameState & gamestate, Player me, Player enemy)
	{
		stopwatch.Start(Constants::TIMEOUT_MICRO_SECONDS);

		actual = gamestate;
		copy = gamestate;

		memset(nodes_pool, 0, max_nodes * sizeof(Node));

		tree_size = 1;

		while (!stopwatch.Timeout()) {
			selection();
			expansion();
			rollout();
			backpropagation();
		}

		std::cerr << "CREATED " << tree_size << " NODES " << std::endl;

		uint8_t best_child = 1;
		uint32_t best_visits = 0;
		for (uint8_t i = 0; i < nodes_pool[0].childs; i++) {
			//If the uct of the actual child is better than the best child found so far, we update best child
			if (nodes_pool[1 + i].visits > best_visits) {
				best_visits = nodes_pool[1 + i].visits;
				best_child = i;
			}
		}

		Action best_action = nodes_pool[1 + best_child].action;
		
		if (best_action.from.y > 3)
			best_action.from.x -= (best_action.from.y - 3);

		if (best_action.to.y > 3)
			best_action.to.x -= (best_action.to.y - 3);

		std::cerr << "TREE SIZE " << tree_size << std::endl;

		return best_action;
	}

	inline void MCTSAgent::selection()
	{
		int actual_node = 0;//Start from root

		path.clear();//Reset path for selection
		path.push(0);//We put the root as the first node of the path

		int first_child;
		int best_child;
		float best_uct;
		float parent_visits_sqrtLog;

		while (nodes_pool[actual_node].childs > 0 &&
			nodes_pool[actual_node].childs == nodes_pool[actual_node].childs_count) {

			first_child = nodes_pool[actual_node].first_child;
			best_child = first_child;
			parent_visits_sqrtLog = sqrtLog[nodes_pool[actual_node].visits];
			best_uct = nodes_pool[first_child + 0].UCT(parent_visits_sqrtLog, invSqrt);

			for (int i = 0; i < nodes_pool[actual_node].childs; i++) {

				float uct = nodes_pool[first_child + i].UCT(parent_visits_sqrtLog, invSqrt);

				//If the uct of the actual child is better than the best child found so far, we update best child
				if (uct > best_uct) {
					best_uct = uct;
					best_child = first_child + i;
				}
			}

			//We do the action that corresponds to the last node we saw
			actual.Do_action(nodes_pool[best_child].action);

			actual_node = best_child;
			path.push(actual_node);
		}

	}

	inline void MCTSAgent::expansion()
	{
		int expansion_node = path.top();

		if (nodes_pool[expansion_node].status == Node::IN_PROGRESS) {
			if (nodes_pool[expansion_node].childs == 0) {
				//If the game has finished set node status to finished and don't expand further
				if (!actual.Is_in_progress()) {
					nodes_pool[expansion_node].status = Node::FINISHED;
					return;
				}

				//nodes_pool[expansion_node].childs = actual.possible_actions();
				nodes_pool[expansion_node].childs = actual.Get_moves(actions);
				nodes_pool[expansion_node].childs_count = 0;
				nodes_pool[expansion_node].first_child = tree_size;
				tree_size += nodes_pool[expansion_node].childs;

				if (tree_size > max_nodes) {
					std::cout << "E R R O R" << std::endl;
				}

				for (uint8_t i = 0; i < nodes_pool[expansion_node].childs; i++) {
					nodes_pool[nodes_pool[expansion_node].first_child + i].action = actions[i];
				}
			}
			//Find the next child index in nodes_pool
			int next_child = nodes_pool[expansion_node].childs_count + nodes_pool[expansion_node].first_child;

			//Execute the nth action in the gamestate
			actual.Do_action(nodes_pool[next_child].action);
			//Increment in 1 the amount of childs expanded from this node
			nodes_pool[expansion_node].childs_count++;
			//push the new child to the path of nodes
			path.push(next_child);

		}
	}

	inline void MCTSAgent::rollout()
	{

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
			nodes_pool[path.top()].wins += rollout_scores[score_i];
			nodes_pool[path.top()].visits++;
			score_i = !score_i;
			path.pop();

			if(path.size() != 0)
				actual.Undo_action();
		}
	}

}