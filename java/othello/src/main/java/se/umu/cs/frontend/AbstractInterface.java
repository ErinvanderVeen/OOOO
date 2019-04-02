package se.umu.cs.frontend;

import se.umu.cs.backend.Othello;
import se.umu.cs.backend.core.Stone;

public abstract class AbstractInterface {
    protected Othello othello;
    protected int boardSize;

    AbstractInterface(int boardSize) {
        this.boardSize = boardSize;
    }

    public void startGame() {
        othello = new Othello(boardSize, true);
        othello.newGame();
        do {
            drawGame();
            othello.newGame();
            drawGame();
            gameLoop();
        } while (!endGame());
    }

    private void gameLoop() {
        while (!othello.isOver()) {
            drawGame();
            if (othello.getCurrentPlayer().getColor() != Stone.BLACK) {
                Move move = getMoveFromPlayer();
                if (move.getX() < boardSize && move.getX() >= 0 && move.getY() < boardSize && move.getY() >= 0) {
                    othello.nextMove(move.getX(), move.getY());
                }
            } else {
                othello.nextMove(-1, -1);
            }
        }
    }

    public abstract Move getMoveFromPlayer();

    public abstract void drawGame();

    public abstract boolean endGame();
}
