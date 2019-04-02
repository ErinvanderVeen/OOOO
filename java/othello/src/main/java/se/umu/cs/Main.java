package se.umu.cs;

import se.umu.cs.frontend.AbstractInterface;
import se.umu.cs.frontend.SwingInterface;

public class Main {
    public static void main(String[] args) {
        AbstractInterface game = new SwingInterface(8);
        game.startGame();
    }
}
