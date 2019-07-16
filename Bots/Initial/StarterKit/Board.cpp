#include "Board.h"

std::map<std::string, std::vector<std::vector<int>>> Board::token_directions = {
	{"HELIOS",{ {1,1,1,1,1,1} } },
	{"BIGON",{ {1,0,0,0,0,1},{1,1,0,0,0,0},{0,1,1,0,0,0},{0,0,1,1,0,0 },{0,0,0,1,1,0},{0,0,0,0,1,1} } },
	{"TRIGON",{ {1,1,0,0,0,1},{1,1,1,0,0,0},{0,1,1,1,0,0},{0,0,1,1,1,0},{0,0,0,1,1,1},{1,0,0,0,1,1} } },
	{"STARGON",{{0,1,0,1,0,1},{1,0,1,0,1,0} }}
};

std::vector<Point> Board::directions = { 
	Point(0, -1), 
	Point(1, 0),
	Point(1, 1),
	Point(0, 1),
	Point(-1, 0),
	Point(-1, -1) 
};

Board::Board()
{
	for (int y = 0; y < 7; y++)
		for (int x = 0; x < 7; x++)
			grid[y][x] = -1;
}


Board::~Board()
{
}

void Board::add_token(int player, std::string type, int x, int y, int orientation)
{
	Point p = internal_point(Point(x, y));
	grid[p.y][p.x] = tokens.size();
	tokens.push_back(Token(player, type, p.x, p.y, orientation));
}

void Board::move_token(Action action)
{
	Point to = internal_point(Point(action.to.x, action.to.y));
	Point from = internal_point(Point(action.from.x, action.from.y));
	int index_to = grid[to.y][to.x];
	int index_from = grid[from.y][from.x];
	if (index_to != -1) {
		tokens[index_to].alive = false;
	}
	grid[to.y][to.x] = index_from;
	grid[from.y][from.x] = -1;
	tokens[index_from].x = to.x;
	tokens[index_from].y = to.y;
	tokens[index_from].orientation = action.orientation;
}

std::vector<Action> Board::get_moves(int player) const
{
	std::vector<Action> actions;

	for (Token t : tokens) {
		if (t.player == player && t.alive == true) {
			Point initial(t.x, t.y);
			for (int i = 0; i < 6; i++) {
				bool has_arrow = Board::token_directions.at(t.type)[t.orientation][i];
				Point direction = directions[i];

				if (has_arrow) {
					Point position = initial;
					position.x += direction.x;
					position.y += direction.y;

					while (valid_cell(position)) {
						if (grid[position.y][position.x] != -1) {
							Token token = tokens[grid[position.y][position.x]];

							if (token.player != player) {
								if (token.type == "HELIOS" || Board::token_directions[token.type][token.orientation][(i + 3) % 6] == false) {
									for (int o = 0; o < Board::token_directions[t.type].size(); o++) {
										actions.push_back(Action(external_point(initial),external_point(position), o));
									}
								}
							}
							break;
						}
						else {
							for (int o = 0; o < Board::token_directions[t.type].size();o++) {
								actions.push_back(Action(external_point(initial), external_point(position), o));
							}
						}

						position.x += direction.x;
						position.y += direction.y;

						if (t.type == "HELIOS")
							break;
					}
				}
			}
		}
	}	
	return actions;
}

std::vector<Piece> Board::get_type_piece_data(int player) const
{
	std::vector<Piece> pieces;

	for (Token t : tokens) {
		if (t.player == player && t.alive == true)
			pieces.push_back(Piece(external_point(Point(t.x, t.y)), t.orientation,t.type));
	}

	return pieces;
}

bool Board::valid_cell(const Point & p) const
{
	if (p.y == 0 && 0 <= p.x && p.x <= 3)
		return true;
	else if (p.y == 1 && 0 <= p.x && p.x <= 4)
		return true;
	else if (p.y == 2 && 0 <= p.x && p.x <= 5)
		return true;
	else if (p.y == 3 && 0 <= p.x && p.x <= 6)
		return true;
	else if (p.y == 4 && 1 <= p.x && p.x <= 6)
		return true;
	else if (p.y == 5 && 2 <= p.x && p.x <= 6)
		return true;
	else if (p.y == 6 && 3 <= p.x && p.x <= 6)
		return true;
	else
		return false;
}

Point Board::external_point(Point p) const
{
	if(p.y > 3)
		p.x -= (p.y - 3);
	return p;
}

Point Board::internal_point(Point p) const
{
	if(p.y > 3)
		p.x += (p.y - 3);
	return p;
}
