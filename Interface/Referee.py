
from GraphicalBoard import*
from LogicalBoard import*

class Referee:
    def __init__(self,player1,player2,max_turns,show):
       self.player1 = player1
       self.player2 = player2
       self.logical_board = LogicalBoard()
       self.history_board = []
       self.graphical_board = GraphicalBoard(self.logical_board,self.history_board)
       self.max_turns = max_turns
       self.show = show

    def get_bot_move(self,player_bot,player_turn):
        info = self.logical_board.get_info()
        player_bot.send(str(len(info))) 
        for t in info:
            player_bot.send(str(t))
            #print(str(t))
        move = player_bot.receive(str).split()

        #print("------ " + str(player_turn) + " " + str(move[0]) + " " + str(move[1]) + " " + str(move[2]) + " " + str(move[3]) + " " + str(move[4]) + " ------")
        
        if len(move) == 0:
            return True

        self.graphical_board.do_bot_move(player_turn,Point(int(move[0]),int(move[1])),Point(int(move[2]),int(move[3])),int(move[4]),self.show)
     
        return False

    def play_game(self,first_player):

        self.history_board.clear()

        self.logical_board.reset()
     
        player_turn = first_player
        
        if self.show == 1:
            self.graphical_board.draw(first_player)
         
        invalid_action = False

        turns = 0

        while not self.graphical_board.terminated and not invalid_action and not self.logical_board.game_finished() and turns < self.max_turns:
            
            invalid_action = False
            
            self.history_board.append(copy.deepcopy(self.logical_board))

            if player_turn == Player.PLAYER0:
                if self.player1.human == True:
                    self.graphical_board.get_human_move(Player.PLAYER0)
                else:
                    invalid_action = self.get_bot_move(self.player1,Player.PLAYER0)

                player_turn = Player.PLAYER1
            else:
                if self.player2.human == True:
                    self.graphical_board.get_human_move(Player.PLAYER1)
                else:
                    invalid_action = self.get_bot_move(self.player2,Player.PLAYER1)

                player_turn = Player.PLAYER0

            if invalid_action == True:
                player_turn = Player.PLAYER0 if player_turn == Player.PLAYER1 else Player.PLAYER1
            
            turns = len(self.history_board)

        if not self.player1.human:
            self.player1.send("-1")
        if not self.player2.human:
            self.player2.send("-1")

        if turns == self.max_turns:
            return (DRAW,invalid_action)

        return (WIN_GOLD,invalid_action) if player_turn == Player.PLAYER1 else (WIN_WHITE,invalid_action)

    def play_games(self,games,first_turn_str):
         
        first_turn = Player.PLAYER0 if first_turn_str == GOLD else Player.PLAYER1

        if not self.player1.human:
            self.player1.send("0 " + str(games))

        if not self.player2.human:
            self.player2.send("1 " + str(games))

        last_winner = -1

        wins_player1 = 0
        wins_player2 = 0
        draws = 0

        while games > 0 and self.graphical_board.terminated == False:

            if not self.player1.human:
                self.player1.send(str(last_winner))
            if not self.player2.human:
                self.player2.send(str(last_winner))

            (last_winner,invalid_action) = self.play_game(first_turn)

            first_turn = (first_turn+1)%2

            games-=1

            if last_winner == DRAW:
                draws+=1
                print("Max amount of turns reached, it is a draw!")
            elif last_winner == WIN_GOLD:
                wins_player1+=1
                print(self.player1.id + " player won!")
            elif last_winner == WIN_WHITE:
                wins_player2+=1
                print(self.player2.id + " player won!")
        
            print("Wins for " + GOLD + " player: " + str(wins_player1))
            print("Wins for " + WHITE + " player: " + str(wins_player2))

            if invalid_action == True:
                if last_winner == DRAW:
                    print("Max amount of turns reached, it is a draw!")
                elif last_winner == WIN_GOLD:
                    wins_player1 += games
                    print(WHITE + " player timeouted!")
                elif last_winner == WIN_WHITE:
                    wins_player2 += games
                    print(GOLD + " player timeouted!")
        
                self.player1.finish()
                self.player2.finish()
                games = 0
            
            if self.graphical_board.terminated == True:
                self.player1.finish()
                self.player2.finish()


                
        print("Final results:")
        print("Wins for " + GOLD + " player: " + str(wins_player1))
        print("Wins for " + WHITE + " player: " + str(wins_player2))
        print("Draws: " + str(draws))
        if wins_player1 > wins_player2:        
            print(self.player1.id + " player wins the match!")
        elif wins_player1 < wins_player2:        
            print(self.player2.id + " player wins the match!")
        else:
            print("None won the match,it is a draw!")

    def state_info(self,info : list) -> str:
        state_str = ""
        for t in info:
            state_str += str(t) + " "
        state_str = state_str[:-1]
        return state_str
