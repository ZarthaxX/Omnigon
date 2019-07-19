from Constants import*
from Utilities import*

import copy

class Token:
    def __init__(self, type: TokenType,orientation: int,owner : Player):
        self.type = type
        self.orientation = orientation
        self.owner = owner
        
    rotations = {
        (TokenType.HELIOS,0) : 0,
        (TokenType.BIGON,0) : 1,
        (TokenType.BIGON,1) : 2,
        (TokenType.BIGON,2) : 3,
        (TokenType.BIGON,3) : 4,
        (TokenType.BIGON,4) : 5,
        (TokenType.BIGON,5) : 0,
        (TokenType.TRIGON,0) : 1,
        (TokenType.TRIGON,1) : 2,
        (TokenType.TRIGON,2) : 3,
        (TokenType.TRIGON,3) : 4,
        (TokenType.TRIGON,4) : 5,
        (TokenType.TRIGON,5) : 0,
        (TokenType.STARGON,0) : 1,
        (TokenType.STARGON,1) : 0
    }

    moves = {
        (TokenType.HELIOS,0) : [1,1,1,1,1,1],
        (TokenType.BIGON,0) : [1,0,0,0,0,1],
        (TokenType.BIGON,1) : [1,1,0,0,0,0],
        (TokenType.BIGON,2) : [0,1,1,0,0,0],
        (TokenType.BIGON,3) : [0,0,1,1,0,0],
        (TokenType.BIGON,4) : [0,0,0,1,1,0],
        (TokenType.BIGON,5) : [0,0,0,0,1,1],
        (TokenType.TRIGON,0) : [1,1,0,0,0,1],
        (TokenType.TRIGON,1) : [1,1,1,0,0,0],
        (TokenType.TRIGON,2) : [0,1,1,1,0,0],
        (TokenType.TRIGON,3) : [0,0,1,1,1,0],
        (TokenType.TRIGON,4) : [0,0,0,1,1,1],
        (TokenType.TRIGON,5) : [1,0,0,0,1,1],
        (TokenType.STARGON,0) : [0,1,0,1,0,1],
        (TokenType.STARGON,1) : [1,0,1,0,1,0]
    }
    
    directions = [Point(0,-1),Point(1,0),Point(1,1),Point(0,1),Point(-1,0),Point(-1,-1)]

class CellInfo:
    def __init__(self, token : Token,location : Point):
        self.token = token
        self.location = location

    def __str__(self):
        return (str(self.token.owner) + " " +
                str(token_type_str[self.token.type]) + " " + 
                str(self.location.x) + " " +
                str(self.location.y) + " " +
                str(self.token.orientation))

class LogicalBoard:

    def __init__(self):
        self.grid = [[Token(-1,-1,-1) for x in range(7)] for y in range(7)] 
        self.player_tokens = []
        self.game_done = False

    def reset(self):
        self.grid = [[Token(-1,-1,-1) for x in range(7)] for y in range(7)] 
        self.player_tokens = []
        self.game_done = False

        self.add_token(TokenType.HELIOS,0,Point(1,3),0)
        self.add_token(TokenType.HELIOS,0,Point(5,3),1)
        self.add_token(TokenType.TRIGON,1,Point(0,3),0)
        self.add_token(TokenType.TRIGON,4,Point(6,3),1)
        self.add_token(TokenType.STARGON,1,Point(0,2),0)
        self.add_token(TokenType.STARGON,1,Point(2,3),0)
        self.add_token(TokenType.STARGON,1,Point(0,4),0)
        self.add_token(TokenType.STARGON,0,Point(5,2),1)
        self.add_token(TokenType.STARGON,0,Point(4,3),1)
        self.add_token(TokenType.STARGON,0,Point(5,4),1)
        self.add_token(TokenType.BIGON,0,Point(1,2),0)
        self.add_token(TokenType.BIGON,3,Point(1,4),0)
        self.add_token(TokenType.BIGON,0,Point(4,2),1)
        self.add_token(TokenType.BIGON,3,Point(4,4),1)
    

    def external_point(self,p : Point) :
        point = copy.deepcopy(p)
        if point.y > 3:
            point.x -= (point.y - 3)
        return point
    
    def internal_point(self,p : Point) :
        point = copy.deepcopy(p)
        if point.y > 3:
            point.x += (point.y - 3)
        return point
    
    def add_token(self,token_type : TokenType,token_orientation : int,location : Point,owner : int) -> None:
        self.player_tokens.append(CellInfo(Token(token_type,token_orientation,owner),location))
        location = self.internal_point(location)
        self.grid[location.y][location.x] = Token(token_type,token_orientation,owner)
        
    def get_info(self) -> list:
        return self.player_tokens

    def __str__(self) -> str:
        board_str = ""
        for y in range (7):
            for x in range (7):
                if self.grid[y][x].type != -1:
                    board_str += token_str[self.grid[y][x].type] + "-" + str(self.grid[y][x].orientation) + " "
                else:
                    board_str += "--- "
            board_str += "\n"
        return board_str
    
    def valid_cell(self,location : Point) -> bool:
        return self.valid_cell_internal(self.internal_point(location))

    def valid_cell_internal(self,location : Point) -> bool:
        if location.y == 0 and 0 <= location.x <= 3:
            return True
        elif location.y == 1 and 0 <= location.x <= 4:
            return True
        elif location.y == 2 and 0 <= location.x <= 5:
            return True
        elif location.y == 3 and 0 <= location.x <= 6:
            return True
        elif location.y == 4 and 1 <= location.x <= 6:
            return True
        elif location.y == 5 and 2 <= location.x <= 6:
            return True
        elif location.y == 6 and 3 <= location.x <= 6:
            return True
        else:
            return False
    
    def get_token_info(self,location : Point):
        for cell_info in self.player_tokens:
            if cell_info.location == location:
                return cell_info

        return CellInfo(Token(-1,-1),Point(-1,-1))

    def empty_cell(self,location : Point):  
        for cell_info in self.player_tokens:
            if cell_info.location == location:
                return False

        return True
    
    def valid_moves(self,_location : Point):
        location = self.internal_point(_location)
        return self.valid_moves_internal(location)

    def valid_moves_internal(self,location : Point):
        moves = list()
        token = self.grid[location.y][location.x]

        for i in range(6):
            has_arrow = Token.moves[(token.type,token.orientation)][i]
            direction = Token.directions[i]
            if has_arrow:
                position = copy.copy(direction)
                position.x += location.x
                position.y += location.y
                while self.valid_cell_internal(position):
                    if self.grid[position.y][position.x].owner != -1:
                        if self.grid[position.y][position.x].owner != token.owner:
                            enemy = self.grid[position.y][position.x]
                            if enemy.type == TokenType.HELIOS or Token.moves[(enemy.type,enemy.orientation)][(i+3)%6] == False:
                                moves.append(self.external_point(position))
                        break
                    else:
                        moves.append(self.external_point(position))

                    position.x += Token.directions[i].x
                    position.y += Token.directions[i].y

                    if token.type == TokenType.HELIOS:
                        break
        return moves

    def do_action(self,_from : Point,_to : Point,orientation : int):
        location = self.internal_point(_from)
        destination = self.internal_point(_to)

        token = copy.deepcopy(self.grid[location.y][location.x])
        self.grid[location.y][location.x] = Token(-1,-1,-1)

        if self.grid[destination.y][destination.x].type == TokenType.HELIOS:
            self.game_done = True

        self.grid[destination.y][destination.x] = copy.copy(token)
        token = self.grid[location.y][location.x]

        self.grid[destination.y][destination.x].orientation = orientation

        self.player_tokens = list()
        for y in range(7):
            for x in range(7):
                if self.grid[y][x].owner != -1:
                    self.player_tokens.append(CellInfo(copy.copy(self.grid[y][x]),self.external_point(Point(x,y))))
    

    def move_token(self,_location : Point,_destination : Point):
        location = self.internal_point(_location)
        destination = self.internal_point(_destination)
        self.move_token_internal(location,destination)
    
    def move_token_internal(self,location : Point,destination : Point):
        token = copy.deepcopy(self.grid[location.y][location.x])
        self.grid[location.y][location.x] = Token(-1,-1,-1)

        if self.grid[destination.y][destination.x].type == TokenType.HELIOS:
            self.game_done = True

        self.grid[destination.y][destination.x] = copy.copy(token)

        self.player_tokens = list()
        for y in range(7):
            for x in range(7):
                if self.grid[y][x].owner != -1:
                    self.player_tokens.append(CellInfo(copy.copy(self.grid[y][x]),self.external_point(Point(x,y))))
        
        return

    def rotate_token(self,_location : Point,orientation : int):
        location = self.internal_point(_location)
        self.rotate_token_internal(location,orientation)

    def rotate_token_internal(self,location : Point,orientation : int):
        token = self.grid[location.y][location.x]
        self.grid[location.y][location.x].orientation = orientation

        self.player_tokens = list()
        for y in range(7):
            for x in range(7):
                if self.grid[y][x].owner != -1:
                    self.player_tokens.append(CellInfo(copy.copy(self.grid[y][x]),self.external_point(Point(x,y))))
    
    def game_finished(self) -> bool:
        return self.game_done
