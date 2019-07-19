from LogicalBoard import*
import sys
import pygame

class GraphicalBoard:
    
    x_offset = [102,34]
    y_offset = 60

    def __init__(self,logical_board,history_board): 
        pygame.init()
        self.screen = pygame.display.set_mode((1400,1000))
        self.clock = pygame.time.Clock()
        pygame.font.init()
        self.font = pygame.font.SysFont('Comic Sans MS', 30)

        self.board_loc = [BOARD_WIDTH_OFFSET,BOARD_HEIGHT_OFFSET]

        self.moveable_cell_img = pygame.image.load(Resources.location + Resources.moveable_cell + Resources.file_type).convert()
        transColor = self.moveable_cell_img.get_at((0,0))
        self.moveable_cell_img.set_colorkey(transColor)
        
        self.selected_token_img = pygame.image.load(Resources.location + Resources.selected_token + Resources.file_type).convert()
        transColor = self.selected_token_img.get_at((0,0))
        self.selected_token_img.set_colorkey(transColor)

        self.board_image = pygame.image.load(Resources.location + Resources.board + Resources.file_type).convert()
        transColor = self.board_image.get_at((0,0))
        self.board_image.set_colorkey(transColor)

        self.images = dict()

        for img in Resources.images:
            self.images[img] = pygame.image.load(Resources.location + img + Resources.file_type).convert()
            transColor = self.images[img].get_at((0,0))
            self.images[img].set_colorkey(transColor)

        self.logical_board = logical_board
        self.history_board = history_board
        
        self.FPS = NORMAL_FPS

        self.terminated = False

    def to_visual_point(self,p : Point) -> (int,int):
        point = copy.copy(p)
        if point.y < 2 or point.y > 4 :
            point.x += 1
        
        y = point.y
        x = point.x
        x = x*Resources.token_width+self.x_offset[y%2]
        x = x + self.board_loc[0]
        y = y*Resources.token_height+self.y_offset+self.board_loc[1]

        return (x,y)

    def draw(self,player_turn : int,selected_token : Point = Point(-1,-1),available_moves : list() = list()) -> None:
        
        WHITE = (255, 255, 255)
        BLUE=(0,0,255)
        self.screen.fill(WHITE)
        #pygame.draw.rect(self.screen,WHITE,(self.board_loc[0],self.board_loc[1],2000,2000))
        self.screen.blit(self.board_image,self.board_loc)

        objects = self.logical_board.get_info()

        for info in objects:
            img_key = token_player[info.token.owner] + "_" + token_img[info.token.type] + "_" + str(info.token.orientation)
            
            point = self.to_visual_point(info.location)

            self.screen.blit(self.images[img_key], point)

        if selected_token.x != -1 and selected_token.y != -1:

            point = self.to_visual_point(selected_token)

            self.screen.blit(self.selected_token_img,point)
            
        for move in available_moves:
            point = self.to_visual_point(move)

            self.screen.blit(self.moveable_cell_img,point)

        textsurface = self.font.render("Player " + str(player_turn) + " playing...", False, (0, 0, 0))
        self.screen.blit(textsurface,(20,475))
        pygame.display.update()
        self.clock.tick(self.FPS)

        for event in pygame.event.get():
            if event.type == pygame.MOUSEBUTTONUP:
                None 
    
    def get_human_move(self,player_turn):

        self.FPS = NORMAL_FPS

        done = False
        while done == False and self.terminated == False:
            self.logical_board.grid = copy.deepcopy(self.history_board[-1].grid)
            self.logical_board.player_tokens = copy.deepcopy(self.history_board[-1].player_tokens)
            self.logical_board.game_done = copy.deepcopy(self.history_board[-1].game_done)
            self.draw(player_turn)
            moved_token = self.selecting_token(player_turn)

            if not self.terminated:    
                done = self.rotating_token(player_turn,moved_token)
        

    def decode_mouse(self) -> Point:
        y = int((pygame.mouse.get_pos()[1]-self.y_offset - self.board_loc[1]) / Resources.token_height)
        x = int((pygame.mouse.get_pos()[0]-self.x_offset[y%2] - self.board_loc[0]) / Resources.token_width)
   
        if y < 2 or y > 4:
            x -= 1
        return Point(x,y)

    def do_bot_move(self,player_turn : int,fromP : Point,toP : Point, orientation : int,show : bool):
        
        self.FPS = BOT_FPS

        if show == 1:
            self.draw(player_turn)

        selected_token = copy.copy(fromP)
        available_moves = self.logical_board.valid_moves(selected_token)
        if show == 1:
            self.draw(player_turn,selected_token,available_moves)

        destination = copy.copy(toP)
        self.logical_board.move_token(selected_token,destination)
        if show == 1:
            self.draw(player_turn,destination)

        self.logical_board.rotate_token(toP,orientation)
        if show == 1:
            self.draw(player_turn)
        
        
    def selecting_token(self,player_turn : int) -> Point:
    
        available_moves = list()
        selected_token = Point(-1,-1)

        token_moved = Point(-1,-1)

        while not self.terminated and token_moved.x == -1 and token_moved.y == -1:
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    self.exit()
                    return token_moved
                elif event.type == pygame.KEYUP and event.key == pygame.K_z and len(self.history_board) > 2:
                    self.history_board.pop()
                    self.history_board.pop()
                    self.logical_board.grid = copy.deepcopy(self.history_board[-1].grid)
                    self.logical_board.player_tokens = copy.deepcopy(self.history_board[-1].player_tokens)
                    self.logical_board.game_done = copy.deepcopy(self.history_board[-1].game_done)
                    self.draw(player_turn)
                elif event.type == pygame.MOUSEBUTTONUP:
                    selected = self.decode_mouse()
                    if self.logical_board.valid_cell(selected):
                        if not self.logical_board.empty_cell(selected):
                            token_info = self.logical_board.get_token_info(selected)

                            if token_info.token.owner == player_turn:
                                selected_token = token_info.location
                                available_moves = self.logical_board.valid_moves(token_info.location)

                        for move in available_moves:
                            if move == selected:
                                self.logical_board.move_token(selected_token,move)
                                available_moves = list()
                                #self.move_token(player_turn,selected_token,move)
                                selected_token = move
                                token_moved = move
                                break

                    self.draw(player_turn,selected_token,available_moves)

        return token_moved

    def rotating_token(self,player_turn : int,selected_token : Point):
 
        rotating_done = False
        while not self.terminated and not rotating_done:
            LEFT = 3
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    self.exit()
                    return True
                elif event.type == pygame.KEYUP and event.key == pygame.K_z:
                    return False
                elif event.type == pygame.MOUSEBUTTONUP and event.button == LEFT:
                    rotating_done = True
                    self.draw(player_turn)
                    break
                elif event.type == pygame.MOUSEBUTTONUP:
                    selected = self.decode_mouse()

                    if selected == selected_token:
                        token_info = self.logical_board.get_token_info(selected_token)
                        next_orientation = Token.rotations[(token_info.token.type,token_info.token.orientation)]
                        self.logical_board.rotate_token(token_info.location,next_orientation)

                    self.draw(player_turn,selected_token)


        return True

    def exit(self):
        pygame.quit()
        self.terminated = True