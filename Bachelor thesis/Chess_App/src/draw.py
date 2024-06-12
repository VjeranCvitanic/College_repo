from src.settings import *
from src.helpers import *
from src.piece import piece

def draw_notations(screen):
    for i in range(8):
        label = myfont.render("%d" % (i + 1), 1, white)
        screen.blit(label, (border / 2 - 5, (7 - i + 1/3) * square_height + height_offset + border))
    
    file_labels = []
    file_labels.append(myfont.render('A', 1, white))
    file_labels.append(myfont.render('B', 1, white))
    file_labels.append(myfont.render('C', 1, white))
    file_labels.append(myfont.render('D', 1, white))
    file_labels.append(myfont.render('E', 1, white))
    file_labels.append(myfont.render('F', 1, white))
    file_labels.append(myfont.render('G', 1, white))
    file_labels.append(myfont.render('H', 1, white))

    for i in range(8):
        label = file_labels[i]
        screen.blit(label, (i * square_width + border + 35, height - (border/2 + 12)))

def draw_board(screen):
    square_color_flag = False
    for i in range(8):
        for j in range(8):
            if square_color_flag == True:
                square_color = dark_square_color
            else:
                square_color = light_square_color
            pygame.draw.rect(screen, square_color, pygame.Rect(j * square_width + border, i * square_height + height_offset + border, square_width, square_height), 0)
            square_color_flag = not square_color_flag
        square_color_flag = not square_color_flag

def draw_toolbar(screen):
    #pygame.draw.rect(screen, gray, pygame.Rect(0, 0, width, height_offset))
    pygame.draw.rect(screen, white, pygame.Rect(0, 0, width/2, height_offset))
    pygame.draw.rect(screen, black, pygame.Rect(width/2, 0, width/2, height_offset))

def draw_pieces(screen, board):
    if board.white_state.king.is_in_check and board.white_to_play:
        draw_check_indicator(screen, board.white_state.king.row, board.white_state.king.column)
    elif board.black_state.king.is_in_check and not board.white_to_play:
        draw_check_indicator(screen, board.black_state.king.row, board.black_state.king.column)
    for row in board.array:
        for piece in row:
            if piece != 0:
                screen.blit(piece.image, board_position_to_screen_position(piece.row, piece.column))

def draw_legal_moves(screen, piece):
    for position in piece.array_of_legal_moves:
        screen.blit(legal_move_indicator_surface, board_position_to_screen_position(position[0], position[1]))

def draw_arrow(screen, right_click_start_position, right_click_end_position):
    start_pos = screen_position_to_board_position(right_click_start_position[0], right_click_start_position[1])
    end_pos = screen_position_to_board_position(right_click_end_position[0], right_click_end_position[1])
    if is_row_and_column_in_range(start_pos[0], start_pos[1]) and is_row_and_column_in_range(end_pos[0], end_pos[1]):
        if start_pos == end_pos:
            screen.blit(square_indicator_surface, board_position_to_screen_position(start_pos[0], start_pos[1]))
        else:
            start_pos = board_position_to_center_of_the_square_screen_position(start_pos[0], start_pos[1])
            end_pos = board_position_to_center_of_the_square_screen_position(end_pos[0], end_pos[1])
            pygame.draw.line(screen, arrow_color, start_pos, end_pos, arrow_width)
            """Arrow triangle missing"""
    
def draw_window_for_promoting_piece_choice():
    screen = pygame.display.set_mode((570, 150))
    screen.fill(bg_color)

    img_height = 50

    screen.blit(white_queen_surface, (100, img_height))
    screen.blit(white_rook_surface, (200, img_height))
    screen.blit(white_bishop_surface, (300, img_height))
    screen.blit(white_knight_surface, (400, img_height))

    while True:
        for event in pygame.event.get():
            if event.type == QUIT:
                pass
            if event.type == pygame.MOUSEBUTTONUP:
                if event.button == 1:
                    mouse_pos = pygame.mouse.get_pos()
                    if 50 < mouse_pos[1] < 110:
                        if 100 < mouse_pos[0] < 160:
                            return "q"
                        elif 200 < mouse_pos[0] < 260:
                            return "r"
                        elif 300 < mouse_pos[0] < 360:
                            return "b"
                        elif 400 < mouse_pos[0] < 460:
                            return "n"
        
        pygame.display.flip()
    
def draw_check_indicator(screen, row, column):
    row = 7 - row
    pygame.draw.rect(screen, red, pygame.Rect(column * square_width + border, row * square_height + height_offset + border, square_width, square_height), 0)

def draw(screen, board):
    screen.fill(brown)
    draw_notations(screen)
    draw_board(screen)
    draw_toolbar(screen)
    draw_pieces(screen, board)

def draw_piece_value(screen, white_pieces_value, black_pieces_value):
    white_value_text = myfont4.render("%d" % (white_pieces_value / 100), True, black)
    black_value_text = myfont4.render("%d" % (black_pieces_value / 100), True, white)

    screen.blit(white_value_text, (width/2 - 32, height_offset/6))
    screen.blit(black_value_text, (width/2 + 5, height_offset/6))