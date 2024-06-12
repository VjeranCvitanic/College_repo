from src.piece import *
from src.helpers import *
from src.pick_promotion_piece import pick_promotion_piece
import copy
from src.move import move

class player_state:
    def __init__(self, color, king, short_castle, long_castle, list_of_pieces, pieces_value):
        self.color = color
        self.king = king
        self.short_castle = short_castle
        self.long_castle = long_castle
        self.list_of_pieces = list_of_pieces
        self.pieces_value = pieces_value

class board:
    def __init__(self):
       self.array = [[0 for i in range(8)] for j in range(8)]
       self.white_to_play = True
       self.piece_clicked = 0
       self.stack_of_played_moves = []
       self.is_endgame = False
       self.fifty_moves_rule = 0
       
       self.white_state = player_state("w", 0, True, True, [], 0)
       self.black_state = player_state("b", 0, True, True, [], 0)

       self.starting_setup_test_3()
       self.create_lists_of_pieces()
       self.calculate_value_of_pieces()

    def starting_setup(self):
        for j in range(0, 8):
            white_pawn = pawn("w", 1, j)
            black_pawn = pawn("b", 6, j)
            self.array[1][j] = white_pawn
            self.array[6][j] = black_pawn
        
        white_rook = rook("w", 0, 0)
        self.array[0][0] = white_rook

        white_rook = rook("w", 0, 7)
        self.array[0][7] = white_rook

        black_rook = rook("b", 7, 0)
        self.array[7][0] = black_rook

        black_rook = rook("b", 7, 7)
        self.array[7][7] = black_rook

        white_bishop = bishop("w", 0, 2)
        self.array[0][2] = white_bishop

        white_bishop = bishop("w", 0, 5)
        self.array[0][5] = white_bishop

        black_bishop = bishop("b", 7, 2)
        self.array[7][2] = black_bishop

        black_bishop = bishop("b", 7, 5)
        self.array[7][5] = black_bishop

        white_knight = knight("w", 0, 1)
        self.array[0][1] = white_knight

        white_knight = knight("w", 0, 6)
        self.array[0][6] = white_knight

        black_knight = knight("b", 7, 1)
        self.array[7][1] = black_knight

        black_knight = knight("b", 7, 6)
        self.array[7][6] = black_knight

        white_queen = queen("w", 0, 3)
        self.array[0][3] = white_queen

        black_queen = queen("b", 7, 3)
        self.array[7][3] = black_queen

        white_king = king("w", 0, 4)
        self.array[0][4] = white_king
        self.white_state.king = white_king

        black_king = king("b", 7, 4)
        self.array[7][4] = black_king
        self.black_state.king = black_king
    
    def starting_setup_test(self):
        white_bishop = bishop("w", 0, 2)
        self.array[0][2] = white_bishop

        white_knight = knight("w", 0, 6)
        self.array[0][6] = white_knight

        black_knight = knight("b", 7, 1)
        self.array[7][1] = black_knight

        black_knight = knight("b", 7, 6)
        self.array[7][6] = black_knight

        white_king = king("w", 0, 4)
        self.array[0][4] = white_king
        self.white_state.king = white_king

        black_king = king("b", 7, 4)
        self.array[7][4] = black_king
        self.black_state.king = black_king

        self.white_state.short_castle = False
        self.white_state.long_castle = False
        self.black_state.short_castle = False
        self.black_state.long_castle = False

    def starting_setup_test_3(self):
        for j in range(0, 8):
            if j != 4:
                white_pawn = pawn("w", 1, j)
                self.array[1][j] = white_pawn
            else:
                white_pawn = pawn("w", 3, 4)
                self.array[3][4] = white_pawn
            if j != 0 and j != 1 and j != 3:
                black_pawn = pawn("b", 6, j)
                self.array[6][j] = black_pawn
            elif j == 0:
                black_pawn = pawn("b", 5, 0)
                self.array[5][0] = black_pawn
            elif j == 1:
                black_pawn = pawn("b", 3, 1)
                self.array[3][1] = black_pawn
            else:
                black_pawn = pawn("b", 5, 3)
                self.array[5][3] = black_pawn
        
        white_rook = rook("w", 0, 0)
        self.array[0][0] = white_rook

        white_rook = rook("w", 0, 7)
        self.array[0][7] = white_rook

        black_rook = rook("b", 7, 0)
        self.array[7][0] = black_rook

        black_rook = rook("b", 7, 7)
        self.array[7][7] = black_rook

        white_bishop = bishop("w", 0, 2)
        self.array[0][2] = white_bishop

        white_bishop = bishop("w", 3, 2)
        self.array[3][2] = white_bishop

        black_bishop = bishop("b", 7, 2)
        self.array[7][2] = black_bishop

        black_bishop = bishop("b", 7, 5)
        self.array[7][5] = black_bishop

        black_bishop = bishop("b", 1, 4)
        self.array[1][4] = black_bishop

        white_knight = knight("w", 0, 1)
        self.array[0][1] = white_knight

        white_knight = knight("w", 0, 6)
        self.array[0][6] = white_knight

        black_knight = knight("b", 7, 1)
        self.array[7][1] = black_knight

        black_knight = knight("b", 7, 6)
        self.array[7][6] = black_knight

        white_queen = queen("w", 2, 5)
        self.array[2][5] = white_queen

        black_queen = queen("b", 7, 3)
        self.array[7][3] = black_queen

        white_king = king("w", 0, 4)
        self.array[0][4] = white_king
        self.white_state.king = white_king

        black_king = king("b", 7, 4)
        self.array[7][4] = black_king
        self.black_state.king = black_king

    def calculate_value_of_pieces(self):
        self.white_state.pieces_value = 0
        self.black_state.pieces_value = 0
        for piece in self.white_state.list_of_pieces:
            self.white_state.pieces_value += piece.value
        for piece in self.black_state.list_of_pieces:
            self.black_state.pieces_value += piece.value

    def create_lists_of_pieces(self):
        for row in self.array:
            for square in row:
                if square != 0:
                    if square.color == "w":
                        self.white_state.list_of_pieces.append(square)
                    else:
                        self.black_state.list_of_pieces.append(square)

    def king_position(self, color):
        if color == "w":
            return (self.white_state.king.row, self.white_state.king.column)
        else:
            return (self.black_state.king.row, self.black_state.king.column)

    def peek_last_move(self):
        index_of_last_move = len(self.stack_of_played_moves) - 1
        if index_of_last_move == -1:
            return 0
        return self.stack_of_played_moves[index_of_last_move]

    def pop_last_move(self):
        index_of_last_move = len(self.stack_of_played_moves) - 1
        if index_of_last_move < 0:
            return False 
        last_move = self.stack_of_played_moves[index_of_last_move]

        self.stack_of_played_moves.remove(last_move)

        return last_move

    def push_move(self, move):
        self.stack_of_played_moves.append(move)

    def is_game_over(self):
        color = self.color_to_play()
        is_check = self.is_in_check(color)

        if self.is_stalemate(color):
            if is_check:
                return "checkmate"
            return "stalemate"
        
        if self.is_draw_by_insufficient_material():
            return "draw"
        
        if self.white_to_play:  # if white has the next move
            self.black_state.king.is_in_check = False
            self.white_state.king.is_in_check = is_check
        else:
            self.white_state.king.is_in_check = False
            self.black_state.king.is_in_check = is_check
        return False
    
    def is_draw_by_insufficient_material(self):
        if self.white_state.pieces_value < min(KNIGHT_VALUE, BISHOP_VALUE) * 2 and self.black_state.pieces_value < min(KNIGHT_VALUE, BISHOP_VALUE) * 2:
            flag = False
            for piece in self.white_state.list_of_pieces:
                if piece.type == "p" or piece.type == "r":
                    flag = True
            for piece in self.black_state.list_of_pieces:
                if piece.type == "p" or piece.type == "r":
                    flag = True
            if flag == False:
                return True
        return False

    def is_stalemate(self, color):
        if color == "w":
            list = self.white_state.list_of_pieces
        else:
            list = self.black_state.list_of_pieces

        self.reset_array_of_legal_moves_for_one_player(color)

        for piece in list:
            piece.calculate_legal_moves(self)
            if piece.array_of_legal_moves != []:
                return False

        return True

    def is_move_enpassant(self, move):
        if move.piece.type == "p" and move.start_pos[1] != move.new_pos[1] and self.array[move.new_pos[0]][move.new_pos[1]] == 0:
            return True
        return False

    def is_move_castle(self, move_):
        return self.piece_clicked.type == "k" and self.piece_clicked.column == 4 and (move_.new_pos[1] == 6 or move_.new_pos[1] == 2)

    def is_move_promotion(self, move_):
        return self.piece_clicked.type == "p" and (move_.new_pos[0] == 7 or move_.new_pos[0] == 0)

    def color_to_play(self):
        if self.white_to_play:
            return "w"
        else:
            return "b"

    def castle_short(self):
        self.array[self.piece_clicked.row][7].column = 5
        self.array[self.piece_clicked.row][5] = self.array[self.piece_clicked.row][7]
        self.array[self.piece_clicked.row][7] = 0
       
        self.piece_clicked.column = 6
        self.array[self.piece_clicked.row][4] = 0
        self.array[self.piece_clicked.row][6] = self.piece_clicked

    def castle_long(self):
        self.array[self.piece_clicked.row][0].column = 3
        self.array[self.piece_clicked.row][3] = self.array[self.piece_clicked.row][0]
        self.array[self.piece_clicked.row][0] = 0
       
        self.piece_clicked.column = 2
        self.array[self.piece_clicked.row][4] = 0
        self.array[self.piece_clicked.row][2] = self.piece_clicked

    def reset_arrays_of_legal_moves(self):
        self.reset_array_of_legal_moves_for_one_player("w")
        self.reset_array_of_legal_moves_for_one_player("b")

    def reset_array_of_legal_moves_for_one_player(self, color):
        if color == "w":
            list = self.white_state.list_of_pieces
        else:
            list = self.black_state.list_of_pieces
        for piece in list:
            piece.reset_array_of_legal_moves()

    def is_in_check(self, color):
        opponent_color = switch_color(color)
        self.reset_array_of_legal_moves_for_one_player(opponent_color)
        self.calculate_all_pseudo_legal_moves_for_one_player(opponent_color)

        if opponent_color == "w":
            for piece in self.white_state.list_of_pieces:
                if self.king_position("b") in piece.array_of_legal_moves:
                    return True
        else:
            for piece in self.black_state.list_of_pieces:
                if self.king_position("w") in piece.array_of_legal_moves:
                    return True

        return False  

    def set_is_endgame(self):
        if self.white_state.pieces_value + self.black_state.pieces_value < 1800:
            self.is_endgame = True
        else:
            self.is_endgame = False

#move ordering
    def is_move_capture(self, move):
        if self.array[move.new_pos[0]][move.new_pos[1]] != 0:
            return True
        return False
                
    def is_move_check(self, move, piece):
        old_piece = self.array[move[0]][move[1]]
        piece_old_row = piece.row
        piece_old_column = piece.column

        self.array[move[0]][move[1]] = piece
        piece.row = move[0]
        piece.column = move[1]

        ret_val = self.is_in_check(switch_color(piece.color))
        
        self.array[move[0]][move[1]] = old_piece
        piece.row = piece_old_row
        piece.column = piece_old_column

        return ret_val


#opening
    def uci_to_move(self, best_move_uci):
        row_old = 0
        column_old = 0
        row_new = 0
        column_new = 0

        column_old = dict[best_move_uci[0]]
        column_new = dict[best_move_uci[2]]

        row_old = int(best_move_uci[1]) - 1
        row_new = int(best_move_uci[3]) - 1

        piece = self.array[row_old][column_old]
        best_move = [row_new, column_new]

        return piece, best_move
  
    def move_to_uci(self, move):
        row_old = move.start_pos[0]
        column_old = move.start_pos[1]
        row_new = move.new_pos[0]
        column_new = move.new_pos[1]

        uci_text = ""

        row_old_text = str(row_old + 1)
        column_old_text = dict_num_to_char[column_old]
        row_new_text = str(row_new + 1)
        column_new_text = dict_num_to_char[column_new]

        uci_text += column_old_text
        uci_text += row_old_text
        uci_text += column_new_text
        uci_text += row_new_text

        uci_text

        return uci_text

    def moves_played_so_far_to_uci_string(self):
        line = ""
        for move in self.stack_of_played_moves:
            line += self.move_to_uci(move)
            line += " "
        
        return line

    def is_move_valid(self, row, column, piece):
        captured_piece = self.array[row][column]
        move_ = move(piece.row, piece.column, row, column, piece, captured_piece, 0)

        if self.is_move_enpassant(move_):
            if piece.color == "w":
                captured_piece = self.array[row - 1][column]
                self.black_state.list_of_pieces.remove(captured_piece)
                self.array[row - 1][column] = 0
            else:
                captured_piece = self.array[row + 1][column]
                self.white_state.list_of_pieces.remove(captured_piece)
                self.array[row + 1][column] = 0
            self.array[row][column] = piece
            self.array[piece.row][piece.column] = 0
            piece.row = row
            piece.column = column

            ret_val = self.is_in_check(piece.color)

            self.array[row][column] = 0
            self.array[move_.start_pos[0]][move_.start_pos[1]] = piece
            if piece.color == "w":
                self.array[row - 1][column] = captured_piece
                self.black_state.list_of_pieces.append(captured_piece)
            else:
                self.array[row + 1][column] = captured_piece
                self.white_state.list_of_pieces.append(captured_piece)

            piece.row = move_.start_pos[0]
            piece.column = move_.start_pos[1]

            return not ret_val

        else:
            self.array[row][column] = piece
            self.array[move_.start_pos[0]][move_.start_pos[1]] = 0
            piece.row = move_.new_pos[0]
            piece.column = move_.new_pos[1]
            if captured_piece != 0:
                if captured_piece.color == "w":
                    self.white_state.list_of_pieces.remove(captured_piece)
                else:
                    self.black_state.list_of_pieces.remove(captured_piece)

            ret_val = self.is_in_check(piece.color)

            self.array[move_.start_pos[0]][move_.start_pos[1]] = piece
            self.array[row][column] = captured_piece
            piece.row = move_.start_pos[0]
            piece.column = move_.start_pos[1]
            if captured_piece != 0:
                if captured_piece.color == "w":
                    self.white_state.list_of_pieces.append(captured_piece)
                else:
                    self.black_state.list_of_pieces.append(captured_piece)

            return not ret_val
            
        
#calculate legal moves
    def calculate_all_legal_moves_for_one_player(self, color):
        if color == "w":
            list = self.white_state.list_of_pieces
        else:
            list = self.black_state.list_of_pieces

        for piece in list:
            piece.calculate_legal_moves(self)


#calculate pseudo legal moves
    def calculate_all_pseudo_legal_moves_for_one_player(self, color):
        if color == "w":
            list = self.white_state.list_of_pieces
        else:
            list = self.black_state.list_of_pieces

        for piece in list:
            piece.calculate_pseudo_legal_moves(self)

    def play_a_move_after(self):
        move = self.peek_last_move()

        captured_piece = move.captured_piece

        if self.color_to_play() == "w":
            if self.piece_clicked.type == "k":
                self.white_state.short_castle = False
                self.white_state.long_castle = False
            elif self.piece_clicked.type == "r":
                if move.start_pos == [0, 0]:
                    self.white_state.long_castle = False
                elif move.start_pos == [0, 7]:
                    self.white_state.short_castle = False
            if captured_piece != 0:
                if captured_piece.type == "r" and captured_piece.row == 7:
                    if captured_piece.column == 0:
                        self.black_state.long_castle = False
                    elif captured_piece.column == 7:
                        self.black_state.short_castle = False
        else:
            if self.piece_clicked.type == "k":
                self.black_state.short_castle = False
                self.black_state.long_castle = False
            elif self.piece_clicked.type == "r":
                if move.start_pos ==  [7, 0]:
                    self.black_state.long_castle = False
                elif move.start_pos == [7, 7]:
                    self.black_state.short_castle = False
            if captured_piece != 0:
                if captured_piece.type == "r" and captured_piece.row == 0:
                    if captured_piece.column == 0:
                        self.white_state.long_castle = False
                    elif captured_piece.column == 7:
                        self.white_state.short_castle = False
                    

        self.piece_clicked = 0

        self.white_to_play = not self.white_to_play

        return self.is_game_over()

    def promote_pawn(self, row, column, promotion_piece):
        color = self.piece_clicked.color
        self.array[self.piece_clicked.row][self.piece_clicked.column] = 0
        if promotion_piece == 0:
            self.array[row][column] = pick_promotion_piece(color, row, column)
        else:
            self.array[row][column] = promotion_piece
        self.piece_clicked = self.array[row][column]
        self.piece_clicked.row = row
        self.piece_clicked.column = column

    def play_promote_pawn(self, move, promotion_piece):
        row = move.new_pos[0]
        column = move.new_pos[1]
        captured_piece = self.array[row][column]

        if self.piece_clicked.color == "w":
            self.white_state.pieces_value -= PAWN_VALUE
            self.white_state.list_of_pieces.remove(self.piece_clicked)
        else:
            self.black_state.pieces_value -= PAWN_VALUE
            self.black_state.list_of_pieces.remove(self.piece_clicked)

        self.promote_pawn(row, column, promotion_piece)

        promotion_piece = self.piece_clicked
        if promotion_piece.color == "w":
            self.white_state.pieces_value += promotion_piece.value
            self.white_state.list_of_pieces.append(promotion_piece)
        else:
            self.black_state.pieces_value += promotion_piece.value
            self.black_state.list_of_pieces.append(promotion_piece)

        if captured_piece != 0:
            if captured_piece.color == "w":
                self.white_state.pieces_value -= captured_piece.value
                self.white_state.list_of_pieces.remove(captured_piece)
            else:
                self.black_state.pieces_value -= captured_piece.value
                self.black_state.list_of_pieces.remove(captured_piece)

        return "promote"

    def enpassant(self, move):
        if self.piece_clicked.color == "w":
            self.array[move.new_pos[0] - 1][move.new_pos[1]] = 0
        elif self.piece_clicked.color == "b":
            self.array[move.new_pos[0] + 1][move.new_pos[1]] = 0
        self.array[self.piece_clicked.row][self.piece_clicked.column] = 0
        self.piece_clicked.row = move.new_pos[0]
        self.piece_clicked.column = move.new_pos[1]
        self.array[move.new_pos[0]][move.new_pos[1]] = self.piece_clicked

    def play_enpassant(self, move):
        captured_piece = self.peek_last_move().piece
        move.captured_piece = captured_piece

        self.enpassant(move)

        if move.captured_piece.color == "w":
            self.white_state.pieces_value -= PAWN_VALUE
            self.white_state.list_of_pieces.remove(move.captured_piece)
        else:
            self.black_state.pieces_value -= PAWN_VALUE
            self.black_state.list_of_pieces.remove(move.captured_piece)

        return "capture"

    def castle(self, move):
        if move.new_pos[1] == 6:
            self.castle_short()
        elif move.new_pos[1] == 2:
            self.castle_long()
        if self.white_to_play:
            self.white_state.king.row = move.new_pos[0]
            self.white_state.king.column = move.new_pos[1]   
        else:
            self.black_state.king.row = move.new_pos[0]
            self.black_state.king.column = move.new_pos[1]

    def play_castle(self, move):
        self.castle(move)

        return "castle"

    def nocapture(self, move):
        self.array[self.piece_clicked.row][self.piece_clicked.column] = 0
        self.piece_clicked.row = move.new_pos[0]
        self.piece_clicked.column = move.new_pos[1]
        self.array[move.new_pos[0]][move.new_pos[1]] = self.piece_clicked
        if self.piece_clicked.type == "k":
            if self.piece_clicked.color == "w":
                self.white_state.king.row = move.new_pos[0]
                self.white_state.king.column = move.new_pos[1]
            else:
                self.black_state.king.row = move.new_pos[0]
                self.black_state.king.column = move.new_pos[1]

    def play_a_move_no_capture(self, move):
        self.nocapture(move)
        return "nocapture"

    def capture(self, move):
        self.array[self.piece_clicked.row][self.piece_clicked.column] = 0
        self.piece_clicked.row = move.new_pos[0]
        self.piece_clicked.column = move.new_pos[1]
        self.array[move.new_pos[0]][move.new_pos[1]] = self.piece_clicked
        if self.piece_clicked.type == "k":
            if self.piece_clicked.color == "w":
                self.white_state.king.row = move.new_pos[0]
                self.white_state.king.column = move.new_pos[1]
            else:
                self.black_state.king.row = move.new_pos[0]
                self.black_state.king.column = move.new_pos[1]

    def play_a_capture(self, move):
        if move.captured_piece.color == "w":
            self.white_state.list_of_pieces.remove(move.captured_piece)
            self.white_state.pieces_value -= move.captured_piece.value
        else:
            self.black_state.list_of_pieces.remove(move.captured_piece)
            self.black_state.pieces_value -= move.captured_piece.value

        self.capture(move)
        
        return "capture"

    def play_music(self, move_type):
        if move_type == "castle":
            pygame.mixer.music.load(castle_sound_path)
            pygame.mixer.music.play(1)
        elif move_type == "capture":
            pygame.mixer.music.load(capture_sound_effect)
            pygame.mixer.music.play(1)
        else:
            pygame.mixer.music.load(move_sound_path)
            pygame.mixer.music.play(1)
        
        if self.white_state.king.is_in_check or self.black_state.king.is_in_check:
            pygame.mixer.music.load(check_sound_effect)
            pygame.mixer.music.play(1)

    def play_a_move(self, row, column, promotion_piece):
        move_ = move(self.piece_clicked.row, self.piece_clicked.column, row, column, self.piece_clicked, self.array[row][column], promotion_piece, self.white_state.short_castle, self.white_state.long_castle, self.black_state.short_castle, self.black_state.long_castle)

        move_type = 0
            
        move_type = self.move(move_, promotion_piece)

        if move_type == "promote":
            move_.promoted_piece = self.piece_clicked

        move_.white_state_before_move = (self.white_state.short_castle, self.white_state.long_castle)
        move_.black_state_before_move = (self.black_state.short_castle, self.black_state.long_castle)

        self.push_move(move_)

        result = self.play_a_move_after()

        if result != False:
            return result

        #self.play_music(move_type)

        return False

    def move(self, move_, promotion_piece):
        if self.is_move_castle(move_):
            return self.play_castle(move_)
        if self.is_move_enpassant(move_):
            return self.play_enpassant(move_)
        if self.is_move_promotion(move_):
            return self.play_promote_pawn(move_, promotion_piece)
        if self.array[move_.new_pos[0]][move_.new_pos[1]] == 0:
            return self.play_a_move_no_capture(move_)
        if self.is_move_capture(move_):
            return self.play_a_capture(move_)
            
            
    def unmake_move(self):
        move = self.pop_last_move()
        if not move:
            return

        self.white_state.short_castle = move.white_state_before_move[0]
        self.white_state.long_castle = move.white_state_before_move[1]
        self.black_state.short_castle = move.black_state_before_move[0]
        self.black_state.long_castle = move.black_state_before_move[1]

        self.array[move.start_pos[0]][move.start_pos[1]] = move.piece
        move.piece.row = move.start_pos[0]
        move.piece.column = move.start_pos[1]
        self.array[move.new_pos[0]][move.new_pos[1]] = 0

        if move.piece.type == "k" and abs(move.start_pos[1] - move.new_pos[1]) == 2:
            if move.start_pos[1] - move.new_pos[1] == 2:
                rook = self.array[move.new_pos[0]][move.new_pos[1] + 1]
                self.array[move.new_pos[0]][move.new_pos[1] + 1] = 0
                self.array[move.start_pos[0]][move.start_pos[1] - 4] = rook
                rook.column = move.start_pos[1] - 4
            else:
                rook = self.array[move.new_pos[0]][move.new_pos[1] - 1]
                self.array[move.new_pos[0]][move.new_pos[1] - 1] = 0
                self.array[move.start_pos[0]][move.start_pos[1] + 3] = rook
                rook.column = move.start_pos[1] + 3

        if move.captured_piece != 0:
            if move.captured_piece.color == "w":
                self.white_state.pieces_value += move.captured_piece.value
            else:
                self.black_state.pieces_value += move.captured_piece.value
            #enpassant
            if move.piece.type == "p" and move.start_pos[0] != move.new_pos[0] and move.captured_piece.row != move.new_pos[0]: 
                if move.captured_piece.color == "w":
                    move.captured_piece.row = move.new_pos[0] + 1
                    move.captured_piece.column = move.new_pos[1]
                    self.array[move.new_pos[0] + 1][move.new_pos[1]] = move.captured_piece
                    self.white_state.list_of_pieces.append(move.captured_piece)
                else:
                    move.captured_piece.row = move.new_pos[0] - 1
                    move.captured_piece.column = move.new_pos[1]
                    self.array[move.new_pos[0] - 1][move.new_pos[1]] = move.captured_piece
                    self.black_state.list_of_pieces.append(move.captured_piece)
            else:
                self.array[move.new_pos[0]][move.new_pos[1]] = move.captured_piece
                move.captured_piece.row = move.new_pos[0]
                move.captured_piece.column = move.new_pos[1]
                if move.captured_piece.color == "w":
                    self.white_state.list_of_pieces.append(move.captured_piece)
                else:
                    self.black_state.list_of_pieces.append(move.captured_piece)

            
        if move.promoted_piece != 0:
            if move.promoted_piece.color == "w":
                self.white_state.pieces_value -= move.promoted_piece.value
                self.white_state.pieces_value += move.piece.value
                self.white_state.list_of_pieces.remove(move.promoted_piece)
                self.white_state.list_of_pieces.append(move.piece)
            else:
                self.black_state.pieces_value -= move.promoted_piece.value
                self.black_state.pieces_value += move.piece.value
                self.black_state.list_of_pieces.remove(move.promoted_piece)
                self.black_state.list_of_pieces.append(move.piece)
        
        self.white_to_play = not self.white_to_play

    def print_array(self):
        print("NEW ARRAY _---_____-_____----")
        for row in self.array:
            print("\n")
            for square in row:
                print(square)

    #position evaluation
    def static_eval(self, number):
        ret_val = 0
        white_val = 0
        black_val = 0

        if number == 1:
            white_val = self.value_of_position_1("w")
            black_val = self.value_of_position_1("b")
        elif number == 2:
            white_val = self.value_of_position_2("w")
            black_val = self.value_of_position_2("b")
        elif number == 3:
            white_val = self.value_of_position_3("w")
            black_val = self.value_of_position_3("b")
        elif number == 4:
            white_val = self.value_of_position_4("w")
            black_val = self.value_of_position_4("b")
        

        ret_val = white_val - black_val

        return ret_val

    def value_of_position_final(self, color):
        ret_val = 0
        self.calculate_value_of_pieces()

        if color == "w":
            list = self.white_state.list_of_pieces
            ret_val += self.white_state.pieces_value
        else:
            list = self.black_state.list_of_pieces
            ret_val += self.black_state.pieces_value

        for piece in list:
            if piece.type == "k":
                ret_val += self.king_safety(piece)
            else:
                if piece.type == "p":
                    ret_val += self.pawn_value(piece)
                else:
                    ret_val += self.piece_activity(piece)

        return ret_val
    
    def piece_activity(self, piece):
        count = 0
        factor = 10

        piece.reset_array_of_legal_moves()
        piece.calculate_pseudo_legal_moves(self)
        count = len(piece.array_of_legal_moves)

        return count * factor

    def king_safety(self, king):
        value = 0
        row = king.row
        column = king.column

        if king.color == "b":
            if self.white_state.pieces_value >= 17:
                if not (column < 3 or column > 5):
                    value -= 150
            elif self.white_state.pieces_value <= 5:
                if row > 2 and row < 6:
                    value += 75
            if row == 7 and (column > 5 or column < 3):
                try:
                    if self.array[row - 1][column] != 0:
                        value += 75
                except:
                    pass
                try:
                    if self.array[row - 1][column - 1] != 0:
                        value += 45
                except:
                    pass
                try:
                    if self.array[row - 1][column + 1] != 0:
                        value += 45
                except:
                    pass

        else:
            if self.black_state.pieces_value >= 17:
                if not (column < 3 or column > 5):
                    value -= 150
            elif self.black_state.pieces_value <= 5:
                if row > 2 and row < 6:
                    value += 75
            if row == 0 and (column > 5 or column < 3):
                try:
                    if self.array[row + 1][column] != 0:
                        value += 75
                except:
                    pass
                try:
                    if self.array[row + 1][column - 1] != 0:
                        value += 45
                except:
                    pass
                try:
                    if self.array[row + 1][column + 1] != 0:
                        value += 45
                except:
                    pass

        return value
        
    def pawn_value(self, pawn):
        ret_val = 0
        ret_val += self.is_pawn_supported(pawn)
        ret_val += self.is_pawn_isolated(pawn)
        ret_val += self.is_pawn_passed(pawn)
        ret_val += self.is_pawn_doubled(pawn)

        return ret_val

    def is_pawn_supported(self, pawn):
        row = pawn.row
        column = pawn.column
        color = pawn.color
        ret_val = 0

        if color == "w":
            perspective = 1
        else:
            perspective = -1
        try:
            neighbour_1 = self.array[row - 1 * perspective][column - 1]
            if neighbour_1.type == "p" and neighbour_1.color == color:
                ret_val += 30
        except:
            ret_val -= 5
        try:
            neighbour_2 = self.array[row - 1 * perspective][column + 1]
            if neighbour_2.type == "p" and neighbour_2.color == color:
                ret_val += 30
        except:
            ret_val -= 5
            
        return ret_val
     
    def is_pawn_isolated(self, pawn):
        column = pawn.column
        color = pawn.color
        flag_1 = False
        flag_2 = False

        for i in range(0, 8):
            try:
                neighbour_1 = self.array[i][column - 1]
                if neighbour_1.type == "p" and neighbour_1.color == color:
                    flag_1 = True
            except:
                pass
    
            try:
                neighbour_2 = self.array[i][column + 1]
                if neighbour_2.type == "p" and neighbour_2.color == color:
                    flag_2 = True
            except:
                pass

        if not flag_1 and not flag_2:
            return -10
        
        return 0

    def is_pawn_passed(self, pawn):
        row = pawn.row
        column = pawn.column
        color = pawn.color
        ret_val = 0
        flag_1 = True
        flag_2 = True

        if color == "w":
            for i in range(row + 1, 8):
                try:
                    neighbour_1 = self.array[i][column - 1]
                    if neighbour_1.type == "p" and neighbour_1.color != color:
                        flag_1 = False
                        break
                except:
                    pass

                try:
                    neighbour_1 = self.array[i][column]
                    if neighbour_1.type == "p" and neighbour_1.color != color:
                        flag_1 = False
                        break
                except:
                    pass
            
                try:
                    neighbour_2 = self.array[i][column + 1]
                    if neighbour_2.type == "p" and neighbour_2.color != color:
                        flag_2 = False
                        break
                except:
                    pass
        else:
            for i in range(0, row):
                try:
                    neighbour_1 = self.array[i][column - 1]
                    if neighbour_1.type == "p" and neighbour_1.color != color:
                        flag_1 = False
                        break
                except:
                    pass

                try:
                    neighbour_2 = self.array[i][column]
                    if neighbour_2.type == "p" and neighbour_2.color != color:
                        flag_2 = False
                        break
                except:
                    pass
            
                try:
                    neighbour_2 = self.array[i][column + 1]
                    if neighbour_2.type == "p" and neighbour_2.color != color:
                        flag_2 = False
                        break
                except:
                    pass

        if flag_1 and flag_2:
            return 75
        
        if flag_2 or flag_1:
            return 10
            
        return 0

    def is_pawn_doubled(self, pawn):
        row = pawn.row
        column = pawn.column
        color = pawn.color
        ret_val = 0

        if color == "w":
            for i in range(row + 1, 8):
                try:
                    neighbour = self.array[i][column]
                    if neighbour.type == "p" and neighbour.color == color:
                        ret_val -= 80
                except:
                    pass
        else:
            for i in range(0, row):
                try:
                    neighbour = self.array[i][column]
                    if neighbour.type == "p" and neighbour.color == color:
                        ret_val -= 80
                except:
                    pass
 
        return ret_val

    def value_of_position_1(self, color):
        self.calculate_value_of_pieces()

        if color == "w":
            return self.white_state.pieces_value
        else:
            return self.black_state.pieces_value

    def value_of_position_2(self, color):
        ret_val = 0

        ret_val += self.value_of_position_1(color)

        if color == "w":
            list = self.white_state.list_of_pieces
        else:
            list = self.black_state.list_of_pieces
        for piece in list:
            if piece.type == "p":
                ret_val += self.pawn_value(piece)

        return ret_val
    
    def value_of_position_3(self, color):
        ret_val = 0

        ret_val += self.value_of_position_2(color)

        if color == "w" and self.white_state.king.row == 0:
            try:
                square_in_front_of_king = self.array[self.white_state.king.row + 1][self.white_state.king.column]
                if square_in_front_of_king != 0:
                    if square_in_front_of_king.type == "p" and square_in_front_of_king.color == color:
                        ret_val += 50
            except:
                pass
            try:
                square_left = self.array[self.white_state.king.row + 1][self.white_state.king.column - 1]
                if square_left != 0:
                    if square_left.type == "p" and square_left.color == color:
                        ret_val += 20
            except:
                pass
            try:
                square_right = self.array[self.white_state.king.row + 1][self.white_state.king.column + 1]
                if square_right != 0:
                    if square_right.type == "p" and square_right.color == color:
                        ret_val += 20
            except:
                pass
        elif color == "b" and self.black_state.king.row == 7:
            try:
                square_in_front_of_king = self.array[self.black_state.king.row - 1][self.black_state.king.column]
                if square_in_front_of_king != 0:
                    if square_in_front_of_king.type == "p" and square_in_front_of_king.color == color:
                        ret_val += 50
            except:
                pass
            try:
                square_left = self.array[self.black_state.king.row - 1][self.black_state.king.column - 1]
                if square_left != 0:
                    if square_left.type == "p" and square_left.color == color:
                        ret_val += 20
            except:
                pass
            try:
                square_right = self.array[self.black_state.king.row - 1][self.black_state.king.column + 1]
                if square_right != 0:
                    if square_right.type == "p" and square_right.color == color:
                        ret_val += 20
            except:
                pass

        return ret_val
    
    def value_of_position_4(self,color):
        ret_val = 0

        ret_val += self.value_of_position_3(color)

        if color == "w":
            list = self.white_state.list_of_pieces
            for piece in list:
                if piece.type != "k":
                    ret_val += piece.position_values[63 - 8 * piece.row - piece.column]
                else:
                    if self.is_endgame:
                        ret_val += piece.position_values_endgame[63 - 8 * piece.row - piece.column]
                    else:
                        ret_val += piece.position_values_opening[63 - 8 * piece.row - piece.column]

        else:
            list = self.black_state.list_of_pieces
            for piece in list:
                if piece.type != "k":
                    ret_val += piece.position_values[8 * piece.row + piece.column]
                else:
                    if self.is_endgame:
                        ret_val += piece.position_values_endgame[8 * piece.row + piece.column]
                    else:
                        ret_val += piece.position_values_opening[8 * piece.row + piece.column]
        
        return ret_val

         