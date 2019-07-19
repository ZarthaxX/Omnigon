import pygame

class Resources:

    location = "Resources/"
    file_type = ".png"
    images = {
        "A_helios_0",
        "B_helios_0",
        "A_stargon_0",
        "A_stargon_1",
        "B_stargon_0",
        "B_stargon_1",
        "A_trigon_0",
        "A_trigon_1",
        "A_trigon_2",
        "A_trigon_3",
        "A_trigon_4",
        "A_trigon_5",
        "B_trigon_0",
        "B_trigon_1",
        "B_trigon_2",
        "B_trigon_3",
        "B_trigon_4",
        "B_trigon_5",
        "A_bigon_0",
        "A_bigon_1",
        "A_bigon_2",
        "A_bigon_3",
        "A_bigon_4",
        "A_bigon_5",
        "B_bigon_0",
        "B_bigon_1",
        "B_bigon_2",
        "B_bigon_3",
        "B_bigon_4",
        "B_bigon_5",
    }

    token_width = 136
    token_height = 120

    board = "board"
    moveable_cell = "moveable_cell"
    selected_token = "selected_token"
    
class TokenType:
    HELIOS = 0
    BIGON = 1
    TRIGON = 2
    STARGON = 3

class Player:
    PLAYER0 = 0
    PLAYER1 = 1

token_str = {TokenType.HELIOS : "H",TokenType.BIGON : "B",TokenType.TRIGON: "T",TokenType.STARGON : "S"}
token_img = {TokenType.HELIOS : "helios",TokenType.BIGON : "bigon",TokenType.TRIGON: "trigon",TokenType.STARGON : "stargon"}
token_player = { Player.PLAYER0 : "A",Player.PLAYER1 : "B"}

token_type_str = {
    TokenType.HELIOS : "HELIOS",
    TokenType.BIGON : "BIGON",
    TokenType.TRIGON: "TRIGON",
    TokenType.STARGON : "STARGON"
}

WIN_GOLD = 0
WIN_WHITE = 1
DRAW = 2

GOLD = 'gold'
WHITE = 'white'

BOARD_WIDTH_OFFSET = 300
BOARD_HEIGHT_OFFSET = 20

NORMAL_FPS = 60
BOT_FPS = 1

TIME_LIMIT = 10.5 #time limit for bots,in seconds