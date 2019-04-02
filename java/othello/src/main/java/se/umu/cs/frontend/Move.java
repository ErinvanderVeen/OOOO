package se.umu.cs.frontend;

public class Move {
    private int x;
    private int y;

    public int getY() {
        return y;
    }

    public Move setY(int y) {
        this.y = y;
        return this;
    }

    public int getX() {
        return x;
    }

    public Move setX(int x) {
        this.x = x;
        return this;
    }
}
