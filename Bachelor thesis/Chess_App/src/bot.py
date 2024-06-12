from src.settings import *
from src.helpers import *
from src.BoardModule import board
from src.move import move
from random import choice, randint
from src.piece import bishop, rook, knight, queen

class bot_IF():
    def __init__(self, color, eval_func_num):
        self.color = color
        self.eval_func_num = eval_func_num
        self.number_of_searches = 0
    
    def is_search_over(self, board_, is_max_playing, depth):
        game_state = board_.is_game_over()
        if depth == 0 or game_state != False:
            if game_state == "checkmate":
                if is_max_playing:
                    return -INF
                else:
                    return INF
            elif game_state == "stalemate" or game_state == "draw":
                return 0
            else:
                if self.color == "w":
                    return board_.static_eval(self.eval_func_num)
                else:
                    return -board_.static_eval(self.eval_func_num)
        return "False"

    def opening(self, board_):
        color = self.color
        new_piece_ = 0

        if color == "w":
            list = board_.white_state.list_of_pieces
        else:
            list = board_.black_state.list_of_pieces

        legal_moves_uci = []
        for piece in list:
            piece.reset_array_of_legal_moves()
            piece.calculate_legal_moves(board_)
            for possible_move in piece.array_of_legal_moves:
                move_ = move(piece.row, piece.column, possible_move[0], possible_move[1], 0, 0, 0)
                legal_moves_uci.append(board_.move_to_uci(move_))
        curr_string = board_.moves_played_so_far_to_uci_string()
        curr_list = curr_string.split()

        list_of_possible_best_moves = []
        list_of_possible_best_pieces = []

        with open('src/openings_in_uci_format.txt') as f:
            line = 1
            while line:
                flag = True
                line = f.readline()
                if line == "":
                    break
                list = line.split()

                length = min(len(curr_list), len(list))
                for i in range(0, length):
                    if curr_list[i] != list[i]:
                        flag = False
                        break
                if flag and len(curr_list) < len(list):
                    best_move_uci = list[len(curr_list)]
                    if best_move_uci in legal_moves_uci:
                        piece, best_move = board_.uci_to_move(best_move_uci)
                        list_of_possible_best_moves.append(best_move)
                        list_of_possible_best_pieces.append(piece)
        f.close()

        if list_of_possible_best_moves == []:
            return False, False, False
        
        random_num = randint(0, len(list_of_possible_best_moves) - 1)

        return list_of_possible_best_pieces[random_num], list_of_possible_best_moves[random_num], new_piece_

class bot_minimax(bot_IF): 
    def __init__(self, color, num_eval):
        super().__init__(color, num_eval)
        self.depth = DEPTH_MINMAX
        self.out_of_opening = False

    def move(self, board_):
        piece, best_move, new_piece = self.minimax(board_, True, self.depth)

        board_.piece_clicked = piece
        return board_.play_a_move(best_move[0], best_move[1], new_piece)
            
    def minimax(self, board_, is_max_playing, depth):
        self.number_of_searches += 1
        best_new_piece = 0
        ret_val = self.is_search_over(board_, is_max_playing, depth)
        if  ret_val != "False":
            return ret_val

        if is_max_playing:
            list_of_possible_best_moves = []
            list_of_possible_best_pieces = []
            list_of_possible_best_new_pieces = []
            if depth == self.depth and not self.out_of_opening:
                best_piece, best_move, best_new_piece = self.opening(board_)
                if best_piece != False:
                    return best_piece, best_move, best_new_piece
                self.out_of_opening = True

            if self.color == "w":
                list = board_.white_state.list_of_pieces
            else:
                list = board_.black_state.list_of_pieces
            max_val = -INF - 1
            for piece in list:
                board_.reset_arrays_of_legal_moves()
                piece.calculate_legal_moves(board_)
                for possible_move in piece.array_of_legal_moves:
                    board_.piece_clicked = piece
                    #promote to every possible piece and call minmax()
                    if(piece.type == "p" and (possible_move[0] == 7 or possible_move[0] == 0)):
                        _bishop = bishop(self.color)
                        _rook = rook(self.color)
                        _queen = queen(self.color)
                        _knight = knight(self.color)
                        list_of_pieces = [_bishop, _rook, _queen, _knight]

                        for new_piece in list_of_pieces:
                            board_.piece_clicked = piece
                            board_.play_a_move(possible_move[0], possible_move[1], new_piece)
                            
                            move_value = self.minimax(board_, False, depth - 1)
                            if move_value >= max_val:
                                max_val = move_value
                            if move_value == max_val and depth == self.depth:
                                list_of_possible_best_moves.append(possible_move)
                                list_of_possible_best_pieces.append(piece)
                                list_of_possible_best_new_pieces.append(new_piece)
                            elif depth == self.depth:
                                list_of_possible_best_moves.clear()
                                list_of_possible_best_pieces.clear()
                                list_of_possible_best_new_pieces.clear()
                                list_of_possible_best_moves.append(possible_move)
                                list_of_possible_best_pieces.append(piece)
                                list_of_possible_best_new_pieces.append(new_piece)

                            board_.unmake_move()
                            board_.reset_array_of_legal_moves_for_one_player(switch_color(self.color))
                    else:
                        board_.play_a_move(possible_move[0], possible_move[1], 0)

                        move_value = self.minimax(board_, False, depth - 1)
                        if move_value >= max_val:
                            max_val = move_value
                            if move_value == max_val and depth == self.depth:
                                list_of_possible_best_moves.append(possible_move)
                                list_of_possible_best_pieces.append(piece)
                                list_of_possible_best_new_pieces.append(new_piece)
                            elif depth == self.depth:
                                list_of_possible_best_moves.clear()
                                list_of_possible_best_pieces.clear()
                                list_of_possible_best_new_pieces.clear()
                                list_of_possible_best_moves.append(possible_move)
                                list_of_possible_best_pieces.append(piece)
                                list_of_possible_best_new_pieces.append(new_piece)


                        board_.unmake_move()
                        board_.reset_array_of_legal_moves_for_one_player(switch_color(self.color))

            if depth == self.depth:
                random_num = randint(0, len(list_of_possible_best_moves) - 1)
                return list_of_possible_best_pieces[random_num], list_of_possible_best_moves[random_num], list_of_possible_best_new_pieces[random_num]
            return max_val

        elif not is_max_playing:
            color = switch_color(self.color)
            board_.reset_arrays_of_legal_moves()
            board_.calculate_all_legal_moves_for_one_player(color)
            min_val = INF + 1

            if color == "w":
                list = board_.white_state.list_of_pieces
            else:
                list = board_.black_state.list_of_pieces

            for piece in list:
                board_.reset_arrays_of_legal_moves()
                piece.calculate_legal_moves(board_)
                for possible_move in piece.array_of_legal_moves:
                    board_.piece_clicked = piece
                    if(piece.type == "p" and (possible_move[0] == 7 or possible_move[0] == 0)):
                        _bishop = bishop(color)
                        _rook = rook(color)
                        _queen = queen(color)
                        _knight = knight(color)
                        list_of_pieces = [_bishop, _rook, _queen, _knight]

                        for new_piece in list_of_pieces:
                            board_.piece_clicked = piece
                            board_.play_a_move(possible_move[0], possible_move[1], new_piece)

                            move_value = self.minimax(board_, True, depth - 1)
                            if move_value <= min_val:
                                min_val = move_value
                            
                            board_.unmake_move()
                            board_.reset_array_of_legal_moves_for_one_player(switch_color(color))
                    else:
                        board_.play_a_move(possible_move[0], possible_move[1], 0)

                        move_value = self.minimax(board_, True, depth - 1)
                        if move_value <= min_val:
                            min_val = move_value
                        
                        board_.unmake_move()
                        board_.reset_array_of_legal_moves_for_one_player(switch_color(color))

            return min_val

class bot_alphabeta(bot_IF):
    def __init__(self, color, num_eval):
        super().__init__(color, num_eval)
        self.depth = DEPTH_AB
        self.out_of_opening = False


    def move(self, board_):
        piece, best_move, new_piece = self.alpha_beta(board_, True, self.depth, -INF, INF)

        board_.piece_clicked = piece

        return board_.play_a_move(best_move[0], best_move[1], new_piece)

    def alpha_beta(self, board_, is_max_playing, depth, alpha, beta):
        self.number_of_searches += 1
        best_new_piece = 0
        ret_val = self.is_search_over(board_, is_max_playing, depth)
        if  ret_val != "False":
            return ret_val

        if is_max_playing:
            list_of_possible_best_moves = []
            list_of_possible_best_pieces = []
            list_of_possible_best_new_pieces = []
            if depth == self.depth and not self.out_of_opening:
                print("OPENING")
                best_piece, best_move, best_new_piece = self.opening(board_)
                if best_piece != False:
                    return best_piece, best_move, best_new_piece
                self.out_of_opening = True

            if self.color == "w":
                list = board_.white_state.list_of_pieces
            else:
                list = board_.black_state.list_of_pieces
            max_val = -INF - 1
            for piece in list:
                board_.reset_arrays_of_legal_moves()
                piece.calculate_legal_moves(board_)
                piece.sort_legal_moves(board_)
                for possible_move in piece.array_of_legal_moves:
                    board_.piece_clicked = piece
                    if(piece.type == "p" and (possible_move[0] == 7 or possible_move[0] == 0)):
                        _bishop = bishop(self.color)
                        _rook = rook(self.color)
                        _queen = queen(self.color)
                        _knight = knight(self.color)
                        list_of_pieces = [_bishop, _rook, _queen, _knight]

                        for new_piece in list_of_pieces:
                            board_.piece_clicked = piece
                            board_.play_a_move(possible_move[0], possible_move[1], new_piece)

                            move_value = self.alpha_beta(board_, False, depth - 1, alpha, beta)
                            if move_value >= max_val:
                                max_val = move_value
                                alpha = max(alpha, max_val)
                            if move_value == max_val and depth == self.depth:
                                list_of_possible_best_moves.append(possible_move)
                                list_of_possible_best_pieces.append(piece)
                                list_of_possible_best_new_pieces.append(new_piece)
                            elif depth == self.depth:
                                list_of_possible_best_moves.clear()
                                list_of_possible_best_pieces.clear()
                                list_of_possible_best_new_pieces.clear()
                                list_of_possible_best_moves.append(possible_move)
                                list_of_possible_best_pieces.append(piece)
                                list_of_possible_best_new_pieces.append(new_piece)
                                if beta <= alpha:
                                    board_.unmake_move()
                                    board_.reset_array_of_legal_moves_for_one_player(switch_color(self.color))
                                    break

                            board_.unmake_move()
                            board_.reset_array_of_legal_moves_for_one_player(switch_color(self.color))
                    else:
                        board_.play_a_move(possible_move[0], possible_move[1], 0)
                        move_value = self.alpha_beta(board_, False, depth - 1, alpha, beta)
                        if move_value >= max_val:
                            max_val = move_value
                            alpha = max(alpha, max_val)
                            if move_value == max_val and depth == self.depth:
                                list_of_possible_best_moves.append(possible_move)
                                list_of_possible_best_pieces.append(piece)
                                list_of_possible_best_new_pieces.append(best_new_piece)
                            elif depth == self.depth:
                                list_of_possible_best_moves.clear()
                                list_of_possible_best_pieces.clear()
                                list_of_possible_best_new_pieces.clear()
                                list_of_possible_best_moves.append(possible_move)
                                list_of_possible_best_pieces.append(piece)
                                list_of_possible_best_new_pieces.append(best_new_piece)
                            if beta <= alpha:
                                board_.unmake_move()
                                board_.reset_array_of_legal_moves_for_one_player(switch_color(self.color))
                                break
                        board_.unmake_move()
                        board_.reset_array_of_legal_moves_for_one_player(switch_color(self.color))
            if depth == self.depth:
                random_num = randint(0, len(list_of_possible_best_moves) - 1)
                return list_of_possible_best_pieces[random_num], list_of_possible_best_moves[random_num], list_of_possible_best_new_pieces[random_num]
            else:
                return max_val

        elif not is_max_playing:
            color = switch_color(self.color)
            board_.reset_arrays_of_legal_moves()
            board_.calculate_all_legal_moves_for_one_player(color)
            min_val = INF + 1

            if color == "w":
                list = board_.white_state.list_of_pieces
            else:
                list = board_.black_state.list_of_pieces

            for piece in list:
                board_.reset_arrays_of_legal_moves()
                piece.calculate_legal_moves(board_)
                piece.sort_legal_moves(board_)
                for possible_move in piece.array_of_legal_moves:
                    board_.piece_clicked = piece
                    if(piece.type == "p" and (possible_move[0] == 7 or possible_move[0] == 0)):
                        _bishop = bishop(color)
                        _rook = rook(color)
                        _queen = queen(color)
                        _knight = knight(color)
                        list_of_pieces = [_bishop, _rook, _queen, _knight]

                        for new_piece in list_of_pieces:
                            board_.piece_clicked = piece
                            board_.play_a_move(possible_move[0], possible_move[1], new_piece)
                            move_value = self.alpha_beta(board_, True, depth - 1, alpha, beta)
                            min_val = min(min_val, move_value)
                            beta = min(beta, min_val)
                            if beta <= alpha:
                                board_.unmake_move()
                                board_.reset_array_of_legal_moves_for_one_player(switch_color(color))
                                break

                            board_.unmake_move()
                            board_.reset_array_of_legal_moves_for_one_player(switch_color(color))
                    else:
                        board_.play_a_move(possible_move[0], possible_move[1], 0)

                        move_value = self.alpha_beta(board_, True, depth - 1, alpha, beta)
                        min_val = min(min_val, move_value)
                        beta = min(beta, min_val)
                        if beta <= alpha:
                            board_.unmake_move()
                            board_.reset_array_of_legal_moves_for_one_player(switch_color(color))
                            break

                        board_.unmake_move()
                        board_.reset_array_of_legal_moves_for_one_player(switch_color(color))

            else:
                return min_val   
    
class bot_random(bot_IF):
    def __init__(self, color, num_eval):
        super().__init__(color, num_eval)
        
    def move(self, board_):
        board_.calculate_all_legal_moves_for_one_player(self.color)

        list_of_pieces = []

        if self.color == "w":
            list = board_.white_state.list_of_pieces
        else:
            list = board_.black_state.list_of_pieces 

        best_move = 0
        piece = 0

        for piece in list:
            if piece.array_of_legal_moves != []:
                list_of_pieces.append(piece)

        if list_of_pieces == []:
            return "stalemate"

        piece = choice(list_of_pieces)

        board_.piece_clicked = piece

        best_move = choice(piece.array_of_legal_moves)
        
        new_piece = 0

        if(piece.type == "p" and (best_move[0] == 7 or best_move[0] == 0)):
           _bishop = bishop(self.color)
           _rook = rook(self.color)
           _queen = queen(self.color)
           _knight = knight(self.color)
           list_of_pieces = [_bishop, _rook, _queen, _knight]

           new_piece = choice(list_of_pieces)
            
        return board_.play_a_move(best_move[0], best_move[1], new_piece)