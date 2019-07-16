// BasicAgent.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>
#include "BasicAgent.h"

int main()
{

	int my_id;//A number indicating which tokens correspond to you, 0 for gold pieces and 1 for white pieces.
	int number_of_games;//A number indicating how many games will be played on this match.

	std::cin >> my_id >> number_of_games;

	int enemy = 1 - my_id;

	float KILL_HELIOS = 5.0;
	float MY_HELIOS_DANGER = -4.0;
	float ENEMY_HELIOS_DANGER = 0.05;
	float MY_THREAT = 2.5;
	float ENEMY_THREAT = -2.5;
	float KILL_PIECE = 2.5;
	BasicAgent::Agent agent;
	agent.set_coeffs({ KILL_HELIOS,MY_HELIOS_DANGER,ENEMY_HELIOS_DANGER,MY_THREAT,ENEMY_THREAT,KILL_PIECE });

	while (number_of_games > 0) {

		int last_winner;//A number indicating who won last game,the value will be:
						//-1 for first turn
						//0 if gold player won
						//1 if white player won
						//2 if it was a draw

		std::cin >> last_winner;

		while (true) {

			int token_count;//A number indicating how many pieces are on the board in this turn.
			std::cin >> token_count;

			if (token_count == -1)
				break;

			Board board;

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

				board.add_token(type, orientation, Point(x, y), player);
			}

			Action best_action = agent.find_best_action(board, my_id, enemy);

			std::cout << best_action << std::endl;
		}

		number_of_games -= 1;
	}
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
