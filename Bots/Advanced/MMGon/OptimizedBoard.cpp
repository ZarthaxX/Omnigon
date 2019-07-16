#include "OptimizedBoard.h"



namespace MinMaxGon {

	std::vector<std::vector<std::vector<int>>> Board::token_attacks = {
		{ {1,1,1,1,1,1} },
		{ {1,0,0,0,0,1},{1,1,0,0,0,0},{0,1,1,0,0,0},{0,0,1,1,0,0 },{0,0,0,1,1,0},{0,0,0,0,1,1} },
		{ {1,1,0,0,0,1},{1,1,1,0,0,0},{0,1,1,1,0,0},{0,0,1,1,1,0},{0,0,0,1,1,1},{1,0,0,0,1,1} },
		{{0,1,0,1,0,1},{1,0,1,0,1,0} }
	};

	std::vector<std::vector<std::vector<int>>> Board::token_defenses = {
		{ {0,0,0,0,0,0} },
		{ {1,0,0,0,0,1},{1,1,0,0,0,0},{0,1,1,0,0,0},{0,0,1,1,0,0 },{0,0,0,1,1,0},{0,0,0,0,1,1} },
		{ {1,1,0,0,0,1},{1,1,1,0,0,0},{0,1,1,1,0,0},{0,0,1,1,1,0},{0,0,0,1,1,1},{1,0,0,0,1,1} },
		{{0,1,0,1,0,1},{1,0,1,0,1,0} }
	};

	std::vector<Point> Board::directions = {
		Point(0, -1),
		Point(1, 0),
		Point(1, 1),
		Point(0, 1),
		Point(-1, 0),
		Point(-1, -1)
	};

	Board::Board(uint8_t _max_turns)
	{
		max_turns = _max_turns;
	}

	Board::Board(uint8_t _turns_left, Player _player_turn)
	{
		player_turn = _player_turn;
		turns_left = _turns_left;
		winner = -1;

		for (int y = 0; y < 7; y++)
			for (int x = 0; x < 7; x++)
				grid[y][x] = -1;

		token_amount[0] = 0;
		token_amount[1] = 0;
	}


	Board::~Board()
	{
	}

	void Board::Add_token(uint8_t type, uint8_t x, uint8_t y, uint8_t orientation, Player player)
	{
		if (y > 3)
			x += (y - 3);

		grid[y][x] = (int8_t)tokens.size();
		tokens.push_back(Token(type, orientation, x, y, player));
		
		token_amount[player]++;
	}

	void Board::Do_action(const Action & action)
	{
		CacheItem item;

		int index_to = grid[action.to.y][action.to.x];
		int index_from = grid[action.from.y][action.from.x];

		item.from.x = action.from.x;
		item.from.y = action.from.y;
		item.to.x = action.to.x;
		item.to.y = action.to.y;
		item.orientation = tokens[index_from].orientation;
		item.dead_token = index_to;

		if (index_to != -1) {

			if (tokens[index_to].type == HELIOS) {
				winner = player_turn;
			}

			tokens[index_to].alive = false;
			token_amount[tokens[index_to].player]--;
		}
		grid[action.to.y][action.to.x] = index_from;
		grid[action.from.y][action.from.x] = -1;
		tokens[index_from].x = action.to.x;
		tokens[index_from].y = action.to.y;
		tokens[index_from].orientation = action.final_orientation;

		player_turn = !player_turn;

		if (!Has_moves()) {
			winner = !player_turn;
		}

		turns_left--;
		history_cache[turns_left] = item;
	}

	void Board::Undo_action()
	{
		CacheItem item = history_cache[turns_left];
		int index_to = grid[item.to.y][item.to.x];
		grid[item.from.y][item.from.x] = index_to;
		grid[item.to.y][item.to.x] = item.dead_token;

		if (item.dead_token != -1) {
			tokens[item.dead_token].alive = true;
			token_amount[tokens[item.dead_token].player]++;
		}

		tokens[index_to].x = item.from.x;
		tokens[index_to].y = item.from.y;
		tokens[index_to].orientation = item.orientation;

		turns_left++;
		player_turn = !player_turn;
		winner = -1;
	}

	const std::vector<Token>& Board::Get_tokens() const
	{
		return tokens;
	}

	uint8_t Board::Get_token_amount(Player player) const
	{
		return token_amount[player];
	}

	uint8_t Board::Get_moves(std::vector<Action>& actions) const
	{
		uint8_t actions_count = 0;

		for (const Token& t : tokens) {
			if (t.player == player_turn && t.alive == true) {
				Point initial(t.x, t.y);
				for (int i = 0; i < 6; i++) {
					bool has_arrow = token_attacks[t.type][t.orientation][i];
					Point direction = directions[i];

					if (has_arrow) {
						Point position = initial;
						position.x += direction.x;
						position.y += direction.y;

						while (valid_cell(position)) {
							if (grid[position.y][position.x] != -1) {
								Token token = tokens[grid[position.y][position.x]];

								if (token.player != player_turn) {
									if (token_defenses[token.type][token.orientation][(i + 3) % 6] == false) {
										for (uint8_t o = 0; o < token_attacks[t.type].size(); o++) {
											if (!Token_can_move(t,position, o))
												continue;
											actions[actions_count] = Action(initial, position, o);
											actions_count++;
										}
									}
								}
								break;
							}
							else {
								for (uint8_t o = 0; o < token_attacks[t.type].size(); o++) {
									if (!Token_can_move(t,position, o))
										continue; 
									actions[actions_count] = Action(initial, position, o);
									actions_count++;
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

		return actions_count;
	}

	bool Board::Has_moves() const
	{
		uint8_t actions_count = 0;

		for (Token t : tokens) {
			if (t.player == player_turn && t.alive == true) {
				Point initial(t.x, t.y);
				for (int i = 0; i < 6; i++) {
					bool has_arrow = token_attacks[t.type][t.orientation][i];
					Point direction = directions[i];

					if (has_arrow) {
						Point position = initial;
						position.x += direction.x;
						position.y += direction.y;

						if (valid_cell(position)) {
							if (grid[position.y][position.x] != -1) {
								Token token = tokens[grid[position.y][position.x]];

								if (token.player != player_turn && token_defenses[token.type][token.orientation][(i + 3) % 6] == false) {
									for (uint8_t o = 0; o < token_attacks[t.type].size(); o++) {
										if (!Token_can_move(t, position, o))
											continue;
										return true;
									}
								}
							}
							else {
								for (uint8_t o = 0; o < token_attacks[t.type].size(); o++) {
									if (!Token_can_move(t, position, o))
										continue;
									return true;
								}
							}
						}
					}
				}
			}
		}

		return false;
	}

	bool Board::Is_in_progress() const
	{
		return (winner == -1);
	}

	int8_t Board::get_winner() const
	{
		return winner;
	}

	bool Board::valid_cell(const Point & p) const
	{
		if (p.y <= 3)
			return 0 <= p.y && 0 <= p.x && p.x < 4 + p.y;
		else
			return p.y < 7 && p.y - 3 <= p.x && p.x < 7;
	}

	bool Board::Token_can_move(const Token& tok, const Point& to, uint8_t next_orientation) const
	{
		for (int i = 0; i < 6; i++) {
			if (!token_attacks[tok.type][next_orientation][i])
				continue;

			Point next(to.x + directions[i].x, to.y + directions[i].y);

			if (valid_cell(next)) {
				if (grid[next.y][next.x] == -1 || (next.x == tok.x && next.y == tok.y))
					return true;
				else {
					const Token& token = tokens[grid[next.y][next.x]];

					if (token.player != tok.player && token_defenses[token.type][token.orientation][(i + 3) % 6] == false) {
						return true;
					}
				}
			}
		}
		return false;
	}

	void Board::external_do_action(std::tuple<int, int, int, int, int> _action)
	{
		Point from(std::get<0>(_action),std::get<1>(_action));
		Point to(std::get<2>(_action), std::get<3>(_action));
		Action action(from, to, std::get<4>(_action));

		if (action.from.y > 3)
			action.from.x += (action.from.y - 3);

		if (action.to.y > 3)
			action.to.x += (action.to.y - 3);

		Do_action(action);
	}

	void Board::external_start(bool first_player)
	{
		for (int y = 0; y < 7; y++)
			for (int x = 0; x < 7; x++)
				grid[y][x] = -1;

		token_amount[0] = 0;
		token_amount[1] = 0;

		winner = -1;
		player_turn = first_player;
		turns_left = 50;

		for (int i = 0; i < tokens.size(); i++) {
			tokens[i].alive = true;
			tokens[i].x = tokensCopy[i].x;
			tokens[i].y = tokensCopy[i].y;
			tokens[i].orientation = tokensCopy[i].orientation;
			token_amount[tokens[i].player]++;
			grid[tokens[i].y][tokens[i].x] = i;
		}

	}

	void Board::external_init()
	{
		Add_token(HELIOS, 1, 3, 0, 0);
		Add_token(HELIOS, 5, 3, 0, 1);
		Add_token(TRIGON, 0, 3, 1, 0);
		Add_token(TRIGON, 6, 3, 4, 1);
		Add_token(STARGON, 0, 2, 1, 0);
		Add_token(STARGON, 2, 3, 1, 0);
		Add_token(STARGON, 0, 4, 1, 0);
		Add_token(STARGON, 5, 2, 0, 1);
		Add_token(STARGON, 4, 3, 0, 1);
		Add_token(STARGON, 5, 4, 0, 1);
		Add_token(BIGON, 1, 2, 0, 0);
		Add_token(BIGON, 1, 4, 3, 0);
		Add_token(BIGON, 4, 2, 0, 1);
		Add_token(BIGON, 4, 4, 3, 1);

		tokensCopy = tokens;
	}

	bool Board::external_ended()
	{
		return !Is_in_progress();
	}

	void Board::external_set_first_player(bool first)
	{
		player_turn = first;
	}

	int Board::external_get_winner()
	{
		return get_winner();
	}

}