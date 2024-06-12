from src.draw import draw_window_for_promoting_piece_choice, draw_board, draw_notations
from src.piece import *

def pick_promotion_piece(color, row, column):
    piece_type = draw_window_for_promoting_piece_choice()
    screen = pygame.display.set_mode((width, height))

    if piece_type == "q":
        Queen = queen(color, row, column)
        return Queen
    elif piece_type == "b":
        Bishop = bishop(color, row, column)
        return Bishop
    elif piece_type == "r":
        Rook = rook(color, row, column)
        return Rook
    elif piece_type == "n":
        Knight = knight(color, row, column)
        return Knight