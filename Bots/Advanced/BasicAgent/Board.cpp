#include "Board.h"

std::map<int, std::string> Board::token_map_int_str = {
	{HELIOS,"HELIOS"},
	{BIGON,"BIGON"},
	{TRIGON,"TRIGON"},
	{STARGON,"STARGON"}
};

std::map<std::string, int> Board::token_map_str_int = { 
	{"HELIOS",HELIOS},
	{"BIGON",BIGON},
	{"TRIGON",TRIGON},
	{"STARGON",STARGON} 
};

std::map<int, std::vector<std::vector<int>>> Board::token_directions = {
	{HELIOS,{ {1,1,1,1,1,1} } },
	{BIGON,{ {1,0,0,0,0,1},{1,1,0,0,0,0},{0,1,1,0,0,0},{0,0,1,1,0,0 },{0,0,0,1,1,0},{0,0,0,0,1,1} } },
	{TRIGON,{ {1,1,0,0,0,1},{1,1,1,0,0,0},{0,1,1,1,0,0},{0,0,1,1,1,0},{0,0,0,1,1,1},{1,0,0,0,1,1} } },
	{STARGON,{{0,1,0,1,0,1},{1,0,1,0,1,0} }}
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
	player_turn = 0;
	turns_left = 50;
	_winner = -1;

	for (int y = 0; y < 7; y++)
		for (int x = 0; x < 7; x++)
			grid[y][x] = -1;

}

Board::Board(int _max_turns)
{
	max_turns = _max_turns;
}


Board::~Board()
{
}

void Board::external_init() {

	add_token("HELIOS", 0, Point(1, 3), 0);
	add_token("HELIOS", 0, Point(5, 3), 1);
	add_token("TRIGON", 1, Point(0, 3), 0);
	add_token("TRIGON", 4, Point(6, 3), 1);
	add_token("STARGON", 1, Point(0, 2), 0);
	add_token("STARGON", 1, Point(2, 3), 0);
	add_token("STARGON", 1, Point(0, 4), 0);
	add_token("STARGON", 0, Point(5, 2), 1);
	add_token("STARGON", 0, Point(4, 3), 1);
	add_token("STARGON", 0, Point(5, 4), 1);
	add_token("BIGON", 0, Point(1, 2), 0);
	add_token("BIGON", 3, Point(1, 4), 0);
	add_token("BIGON", 0, Point(4, 2), 1);
	add_token("BIGON", 3, Point(4, 4), 1);

}

void Board::add_token(std::string type,int orientation,  Point p, int player)
{
	p = internal_point(p);
	grid[p.y][p.x] = tokens.size();
	tokens.push_back(Token(player, token_map_str_int[type], p.x, p.y, orientation));
}

void Board::undo_action()
{
	CacheItem item = actions_cache[turns_left];
	int index_to = grid[item.to.y][item.to.x];
	grid[item.from.y][item.from.x] = index_to;
	grid[item.to.y][item.to.x] = item.dead_token;

	if (item.dead_token != -1) {
		if (tokens[item.dead_token].type == HELIOS) {
			_winner = -1;
		}
		tokens[item.dead_token].alive = true;
	}

	tokens[index_to].x = item.from.x;
	tokens[index_to].y = item.from.y;
	tokens[index_to].orientation = item.orientation;

	turns_left++;
}

void Board::move_token(Action action)
{
	CacheItem item;

	Point to = internal_point(Point(action.to.x, action.to.y));
	Point from = internal_point(Point(action.from.x, action.from.y));

	item.from = from;
	item.to = to;

	int index_to = grid[to.y][to.x];
	int index_from = grid[from.y][from.x];

	item.orientation = tokens[index_from].orientation;
	item.dead_token = index_to;

	if (index_to != -1) {

		if (tokens[index_to].type == HELIOS) {
			_winner = player_turn;
		}

		tokens[index_to].alive = false;
	}
	grid[to.y][to.x] = index_from;
	grid[from.y][from.x] = -1;
	tokens[index_from].x = to.x;
	tokens[index_from].y = to.y;
	tokens[index_from].orientation = action.orientation;

	actions_cache[turns_left - 1] = item;

	if (!has_moves(!player_turn)) {
		_winner = player_turn;
	}
}

bool Board::has_moves(int player) const
{
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
								if (token.type == HELIOS || Board::token_directions[token.type][token.orientation][(i + 3) % 6] == false) {
									for (int o = 0; o < Board::token_directions[t.type].size(); o++) {
										return true;
									}
								}
							}
							break;
						}
						else {
							for (int o = 0; o < Board::token_directions[t.type].size(); o++) {
								return true;
							}
						}

						position.x += direction.x;
						position.y += direction.y;

						if (t.type == HELIOS)
							break;
					}
				}
			}
		}
	}
	return false;
}

std::vector<int> Board::get_token_attacks(int player, Piece & p) const
{
	std::vector<int> attacks;

	for (int i = 0; i < 6; i++) {
		bool has_arrow = Board::token_directions.at(p.type)[p.orientation][i];
		Point direction = directions[i];

		if (has_arrow) {
			Point position = p.location;
			position.x += direction.x;
			position.y += direction.y;

			while (valid_cell(position)) {
				if (grid[position.y][position.x] != -1) {
					Token token = tokens[grid[position.y][position.x]];

					if (token.player != player) {
						if (token.type == HELIOS || Board::token_directions[token.type][token.orientation][(i + 3) % 6] == false) {
							attacks.push_back(token.type);
						}
					}
					break;
				}

				position.x += direction.x;
				position.y += direction.y;

				if (p.type == HELIOS)
					break;
			}
		}
	}

	return attacks;
}

std::vector<int> Board::get_token_threats(int player, Piece & p) const
{
	std::vector<int> threats;

	for (int i = 0; i < 6; i++) {
		bool has_arrow = Board::token_directions.at(p.type)[p.orientation][i];
		Point direction = directions[i];

		if (!has_arrow) {
			Point position = p.location;
			position.x += direction.x;
			position.y += direction.y;

			int steps = 1;

			while (valid_cell(position)) {
				if (grid[position.y][position.x] != -1) {
					Token token = tokens[grid[position.y][position.x]];

					if (token.player != player) {
						if ((token.type == HELIOS && steps == 1) || (token.type != HELIOS && token_directions[token.type][token.orientation][(i + 3) % 6] == true)) {
							threats.push_back(token.type);
						}
					}
					break;
				}

				position.x += direction.x;
				position.y += direction.y;

				steps++;
			}
		}
	}

	return threats;
}

int Board::get_token_threats_count(int player, Piece & p) const
{
	int threats = 0;

	for (int i = 0; i < 6; i++) {
		bool has_arrow = Board::token_directions.at(p.type)[p.orientation][i];
		Point direction = directions[i];

		if (!has_arrow) {
			Point position = p.location;
			position.x += direction.x;
			position.y += direction.y;

			int steps = 1;

			while (valid_cell(position)) {
				if (grid[position.y][position.x] != -1) {
					Token token = tokens[grid[position.y][position.x]];

					if (token.player != player) {
						if ((token.type == HELIOS && steps == 1) || (token.type != HELIOS && token_directions[token.type][token.orientation][(i + 3) % 6] == true)) {
							threats++;
						}
					}
					break;
				}

				position.x += direction.x;
				position.y += direction.y;

				steps++;
			}
		}
	}

	return threats;
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
								if (token.type == HELIOS || Board::token_directions[token.type][token.orientation][(i + 3) % 6] == false) {
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

						if (t.type == HELIOS)
							break;
					}
				}
			}
		}
	}	
	return actions;
}


int Board::get_moves_count(int player, Piece & p) const
{
	int moves = 0;

	for (int i = 0; i < 6; i++) {
		bool has_arrow = token_directions.at(p.type)[p.orientation][i];
		Point direction = directions[i];

		if (has_arrow) {
			Point position = p.location;
			position.x += direction.x;
			position.y += direction.y;

			while (valid_cell(position)) {
				if (grid[position.y][position.x] != -1) {
					Token token = tokens[grid[position.y][position.x]];

					if (token.player != player) {
						if (token.type == HELIOS || token_directions[token.type][token.orientation][(i + 3) % 6] == false) {
							moves++;
						}
					}
					break;
				}
				else {
					moves++;
				}

				position.x += direction.x;
				position.y += direction.y;

				if (p.type == HELIOS)
					break;
			}
		}
	}

	return moves;
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

int Board::get_type_piece_count(int player) const
{
	int pieces = 0;

	for (Token t : tokens) {
		if (t.player == player && t.alive == true)
			pieces++;
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

void Board::do_action(Action action) {
	move_token(action);
	turns_left--;
	player_turn = !player_turn;
}


void Board::external_start(bool first_player) {
	player_turn = first_player;
	turns_left = 50;
	_winner = -1;

	for (int y = 0; y < 7; y++)
		for (int x = 0; x < 7; x++)
			grid[y][x] = -1;

	for (int i = 0; i < 14; i++) {
		tokens[i].x = tokens[i].xC;
		tokens[i].y = tokens[i].yC;
		tokens[i].orientation = tokens[i].orientationC;
		tokens[i].alive = true;
		grid[tokens[i].y][tokens[i].x] = i;
	}
}

bool Board::external_ended() {
	return !(_winner == -1 && turns_left > 0);
}

void Board::external_do_action(std::tuple<int,int,int,int,int> a) {
	do_action(Action(Point(std::get<0>(a), std::get<1>(a)), Point(std::get<2>(a), std::get<3>(a)), std::get<4>(a)));
}

void Board::external_set_first_player(bool first) {
	player_turn = first;
}

int Board::winner() {
	return _winner;
}