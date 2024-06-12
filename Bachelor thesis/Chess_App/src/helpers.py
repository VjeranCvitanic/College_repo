from src.settings import * 

dict_num_to_char = {
    0 : 'a',
    1 : 'b',
    2 : 'c',
    3 : 'd',
    4 : 'e',
    5 : 'f',
    6 : 'g',
    7 : 'h',
}

dict = {
  'a': 0,
  'b': 1,
  'c': 2,
  'd': 3,
  'e': 4,
  'f': 5,
  'g': 6,
  'h': 7,
}

def board_position_to_screen_position(row, column):
    x = 0
    y = 0

    x = border + column * square_width + 10
    y = height_offset + border + (7 - row) * square_height + 8

    return (x, y)

def screen_position_to_board_position(x, y):
    row = 0
    column = 0

    if y < height_offset:
        row = 8
    elif y > height_offset + 8*square_height + border:
        row = -1
    elif x < border:
        column = -1
    elif x > border + 8 * square_width:
        column = 8
    else:
        column = int((x - border - 10) / square_width)
        row = int(-1 * (((y - height_offset - border - 8) / square_height) - 7) + 1)

    return (row, column)

def board_position_to_center_of_the_square_screen_position(row, column):
    x = 0
    y = 0

    x = border + column * square_width + square_width / 2
    y = height_offset + border + (7 - row) * square_height + square_height / 2

    return (x, y)

def switch_color(color):
    if color == "w":
        return "b"
    else:
        return "w"
    
def is_row_and_column_in_range(row, column):
    if 0 <= row <= 7 and 0 <= column <= 7:
        return True
    return False

def reset_screen_size():
    main_menu_window = pygame.display.set_mode((width, height))

    



