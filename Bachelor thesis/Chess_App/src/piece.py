from src.settings import *
from src.helpers import is_row_and_column_in_range
from src.move import move

class piece:
    def __init__(self, color = 0, row = 0, column = 0):
        self.color = color
        self.type = 0
        self.row = row
        self.column = column
        self.image = 0
        self.image_white = 0
        self.image_black = 0
        self.array_of_legal_moves = []
    
    def calculate_pseudo_legal_moves(self, board):
        pass

    def calculate_legal_moves(self, board):
        if self.type != "k":
            help_array = []
            self.calculate_pseudo_legal_moves(board)
            for move in self.array_of_legal_moves:
                if board.is_move_valid(move[0], move[1], self):
                    help_array.append(move)
            
            self.array_of_legal_moves = help_array
        else:
            help_array = []
            self.calculate_pseudo_legal_moves(board)
            for move in self.array_of_legal_moves:
                if abs(self.column - move[1]) > 1:
                    if self.column > move[1]:
                        if board.is_move_valid(move[0], move[1], self) and board.is_move_valid(move[0], move[1] + 1, self):
                            help_array.append(move)
                    else:
                        if board.is_move_valid(move[0], move[1], self) and board.is_move_valid(move[0], move[1] - 1, self):
                            help_array.append(move)
                else:
                    if board.is_move_valid(move[0], move[1], self): 
                        help_array.append(move)
            
            self.array_of_legal_moves = help_array

    def reset_array_of_legal_moves(self):
        self.array_of_legal_moves = []
    
    def set_image(self):
        if self.color == "w":
            self.image = self.image_white
        
        else:
            self.image = self.image_black

    def sort_legal_moves(self, board):
        captures = []
        checks = []
        other_moves = []
        sorted_list = []
        
        for move_ in self.array_of_legal_moves:
            move_object = move(self.row, self.column, move_[0], move_[1], 0, 0, 0)
            if board.is_move_capture(move_object):
                captures.append(move_)
            elif board.is_move_check(move_, self):
                checks.append(move_)
            else:
                other_moves.append(move_)

        for capture in captures:
            sorted_list.append(capture)
        for check in checks:
            sorted_list.append(check)
        for move_ in other_moves:
            sorted_list.append(move_)

        self.array_of_legal_moves = []
        self.array_of_legal_moves = sorted_list

class pawn(piece):
    def __init__(self, color = 0, row = 0, column = 0):
        super().__init__(color, row, column)
        self.type = "p"
        self.image_white = white_pawn_surface
        self.image_black = black_pawn_surface
        self.value = PAWN_VALUE

        self.position_values = [
            0, 0, 0, 0, 0, 0, 0, 0,
            55, 70, 70, 70, 70, 70, 70, 55,
            45, 45, 48, 50, 50, 48, 45, 45,
            40, 40, 45, 48, 48, 45, 40, 40,
            20, 20, 20, 23, 23, 20, 20, 20,
            10, 12, 12, 14, 14, 12, 12, 10,
            8, 10, 10, 12, 12, 10, 10, 8,
            0, 0, 0, 0, 0, 0, 0, 0 ]

        if self.color != 0:
            self.set_image()

    def calculate_pseudo_legal_moves(self, board):
        last_move = board.peek_last_move()

        if self.color == "w":
            if last_move != 0 and last_move.is_pawn_double_move() and last_move.new_pos[0] == self.row and abs(last_move.new_pos[1] - self.column) == 1:
                self.array_of_legal_moves.append((last_move.new_pos[0] + 1, last_move.new_pos[1]))
            try:
                if 2 <= self.row + 1 <= 7 and 0 <= self.column <= 7 and board.array[self.row + 1][self.column] == 0:
                    self.array_of_legal_moves.append((self.row + 1, self.column))
                    if self.row == 1 and board.array[self.row + 2][self.column] == 0:
                        self.array_of_legal_moves.append((self.row + 2, self.column))
            except:
                pass

            try:
                if 2 <= self.row + 1 <= 7 and 0 <= self.column - 1 <= 7 and board.array[self.row + 1][self.column - 1].color != self.color:
                    self.array_of_legal_moves.append((self.row + 1, self.column - 1))
            except:
                pass

            try:
                if 2 <= self.row + 1 <= 7 and 0 <= self.column + 1 <= 7 and board.array[self.row + 1][self.column + 1].color != self.color:
                    self.array_of_legal_moves.append((self.row + 1, self.column + 1))
            except:
                pass

        elif self.color == "b":
            if last_move != 0 and last_move.is_pawn_double_move() and last_move.new_pos[0] == self.row and abs(last_move.new_pos[1] - self.column) == 1:
                self.array_of_legal_moves.append((last_move.new_pos[0] - 1, last_move.new_pos[1]))
            
            try:
                if 0 <= self.row - 1 <= 5 and 0 <= self.column <= 7 and board.array[self.row - 1][self.column] == 0:
                    self.array_of_legal_moves.append((self.row - 1, self.column))
                    if self.row == 6 and board.array[self.row - 2][self.column] == 0:
                        self.array_of_legal_moves.append((self.row - 2, self.column))
            except:
                pass
        
            try:
                if 0 <= self.row - 1 <= 5 and 0 <= self.column - 1 <= 7 and board.array[self.row - 1][self.column - 1].color != self.color:
                    self.array_of_legal_moves.append((self.row - 1, self.column - 1))
            except:
                pass

            try:
                if 0 <= self.row - 1 <= 5 and 0 <= self.column + 1 <= 7 and board.array[self.row - 1][self.column + 1].color != self.color:
                    self.array_of_legal_moves.append((self.row - 1, self.column + 1))
            except:
                pass        

class bishop(piece):
    def __init__(self, color = 0, row = 0, column = 0):
        super().__init__(color, row, column)
        self.type = "b"
        self.image_white = white_bishop_surface
        self.image_black = black_bishop_surface
        self.value = BISHOP_VALUE

        self.position_values = [
            -30, -25, -45, -45, -45, -45, -25, -30,
            -30, -5, -5, -5, -5, -5, -5, -30,
            -30, 0, 0, 0, 0, 0, 0, -30,
            -30, 0, 0, 0, 0, 0, 0, -30,
            0, 10, 10, 3, 3, 10, 10, 0,
            0, 12, 22, 4, 4, 22, 12, 0,
            10, 50, 2, 2, 2, 2, 50, 10,
            10, 8, 0, -10, -10, 0, 8, 10 ]

        if self.color != 0:
            self.set_image()

    def calculate_pseudo_legal_moves(self, board):
        for i in [-1, 1]:
            for j in [-1, 1]:
                counter = 0
                while is_row_and_column_in_range(self.row + i * (counter + 1), self.column + j * (counter + 1)):
                    counter += 1
                    if board.array[self.row + i * counter][self.column + j * counter] == 0:
                        self.array_of_legal_moves.append((self.row + i * counter, self.column + j * counter))
                    elif board.array[self.row + i * counter][self.column + j * counter].color != self.color:
                        self.array_of_legal_moves.append((self.row + i * counter, self.column + j * counter))
                        break
                    else:
                        break

class knight(piece):
    def __init__(self, color = 0, row = 0, column = 0):
        super().__init__(color, row, column)
        self.type = "n"
        self.image_white = white_knight_surface
        self.image_black = black_knight_surface
        self.value = KNIGHT_VALUE

        self.position_values = [
            -30, -15, -15, -15, -15, -15, -15, -30,
            -30, 45, 48, 50, 50, 48, 45, -30,
            -30, 45, 48, 50, 50, 48, 45, -30,
            -30, 40, 45, 48, 48, 45, 40, -30,
            -30, 20, 20, 23, 23, 20, 20, -30,
            -30, 12, 12, 14, 14, 12, 12, -30,
            -30, 0, 2, 2, 2, 2, 0, -30,
            -30, -15, -15, -15, -15, -15, -15, -30 ]

        if self.color != 0:
            self.set_image()

    def calculate_pseudo_legal_moves(self, board):
        for i in [-2, -1, 1, 2]:
            for j in [-2, -1, 1, 2]:
                if (abs(i * j) == 2):
                    if is_row_and_column_in_range(self.row + i, self.column + j):
                        if board.array[self.row + i][self.column + j] == 0:
                            self.array_of_legal_moves.append((self.row + i, self.column + j))
                        elif board.array[self.row + i][self.column + j].color != self.color:
                            self.array_of_legal_moves.append((self.row + i, self.column + j))
    
class rook(piece):
    def __init__(self, color = 0, row = 0, column = 0):
        super().__init__(color, row, column)
        self.type = "r"
        self.image_white = white_rook_surface
        self.image_black = black_rook_surface
        self.value = ROOK_VALUE

        self.position_values = [
            -5, 0, 0, 50, 50, 50, 0, -5,
            -10, 0, 0, 50, 50, 50, 0, -10,
            -20, 0, 0, 40, 50, 50, 0, -20,
            -30, -30, -30, -30, -30, -30, -30, -30,
            -30, -30, -30, -30, -30, -30, -30, -30,
            -30, -30, -30, -30, -30, -30, -30, -30,
            0, 0, 0, 10, 10, 10, 0, 0,
            0, 0, 0, 50, 50, 50, 0, 0 ]

        if self.color != 0:
            self.set_image()

    def calculate_pseudo_legal_moves(self, board):
        for i in [-1, 1]:
            counter = 0
            while 0 <= self.row + i * (counter + 1) <= 7:
                counter += 1
                if board.array[self.row + i * counter][self.column] == 0:
                    self.array_of_legal_moves.append((self.row + i * counter, self.column))
                elif board.array[self.row + i * counter][self.column].color != self.color:
                    self.array_of_legal_moves.append((self.row + i * counter, self.column))
                    break
                else:
                    break
        for j in [-1, 1]:
            counter = 0
            while 0 <= self.column + j * (counter + 1) <= 7:
                counter += 1
                if board.array[self.row][self.column + j * counter] == 0:
                    self.array_of_legal_moves.append((self.row, self.column + j * counter))
                elif board.array[self.row][self.column + j * counter].color != self.color:
                    self.array_of_legal_moves.append((self.row, self.column + j * counter))
                    break
                else:
                    break
        
class queen(piece):
    def __init__(self, color = 0, row = 0, column = 0):
        super().__init__(color, row, column)
        self.type = "q"
        self.image_white = white_queen_surface
        self.image_black = black_queen_surface
        self.value = QUEEN_VALUE

        self.position_values = [
            10, 20, 30, 40, 40, 30, 20, 10,
            20, 30, 40, 50, 50, 40, 30, 20,
            20, 30, 50, 55, 55, 50, 30, 20,
            20, 30, 40, 50, 50, 40, 30, 20,
            0, 30, 40, 50, 50, 40, 30, 0,
            -10, 10, 20, 25, 25, 20, 10, -10,
            -20, 0, 10, 15, 15, 10, 0, -20,
            -30, 0, 0, 5, 5, 0, 0, -30 ]

        if self.color != 0:
            self.set_image()

    def calculate_pseudo_legal_moves(self, board):
        b = bishop(self.color, self.row, self.column)
        r = rook(self.color, self.row, self.column)

        b.calculate_pseudo_legal_moves(board)
        r.calculate_pseudo_legal_moves(board)

        for position in b.array_of_legal_moves:
            self.array_of_legal_moves.append(position)
        
        for position in r.array_of_legal_moves:
            self.array_of_legal_moves.append(position)

        del b
        del r

class king(piece):
    def __init__(self, color = 0, row = 0, column = 0):
        super().__init__(color, row, column)
        self.type = "k"
        self.value = KING_VALUE
        self.is_in_check = False
        self.image_white = white_king_surface
        self.image_black = black_king_surface
        self.start_row = 0
        self.start_column = 4

        self.position_values_opening = [
            -60, -60, -60, -60, -60, -60, -60, -60,
            -60, -60, -60, -60, -60, -60, -60, -60,
            -60, -60, -60, -60, -60, -60, -60, -60,
            -60, -60, -60, -60, -60, -60, -60, -60,
            -60, -60, -60, -60, -60, -60, -60, -60,
            -35, -35, -35, -35, -35, -35, -35, -35,
            -20, -20, -20, -20, -20, -20, -20, -20,
            80, 80, 35, 5, 5, 35, 80, 80
        ]

        self.position_values_endgame = [
            40, 40, 40, 40, 40, 40, 40, 40,
            35, 35, 35, 35, 35, 35, 35, 35,
            25, 27, 27, 27, 27, 27, 27, 25,
            25, 25, 25, 25, 25, 25, 25, 25,
            25, 25, 25, 25, 25, 25, 25, 25,
            15, 15, 15, 15, 15, 15, 15, 15,
            0, 0, 0, 0, 0, 0, 0, 0,
            -10, -10, -10, -10, -10, -10, -10, -10
        ]

        if self.color != 0:
            self.set_image()
            self.set_start_pos()
        
    def set_start_pos(self):
        if self.color == "w":
            self.start_row = 0
        else:
            self.start_row = 7

    def calculate_pseudo_legal_moves(self, board):
        for i in range(-1, 2):
            for j in range(-1, 2):
                if i == j and j == 0:
                    pass
                elif is_row_and_column_in_range(self.row + i, self.column + j):
                    if board.array[self.row + i][self.column + j] == 0:
                        self.array_of_legal_moves.append((self.row + i, self.column + j))

                    elif board.array[self.row + i][self.column + j].color != self.color:
                        self.array_of_legal_moves.append((self.row + i, self.column + j))
        if not self.is_in_check:
            if self.color == "w":
                if board.white_state.short_castle and board.array[self.row][self.column + 1] == 0 and board.array[self.row][self.column + 2] == 0:
                    self.array_of_legal_moves.append((self.row, self.column + 2))
                if board.white_state.long_castle and board.array[self.row][self.column - 1] == 0 and board.array[self.row][self.column - 2] == 0 and board.array[self.row][self.column - 3] == 0:
                    self.array_of_legal_moves.append((self.row, self.column - 2))
            else:
                if board.black_state.short_castle and board.array[self.row][self.column + 1] == 0 and board.array[self.row][self.column + 2] == 0:
                    self.array_of_legal_moves.append((self.row, self.column + 2))
                if board.black_state.long_castle and board.array[self.row][self.column - 1] == 0 and board.array[self.row][self.column - 2] == 0 and board.array[self.row][self.column - 3] == 0:
                    self.array_of_legal_moves.append((self.row, self.column - 2))
