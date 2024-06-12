from src.settings import *
from src.draw import *
from src.BoardModule import board
from src.bot import *
from src.piece import pawn

def bot_vs_bot():
    white_wins = 0
    black_wins = 0
    draws = 0
    num_of_games = 0
    while num_of_games < 25:
        num_of_games += 1
        print(num_of_games, sep=' ', end='', flush=True)
        print("\n")
        Board = board()

        bot_w = bot_alphabeta("w", 3)
        bot_b = bot_alphabeta("b", 4)

        #bot_w = bot_minimax("w", 4)
        #bot_b = bot_minimax("b", 4)

        #bot_w = bot_random("w")
        #bot_b = bot_random("b")

        Board.calculate_all_legal_moves_for_one_player("w")

        num_of_moves = 0

        while True:
            result = False
            if Board.white_to_play:
                num_of_moves += 1
                print(num_of_moves, sep=' ', end='', flush=True)
                result = bot_w.move(Board)
                last_move = Board.peek_last_move()
                if last_move.piece == "p" or last_move.captured_piece != 0:
                    Board.fifty_moves_rule = 0
                else:
                    Board.fifty_moves_rule += 1
                Board.set_is_endgame()
                Board.white_to_play = False
            else:
                num_of_moves += 1
                print(num_of_moves, sep=' ', end='', flush=True)
                result = bot_b.move(Board)
                if last_move.piece == "p" or last_move.captured_piece != 0:
                    Board.fifty_moves_rule = 0
                Board.set_is_endgame()
                Board.white_to_play = True

            if result == "draw":
                print("Draw by insufficient material!")
                draws += 1
                break
            if Board.fifty_moves_rule >= 50:
                print("Draw by 50 move rule!")
                draws += 1
                break
            if result == "checkmate":
                print("Checkmate", ", winner is ")
                if Board.white_to_play:
                    black_wins += 1
                    #print("BLACK")
                else:
                    white_wins += 1
                    #print("WHITE")
                break
            if result == "stalemate":
                draws += 1
                print("Stalemate")
                break

    print("WHITE: ", white_wins, "\nDRAWS: ", draws, "\nBLACK: ", black_wins)

def vs_bot():
    Board = board()
    screen = pygame.display.set_mode((width, height))

    bot_color = "b"
    player_color = switch_color(bot_color)

    is_bot_to_move = False
    if bot_color == "w":
        is_bot_to_move = True

    #bot = bot_alphabeta(bot_color)
    bot = bot_minimax(bot_color)
    #bot = bot_random(bot_color)

    draw(screen, Board)


    right_click_start_position = (0, 0)
    right_click_end_position = (0, 0)

    Board.calculate_all_legal_moves_for_one_player("w")

    while True:
        result = False
        if not is_bot_to_move:
            for event in pygame.event.get():
                if event.type == QUIT:
                    return
                if event.type == pygame.MOUSEBUTTONDOWN:
                    if event.button == 1:
                        row, column = screen_position_to_board_position(pygame.mouse.get_pos()[0], pygame.mouse.get_pos()[1])
                        if Board.piece_clicked != 0:
                            if (row, column) in Board.piece_clicked.array_of_legal_moves:
                                result = Board.play_a_move(row, column, 0)
                                Board.set_is_endgame()
                                Board.reset_arrays_of_legal_moves()
                                Board.calculate_all_legal_moves_for_one_player(Board.color_to_play())
                                draw(screen, Board)
                                is_bot_to_move = True
                            else:
                                if is_row_and_column_in_range(row, column) and Board.array[row][column] != 0 and Board.array[row][column].color == Board.piece_clicked.color:
                                    if Board.piece_clicked == Board.array[row][column]:
                                        Board.piece_clicked = 0
                                        draw_board(screen)
                                        draw_pieces(screen, Board)   
                                    else:
                                        draw_board(screen)
                                        draw_pieces(screen, Board)
                                        Board.piece_clicked = Board.array[row][column]
                                        draw_legal_moves(screen, Board.piece_clicked)
                                else:
                                    Board.piece_clicked = 0
                                    draw_board(screen)
                                    draw_pieces(screen, Board)            
                        else:
                            draw_board(screen)
                            draw_pieces(screen, Board)
                            if is_row_and_column_in_range(row, column) and Board.array[row][column] != 0:
                                if Board.color_to_play() == Board.array[row][column].color:
                                    Board.piece_clicked = Board.array[row][column]
                                    draw_legal_moves(screen, Board.piece_clicked)

                    elif event.button == 3:
                        Board.piece_clicked = 0
                        draw_board(screen)
                        draw_pieces(screen, Board)
                        right_click_start_position = pygame.mouse.get_pos()

                elif event.type == pygame.MOUSEBUTTONUP:
                    if event.button == 3:
                        Board.unmake_move()
                        if Board.stack_of_played_moves != []:
                            is_bot_to_move = not is_bot_to_move
                        Board.reset_arrays_of_legal_moves()
                        Board.calculate_all_legal_moves_for_one_player(Board.color_to_play())
                        draw(screen, Board)
                        right_click_end_position = pygame.mouse.get_pos()
                        draw_arrow(screen, right_click_start_position, right_click_end_position)

        else:
            result = bot.move(Board)
            Board.set_is_endgame()
            Board.reset_arrays_of_legal_moves()
            Board.calculate_all_legal_moves_for_one_player(Board.color_to_play())
            draw(screen, Board)
            is_bot_to_move = False

        draw_toolbar(screen)
        draw_piece_value(screen, Board.white_state.pieces_value, Board.black_state.pieces_value)
            
        pygame.display.flip()

        if result == "checkmate":
            messagebox.showinfo(title="Checkmate", message="Checkmate")
            return
        if result == "draw":
            messagebox.showinfo(title="Draw", message="Draw by insufficient material!")
            return
        if result == "stalemate":
            messagebox.showinfo(title="Stalemate", message="Stalemate")
            return

def time_test_bots():
    Board = board()
    bot_w = bot_alphabeta("w", 4)
    Board.calculate_all_legal_moves_for_one_player("w")
    start_time = time()
    bot_w.move(Board)
    end_time = time()
    print("Time alphabeta: ", end_time - start_time)
    print("Moves alphabeta: ", bot_w.number_of_searches)

    Board = board()
    bot_w = bot_minimax("w", 4)
    Board.calculate_all_legal_moves_for_one_player("w")
    start_time = time()
    bot_w.move(Board)
    end_time = time()
    print("Time minmax: ", end_time - start_time)
    print("Moves minmax: ", bot_w.number_of_searches)

if __name__ == "__main__":
    time_test_bots()
    #bot_vs_bot()
    #vs_bot()
