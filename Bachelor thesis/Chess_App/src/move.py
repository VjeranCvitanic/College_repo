class move:
    def __init__(self, start_row, start_column, new_row, new_column, piece, captured_piece, promoted_piece, wsc = False, wlc = False, bsc = False, blc = False):
        self.start_pos = [start_row, start_column]
        self.new_pos = [new_row, new_column]
        self.piece = piece
        self.captured_piece = captured_piece
        self.promoted_piece = promoted_piece
        self.white_state_before_move = (wsc, wlc)
        self.black_state_before_move = (bsc, blc)

    def is_pawn_double_move(self):
        if self.piece.type == "p" and abs(self.start_pos[0] - self.new_pos[0]) == 2:
            return True
        return False