package se.umu.cs.backend;

import se.umu.cs.backend.core.Board;
import se.umu.cs.backend.core.Player;
import se.umu.cs.backend.core.Stone;
import se.umu.cs.backend.logic.AI;
import se.umu.cs.backend.logic.BoardLogic;
import se.umu.cs.backend.logic.RandomAI;
import se.umu.cs.frontend.Move;

public class Othello {
    private Board board;
    private int boardSize;
    private int currentRound;
    private int skips;

    private boolean isOver;
    private boolean useAI;

    private Player blackPlayer;
    private Player whitePlayer;
    private Player currentPlayer;

    private AI ai;

    private String transcript;

    public Othello(int boardSize, boolean useAI) {
        this.boardSize = boardSize;
        this.useAI = useAI;
        if (useAI) {
            ai = new RandomAI();
        }

        board = new Board(boardSize);
        blackPlayer = new Player(Stone.BLACK);
        whitePlayer = new Player(Stone.WHITE);
    }

    public void newGame() {
        transcript = "";
        currentRound = 1;
        skips = 0;
        isOver = false;

        currentPlayer = blackPlayer;
        blackPlayer.resetScore();
        whitePlayer.resetScore();

        board.reset();
        board.set(boardSize / 2 - 1, boardSize / 2 - 1, Stone.WHITE);
        board.set(boardSize / 2, boardSize / 2, Stone.WHITE);
        board.set(boardSize / 2 - 1, boardSize / 2, Stone.BLACK);
        board.set(boardSize / 2, boardSize / 2 - 1, Stone.BLACK);

        recalculateScores();
        BoardLogic.markValidMoves(board.getBoard(), currentPlayer.getColor());
        nextMove(-1, -1);
    }

    public boolean isOver() {
        if (isOver || currentRound > boardSize * boardSize - 4) {
            setWinnerLoser();
            return true;
        }
        return false;
    }

    public Stone[][] getStonePositions() {
        return board.getBoard();
    }

    public Player getBlackPlayer() {
        return blackPlayer;
    }

    public Player getWhitePlayer() {
        return whitePlayer;
    }

    public Player getCurrentPlayer() {
        return currentPlayer;
    }

    public void nextMove(int x, int y) {
        if (!isOver()) {
            if (!BoardLogic.availableMoves(board.getBoard())) {
                skips++;
                if (skips > 1) {
                    isOver = true;
                    return;
                }
                currentRound++;
                currentPlayer = (currentPlayer.getColor() == Stone.BLACK) ? whitePlayer : blackPlayer;
                BoardLogic.markValidMoves(board.getBoard(), currentPlayer.getColor());
                return;
            }
            skips = 0;
            if (currentPlayer.getColor() == Stone.BLACK) {
                BoardLogic.markValidMoves(board.getBoard(), currentPlayer.getColor());
                Move move = ai.makeMove(board.getBoard());
                x = move.getX();
                y = move.getY();
            }
            if (BoardLogic.validateMove(board.getBoard(), x, y)) {
                board.set(x, y, currentPlayer.getColor());
                BoardLogic.performMove(board.getBoard(), currentPlayer.getColor(), x, y);
                recalculateScores();
                addToTranscript(x, y);
                currentRound++;
                currentPlayer = (currentPlayer.getColor() == Stone.BLACK) ? whitePlayer : blackPlayer;
            }
            BoardLogic.markValidMoves(board.getBoard(), currentPlayer.getColor());
        } else {
            isOver = true;
        }
    }

    private void setWinnerLoser() {
        recalculateScores();
        if (blackPlayer.getScore() > whitePlayer.getScore()) {
            blackPlayer.incrementWins();
            whitePlayer.incrementLoses();
        } else if (blackPlayer.getScore() < whitePlayer.getScore()) {
            whitePlayer.incrementWins();
            blackPlayer.incrementLoses();
        } else {
            blackPlayer.incrementWins();
            whitePlayer.incrementWins();
        }
    }

    private void addToTranscript(int x, int y) {
        transcript += (char) ('A' + x) + "" + (y + 1);
    }

    private void recalculateScores() {
        blackPlayer.resetScore();
        whitePlayer.resetScore();
        for (Stone[] stones : board.getBoard()) {
            for (Stone stone : stones) {
                if (stone == Stone.BLACK) {
                    blackPlayer.incrementScore();
                } else if (stone == Stone.WHITE) {
                    whitePlayer.incrementScore();
                }
            }
        }
    }

    public int getRound() {
        return currentRound;
    }

    public String getTranscript() {
        return transcript;
    }
}
