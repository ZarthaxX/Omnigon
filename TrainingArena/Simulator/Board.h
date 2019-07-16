#pragma once

#include <string>
#include <vector>
#include <map>
#include <ostream>



struct Point {
	Point() {}
	Point(int _x, int _y) : x(_x), y(_y) {}
	bool operator==(const Point& p) { return x == p.x && y == p.y; }
	int x, y;
};

enum {
	HELIOS = 0,
	BIGON,
	TRIGON,
	STARGON
};

struct Piece {
	Piece() {}
	Piece(const Point& p, int _orientation,int _type) :
		location(p), orientation(_orientation) , type(_type){}
	bool operator<(const Piece& p) {
		return true;
	}
	int orientation;
	Point location;
	int type;
};

struct Action {
	Action() {}
	Action(Point _from,Point _to,int _orientation) :
		from(_from),to(_to), orientation(_orientation) {}

	Action(std::vector<int> action) {
		from.x = action[0];
		from.y = action[1];
		to.x = action[2];
		to.y = action[3];
		orientation = action[4];
	}

	static friend std::ostream& operator<<(std::ostream &out, const Action &a) {
		out << a.from.x << " " << a.from.y << " " << a.to.x << " " << a.to.y << " " << a.orientation;
		return out;
	}

	Point from;
	Point to;
	int orientation;
	std::vector<int> to_vector() { return { from.x,from.y,to.x,to.y,orientation }; }
};

class Board
{
public:
	Board();
	~Board();
	void move_token(Action action);
	static friend std::ostream& operator<<(std::ostream &out, const Board &b) {
		for (int y = 0; y < 7; y++) {
			std::string offset;
			int i = abs(3 - y) * 2;
			while (i-- > 0)offset += " ";
			out << offset;
			int external_start = (y > 3) ? y - 3 : 0;
			int end = (y < 3) ? 7-(3-y) : 7;
			for (int x = external_start; x < end; x++) {
				if (b.grid[y][x] != -1)
					out << b.tokens[b.grid[y][x]].player << token_map_int_str[b.tokens[b.grid[y][x]].type][0] << b.tokens[b.grid[y][x]].orientation << " ";
				else
					out << "--- ";
			}
			out << std::endl;
		}
		return out;
	}

	int get_moves_count(int player, Piece & p) const;
	std::vector<int> get_token_attacks(int player, Piece& p) const;
	std::vector<int> get_token_threats(int player, Piece& p) const;
	int get_token_threats_count(int player, Piece & p) const;
	std::vector<Action> get_moves(int player) const;
	std::vector<Piece> get_type_piece_data(int player) const; 
	int get_type_piece_count(int player) const;
	void undo_action();
	void do_action(Action action);

	void external_init();
	void external_start();
	bool external_ended();
	void external_do_action(std::tuple<int,int,int,int,int> action);
	void external_set_first_player(bool first);
	int winner();

	void add_token(std::string type, int orientation, Point p, int player);
private:

	bool has_moves(int player) const;
	bool valid_cell(const Point& p) const;
	Point external_point(Point p) const;
	Point internal_point(Point p) const;
	static std::map<int, std::string>token_map_int_str;
	static std::map<std::string, int>token_map_str_int;
	static std::map<int, std::vector<std::vector<int>>>token_directions;
	static std::vector<Point> directions;

	struct Token {
		Token() {}
		Token(int _player,int _type, int _x, int _y, int _orientation) :
			player(_player),type(_type), x(_x),xC(_x), y(_y),yC(_y), orientation(_orientation),orientationC(_orientation) , alive(true){}
		int type;
		int x,xC;
		int y,yC;
		int orientation,orientationC;
		int player;
		bool alive;
	};

	struct CacheItem {
		Point from, to;
		int orientation;
		int dead_token;
	};

	int grid[7][7];
	std::vector<Token>tokens;

	CacheItem actions_cache[50];

	int turns_left;
	bool player_turn;
	int _winner;
};

