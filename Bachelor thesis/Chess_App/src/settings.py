import pygame
from pygame.locals import *
import sys
from tkinter import messagebox
from time import sleep, time
#import pyautogui

pygame.init()

"""fps = 60
fpsClock = pygame.time.Clock()"""

size_factor = 10

height_offset = 4 * size_factor
border = 3 * size_factor

height = 74 * size_factor
width = height - height_offset

promotion_window_height = 200
promotion_window_width = 600

time_format_screen_width = 400
time_format_screen_height = 300

button_width = 300
button_height = 70

#main_menu_window = pygame.display.set_mode((width, height))
"""screen = pygame.display.set_mode((0, 0), pygame.FULLSCREEN)"""

board_size = height - height_offset - 2 * border

minutes_text_pos = (80, 90)
seconds_text_pos = (210, 90)
increment_text_pos = (210, 160)
classical_text_pos = (120, 230)

time_text_size = (100, 35)

square_width = board_size/8
square_height = board_size/8

width_mini = 5 * square_width + 2 * border
height_mini = 5 * square_height + 2 * border

dark_square_color = (0, 146, 29)
light_square_color = (250, 249, 224)
dark_square_color_mini = (50, 50, 59)
light_square_color_mini = (160, 160, 160)
black = (0, 0, 0)
brown = (139,69,19)
gray = (175, 175, 175)
red = (100, 20, 20)
arrow_color = (199, 115, 29)
white = (255, 255, 255)
bg_color = (200, 220, 200)
button_color = (155, 155, 155)
button_color_hover = (200,200,200)
yellow = (200, 200, 20)

arrow_width = 15

caption = "Chess"
icon_image = "images/knight_logo.png"
icon_surface = pygame.image.load(icon_image)

scale_factor = 60

white_pawn_img = "images/white_pawn.png"
white_pawn_surface = pygame.image.load(white_pawn_img)
white_pawn_surface = pygame.transform.scale(white_pawn_surface, (scale_factor, scale_factor))

black_pawn_img = "images/black_pawn.png"
black_pawn_surface = pygame.image.load(black_pawn_img)
black_pawn_surface = pygame.transform.scale(black_pawn_surface, (scale_factor, scale_factor))

white_bishop_img = "images/white_bishop.png"
white_bishop_surface = pygame.image.load(white_bishop_img)
white_bishop_surface = pygame.transform.scale(white_bishop_surface, (scale_factor, scale_factor))

black_bishop_img = "images/black_bishop.png"
black_bishop_surface = pygame.image.load(black_bishop_img)
black_bishop_surface = pygame.transform.scale(black_bishop_surface, (scale_factor, scale_factor))

white_knight_img = "images/white_knight.png"
white_knight_surface = pygame.image.load(white_knight_img)
white_knight_surface = pygame.transform.scale(white_knight_surface, (scale_factor, scale_factor))

black_knight_img = "images/black_knight.png"
black_knight_surface = pygame.image.load(black_knight_img)
black_knight_surface = pygame.transform.scale(black_knight_surface, (scale_factor, scale_factor))

white_rook_img = "images/white_rook.png"
white_rook_surface = pygame.image.load(white_rook_img)
white_rook_surface = pygame.transform.scale(white_rook_surface, (scale_factor, scale_factor))

black_rook_img = "images/black_rook.png"
black_rook_surface = pygame.image.load(black_rook_img)
black_rook_surface = pygame.transform.scale(black_rook_surface, (scale_factor, scale_factor))

white_queen_img = "images/white_queen.png"
white_queen_surface = pygame.image.load(white_queen_img)
white_queen_surface = pygame.transform.scale(white_queen_surface, (scale_factor, scale_factor))

black_queen_img = "images/black_queen.png"
black_queen_surface = pygame.image.load(black_queen_img)
black_queen_surface = pygame.transform.scale(black_queen_surface, (scale_factor, scale_factor))

white_king_img = "images/white_king.png"
white_king_surface = pygame.image.load(white_king_img)
white_king_surface = pygame.transform.scale(white_king_surface, (scale_factor, scale_factor))

black_king_img = "images/black_king.png"
black_king_surface = pygame.image.load(black_king_img)
black_king_surface = pygame.transform.scale(black_king_surface, (scale_factor, scale_factor))

legal_move_indicator_img = "images/legal_move_indicator.png"
legal_move_indicator_surface = pygame.image.load(legal_move_indicator_img)
legal_move_indicator_surface = pygame.transform.scale(legal_move_indicator_surface, (scale_factor, scale_factor))

square_indicator_img = "images/square_indicator.png"
#square_indicator_surface = pygame.image.load(square_indicator_img).convert_alpha()
#square_indicator_surface = pygame.transform.scale(square_indicator_surface, (scale_factor, scale_factor))

move_sound_path = 'sound_effects/move.wav'
start_game_sound_path = 'sound_effects/start_game.wav'
castle_sound_path = 'sound_effects/castle.wav'
capture_sound_effect = 'sound_effects/capture.wav'
check_sound_effect = 'sound_effects/check.wav'
checkmate_sound_effect = 'sound_effects/checkmate.wav'
draw_sound_effect = 'sound_effects/draw.wav'
stalemate_sound_effect = 'sound_effects/stalemate.wav'

myfont = pygame.font.SysFont("monospace", 20)
myfont2 = pygame.font.SysFont("monospace", 50)
myfont3 = pygame.font.SysFont("monospace", 60)
myfont4 = pygame.font.SysFont("arial", 30)

main_text = myfont3.render("Welcome to chess!", 30, black, white)
chess_game_text = myfont2.render("Chess", 10, black, white)
mini_chess_game_text = myfont2.render("MiniChess", 10, black, white)
chess_game_vs_bot_text = myfont2.render("vs Bot", 10, black, white)
chess_game_bot_vs_bot_text = myfont2.render("Bot vs Bot", 10, black, white)

rect_1_Pos = (width/2 - button_width/2, 200)
rect_2_Pos = (width/2 - button_width/2, 300)
rect_3_Pos = (width/2 - button_width/2, 400)
rect_4_Pos = (width/2 - button_width/2, 500)

minutes_text = myfont.render("min:", True, black, white)
seconds_text = myfont.render("s:", True, black, white)
increment_text = myfont.render("increment(in s):", True, black, white)
classical_text = myfont.render("No time limit", True, black, white)


INF = 100000

DEPTH_MINMAX = 2
DEPTH_AB = 2

PAWN_VALUE = 100
KNIGHT_VALUE = 300
BISHOP_VALUE = 315
ROOK_VALUE = 500
QUEEN_VALUE = 900
KING_VALUE = 0 #not needed to set, as both players will always have a king a piece

PIECES_START_VALUE = 39 * 100 #value of all pieces at start
