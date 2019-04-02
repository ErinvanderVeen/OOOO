package se.umu.cs.frontend;

import se.umu.cs.backend.core.Stone;

import java.util.Scanner;

public class TerminalInterface extends AbstractInterface {

    public TerminalInterface(int boardSize) {
        super(boardSize);
    }

    @Override
    public Move getMoveFromPlayer() {
        Move move = new Move();
        Scanner sc = new Scanner(System.in);

        System.out.print("Enter coordinates (separated by whitespace): ");
        move.setX(sc.nextInt() - 1);
        move.setY(sc.nextInt() - 1);
        System.out.print("\r");

        return move;
    }

    @Override
    public void drawGame() {
        int i = 1;

        System.out.println();
        System.out.println();
        System.out.println();

        System.out.println("---------------------------------");
        System.out.println("| Round: " + othello.getRound());
        System.out.println("| Current player: " + othello.getCurrentPlayer().getColor());
        System.out.println("| White: " + othello.getWhitePlayer().getScore());
        System.out.println("| Black: " + othello.getBlackPlayer().getScore());
        System.out.println("---------------------------------");
        System.out.println("    1   2   3   4   5   6   7   8");
        for (Stone[] columns : othello.getStonePositions()) {
            System.out.print(i++ + " ");
            for (Stone stone : columns) {
                System.out.print("| ");
                switch (stone) {
                    case BLACK:
                        System.out.print("X ");
                        break;
                    case WHITE:
                        System.out.print("O ");
                        break;
                    case VALID:
                        System.out.print("* ");
                        break;
                    case UNUSED:
                    case INVALID:
                        System.out.print("  ");
                        break;
                }
            }
            System.out.println("|");
        }
    }

    @Override
    public boolean endGame() {
        return false;
    }
}
