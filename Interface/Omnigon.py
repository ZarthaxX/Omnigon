from Referee import*
from HumanPlayer import*
from PlayerCommunicator import*
from Constants import*

import argparse

def main(args):
    #os.system(f"echo '{code}' | gcc -o bincode -xc - ; ./bincode") <-- Zenoscave's code to call a c++ program from python
    
    if args.gold_player is None:
        player1 = HumanPlayer(GOLD)
    else:
        player1 = PlayerCommunicator(args.gold_player,GOLD)
    
    if args.white_player is None:
        player2 = HumanPlayer(WHITE)
    else:
        player2 = PlayerCommunicator(args.white_player,WHITE)

    games = args.games

    first_turn = args.first
    
    max_turns = args.max_turns

    referee = Referee(player1,player2,max_turns,args.show)

    referee.play_games(games,first_turn)

if __name__== "__main__":
    description = 'Omnigon board game,designed by ZarthaxX'
    parser = argparse.ArgumentParser(description=description)
    parser.add_argument('--gold_player',
                        type=str,
                        nargs='*',
                        help='Path for gold_player\'s bot executable.')
    parser.add_argument('--white_player',
                        type=str,
                        nargs='*',
                        help='Path for white_player\'s bot executable.')
    parser.add_argument('--games',
                        type=int,
                        default=1,
                        help="The amount of games that are going to be played between these 2 players.")
    parser.add_argument('--max_turns',
                        type=int,
                        default=50,
                        help="Maximum number of turns that will be played between each game.")
    parser.add_argument('--first',
                        type=str,
                        choices=[GOLD, WHITE],
                        help='Color of the first player for the initial game.')
    parser.add_argument('--show',
                        type=int,
                        default=1,
                        help="Shows how the match is being played if the value is True,and does not otherwise.")
    args = parser.parse_args()

    main(args)

