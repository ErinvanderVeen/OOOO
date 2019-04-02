package se.umu.cs.backend.core;

public class Board {
    private int boardSize;
    private Stone[][] board;

    public Board(int boardSize) {
        this.boardSize = boardSize;
        board = new Stone[boardSize][boardSize];
        reset();
    }

    public void set(int x, int y, Stone color) {
        if (validatePosition(x, y)) {
            board[y][x] = color;
        }
    }

    public void reset() {
        for (int y = 0; y < boardSize; y++) {
            for (int x = 0; x < boardSize; x++) {
                board[y][x] = Stone.UNUSED;
            }
        }
    }

    private boolean validatePosition(int x, int y) {
        return x < boardSize && x >= 0 && y < boardSize && y >= 0;
    }

    public Stone[][] getBoard() {
        return board;
    }
}
