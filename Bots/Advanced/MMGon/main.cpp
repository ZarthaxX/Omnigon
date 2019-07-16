#include <iostream>
#include <fstream>

#include "MinMax.h"

using namespace MinMaxGon;

int main()
{
	/*
	Board t(50, 0);
	t.external_init();
	std::cerr << t << std::endl;
	std::vector<Action> a(255,Action());
	std::cerr << t.Get_moves(a) << std::endl;
	*/
	Agent agent;

	int my_id;//A number indicating which tokens correspond to you, 0 for gold pieces and 1 for white pieces.
	int number_of_games;//A number indicating how many games will be played on this match.

	std::cin >> my_id >> number_of_games;

	int enemy = 1 - my_id;

	while (number_of_games > 0) {

		int last_winner;//A number indicating who won last game,the value will be:
						//-1 for first turn
						//0 if gold player won
						//1 if white player won
						//2 if it was a draw

		std::cin >> last_winner;

		int turns_left = 50;
		while (true) {
			int token_count;//A number indicating how many pieces are on the board in this turn.
			std::cin >> token_count;

			if (token_count == -1)
				break;

			Board board(turns_left,my_id);

			for (int i = 0; i < token_count; i++) {
				// Coordinates system for Omnigon board
				//		   (0,0) (1,0) (2,0) (3,0)
				//		(0,1) (1,1) (2,1) (3,1) (4,1)
				//	 (0,2) (1,2) (2,2) (3,2) (4,2) (5,2)
				//(0,3) (1,3) (2,3) (3,3) (4,3) (5,3) (6,3)
				//	 (0,4) (1,4) (2,4) (3,4) (4,4) (5,4)
				//		(0,5) (1,5) (2,5) (3,5) (4,5)
				//		   (0,6) (1,6) (2,6) (3,6)
				int player;//A number indicting who is the owner of this piece,it can either be 0 or 1.
				std::string type;//A string indicating what type of pieces it is,the values can be "HELIOS","BIGON","TRIGON" and "STARGON".
				int x;//A number indicating the x coordinate of the piece, refer to the board image above to understand coordinates better.
				int y;//A number indicating the y coordinate of the piece, refer to the board image above to understand coordinates better.
				int orientation;//A number indicating the orientation of the piece, refer to the pieces images to understand how many orientations each piece has.
				std::cin >> player >> type >> x >> y >> orientation;

				int _type;
				if (type == "HELIOS")_type = HELIOS;
				if (type == "TRIGON")_type = TRIGON;
				if (type == "STARGON")_type = STARGON;
				if (type == "BIGON")_type = BIGON;

				board.Add_token(_type, x, y, orientation, player);
			}

			Action best_action = agent.find_best_action(board, my_id, enemy);

			std::cout << 
				(int)best_action.from.x << " " << (int)best_action.from.y << " " <<
				(int)best_action.to.x << " " << (int)best_action.to.y << " " <<
				(int)best_action.final_orientation << std::endl;

			turns_left--;

		}

		number_of_games -= 1;
	}
}