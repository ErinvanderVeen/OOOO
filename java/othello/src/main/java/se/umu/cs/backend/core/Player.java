package se.umu.cs.backend.core;

public class Player {
    private int wins;
    private int loses;
    private int score;

    private Stone color;

    public Player(Stone color) {
        this.color = color;
        wins = 0;
        loses = 0;
        score = 0;
    }

    public Stone getColor() {
        return color;
    }

    public void incrementWins() {
        wins++;
    }

    public void incrementLoses() {
        loses++;
    }

    public void incrementScore() {
        score++;
    }

    public void resetScore() {
        score = 0;
    }

    public int getScore() {
        return score;
    }

    public int getLoses() {
        return loses;
    }

    public int getWins() {
        return wins;
    }
}
