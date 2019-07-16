#pragma once
#include <iostream>
#include <algorithm>
#include <vector>
#include <map>
#include <ostream>
#include <string>

#include "Constants.h"

namespace MinMaxGon {

	struct Point {
		Point() {}
		Point(uint8_t _x, uint8_t _y) : x(_x), y(_y) {}
		bool operator==(const Point& p) { return x == p.x && y == p.y; }
		uint8_t x,y;
	};

	enum TokenType {
		HELIOS = 0,
		BIGON,
		TRIGON,
		STARGON
	};

	typedef bool Player;

	struct Action {
		Action(){}
		Action(Point& _from,Point& _to,uint8_t _final_orientation) : 
			from(_from),to(_to),final_orientation(_final_orientation){}

		Action(std::vector<int> action) {
			from.x = action[0];
			from.y = action[1];
			to.x = action[2];
			to.y = action[3];
			final_orientation = action[4];
		}

		Point from, to;
		uint8_t final_orientation;

		std::vector<int> to_vector() { return { from.x,from.y,to.x,to.y,final_orientation }; }
	};

	const uint8_t NULL_TOKEN_INDEX = 255;

	struct Token {
		Token(uint8_t _type, uint8_t _orientation, uint8_t _x, uint8_t _y, Player _player) :
			type(_type), orientation(_orientation), x(_x), y(_y), player(_player),alive(true) {}
		uint8_t type;
		uint8_t orientation;
		uint8_t x, y;
		Player player;
		bool alive;
	};

	enum CELL_TYPE {
		EMPTY = 0,
		OBSTACLE
	};

	class Board
	{
	public:
		Board(uint8_t _max_turns);
		Board(uint8_t _turns_left,Player _player_turn);
		~Board();

		static friend std::ostream& operator<<(std::ostream &out, const Board &b) {
			std::map<int, std::string> token_map_int_str = {
				{HELIOS,"HELIOS"},
				{BIGON,"BIGON"},
				{TRIGON,"TRIGON"},
				{STARGON,"STARGON"}
			};
			for (int y = 0; y < 7; y++) {
				std::string offset;
				int i = abs(3 - y) * 2;
				while (i-- > 0)offset += " ";
				out << offset;
				int external_start = (y > 3) ? y - 3 : 0;
				int end = (y < 3) ? 7 - (3 - y) : 7;
				for (int x = external_start; x < end; x++) {
					if (b.grid[y][x] != -1)
						out << (int)b.tokens[b.grid[y][x]].player << token_map_int_str[b.tokens[b.grid[y][x]].type][0] << (int)b.tokens[b.grid[y][x]].orientation << " ";
					else
						out << "--- ";
				}
				out << std::endl;
			}
			return out;
		}

		void Add_token(uint8_t type, uint8_t x, uint8_t y, uint8_t orientation, Player player);
		void Do_action(const Action& action);
		void Undo_action();
		const std::vector<Token>& Get_tokens() const;
		uint8_t Get_token_amount(Player player) const;
		uint8_t Get_moves(std::vector<Action>&actions) const; 
		bool Is_in_progress() const;
		int8_t get_winner() const;
		
		void external_do_action(std::tuple<int,int,int,int,int> action);
		void external_start(bool first_player);
		void external_init();
		bool external_ended();
		void external_set_first_player(bool first);
		int external_get_winner();
	private:
		bool Has_moves() const;
		bool valid_cell(const Point& p) const;
		bool Token_can_move(const Token& t, const Point& to, uint8_t next_orientation) const;

		uint8_t turns_left;
		Player player_turn;
		int8_t winner;
		int8_t grid[7][7];
		uint8_t max_turns;

		std::vector<Token> tokensCopy;
		std::vector<Token> tokens;
		static std::vector<std::vector<std::vector<int>>>token_attacks;
		static std::vector<std::vector<std::vector<int>>>token_defenses;
		static std::vector<Point> directions;

		struct CacheItem {
			CacheItem() {}
			CacheItem(uint8_t _orientation, Point& _from,Point& _to, uint8_t _dead_token) :
				orientation(_orientation), to(_to),from(_from), dead_token(_dead_token) {}
			uint8_t orientation;
			Point from, to;
			int8_t dead_token;
		};

		CacheItem history_cache[Constants::MAX_TURNS];
		uint8_t token_amount[2];
	};

}