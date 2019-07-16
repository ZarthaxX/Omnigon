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

struct Piece {
	Piece() {}
	Piece(const Point& p, int _orientation,std::string _type) :
		location(p), orientation(_orientation) , type(_type){}
	bool operator<(const Piece& p) {
		return true;
	}
	int orientation;
	Point location;
	std::string type;
};

struct Action {
	Action() {}
	Action(Point _from,Point _to,int _orientation) :
		from(_from),to(_to), orientation(_orientation) {}

	static friend std::ostream& operator<<(std::ostream &out, const Action &a) {
		out << a.from.x << " " << a.from.y << " " << a.to.x << " " << a.to.y << " " << a.orientation;
		return out;
	}

	Point from;
	Point to;
	int orientation;
};

class Board
{
public:
	Board();
	~Board();
	void add_token(int player,std::string type,int x,int y,int orientation);
	void move_token(Action action);
	static friend std::ostream& operator<<(std::ostream &out, const Board &b) {
		for (int y = 0; y < 7; y++) {
			std::string offset;
			int i = abs(3 - y) * 2;
			while (i-- > 0)offset += " ";
			out << offset;
			int start = (y > 3) ? y - 3 : 0;
			int end = (y < 3) ? 7-(3-y) : 7;
			for (int x = start; x < end; x++) {
				if (b.grid[y][x] != -1)
					out << b.tokens[b.grid[y][x]].type[0] << "-" << b.tokens[b.grid[y][x]].orientation << " ";
				else
					out << "--- ";
			}
			out << std::endl;
		}
		return out;
	}
	std::vector<Action> get_moves(int player) const;
	std::vector<Piece> get_type_piece_data(int player) const;
private:

	bool valid_cell(const Point& p) const;
	Point external_point(Point p) const;
	Point internal_point(Point p) const;
	static std::map<std::string, std::vector<std::vector<int>>>token_directions;
	static std::vector<Point> directions;

	struct Token {
		Token() {}
		Token(int _player,std::string _type, int _x, int _y, int _orientation) :
			player(_player),type(_type), x(_x), y(_y), orientation(_orientation) , alive(true) {}
		std::string type;
		int x;
		int y;
		int orientation;
		int player;
		bool alive;
	};

	int grid[7][7];
	std::vector<Token>tokens;
};

