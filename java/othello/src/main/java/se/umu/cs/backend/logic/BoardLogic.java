package se.umu.cs.backend.logic;

import se.umu.cs.backend.core.Stone;

public class BoardLogic {
    private static int[][] delta = {
            {0, -1},    // NORTH
            {1, -1},    // NORTH-EAST
            {1, 0},     // EAST
            {1, 1},     // SOUTH-EAST
            {0, 1},     // SOUTH
            {-1, 1},    // SOUTH-WEST
            {-1, 0},    // WEST
            {-1, -1}    // NORTH-WEST
    };

    public static boolean validateMove(Stone[][] board, int x, int y) {
        return board[y][x] == Stone.VALID;
    }

    /**
     * Return true if at least one valid move exists on the board.
     */
    public static boolean availableMoves(Stone[][] board) {
        for (Stone[] columns : board) {
            for (Stone stone : columns) {
                if (stone == Stone.VALID) {
                    return true;
                }
            }
        }
        return false;
    }

    public static void performMove(Stone[][] board, Stone color, int x, int y) {
        int size = board.length;
        for (int[] d : delta) {
            int nrOfStonesToChange = 0;
            int xx = x + d[0];
            int yy = y + d[1];
            while (xx < size && xx >= 0 && yy < size && yy >= 0) {
                if (board[yy][xx] == color.opposite()) {
                    nrOfStonesToChange++;
                    xx = xx + d[0];
                    yy = yy + d[1];
                } else if (board[yy][xx] == color) {
                    break;
                } else {
                    nrOfStonesToChange = 0;
                    break;
                }
            }
            if (nrOfStonesToChange > 0) {
                if (xx < size && xx >= 0 && yy < size && yy >= 0) {
                    while (xx != x || yy != y) {
                        xx = xx - d[0];
                        yy = yy - d[1];
                        board[yy][xx] = color;
                    }
                }
            }
        }
    }

    public static void markValidMoves(Stone[][] board, Stone color) {
        removeOldValidMoves(board);

        int size = board.length;
        for (int y = 0; y < size; y++) {
            for (int x = 0; x < size; x++) {
                if (board[y][x] == color) {
                    for (int[] d : delta) {
                        int distance = 0;
                        int xx = x + d[0];
                        int yy = y + d[1];
                        while (xx < size && xx >= 0 && yy < size && yy >= 0) {
                            if (board[yy][xx] == color.opposite()) {
                                xx = xx + d[0];
                                yy = yy + d[1];
                                distance++;
                            } else if (board[yy][xx] == color) {
                                break;
                            } else if (board[yy][xx] == Stone.UNUSED && distance > 0) {
                                board[yy][xx] = Stone.VALID;
                                break;
                            } else {
                                break;
                            }
                        }
                    }
                }
            }
        }

    }

    private static void removeOldValidMoves(Stone[][] board) {
        int size = board.length;
        for (int y = 0; y < size; y++) {
            for (int x = 0; x < size; x++) {
                if (board[y][x] == Stone.VALID) {
                    board[y][x] = Stone.UNUSED;
                }
            }
        }
    }
}
