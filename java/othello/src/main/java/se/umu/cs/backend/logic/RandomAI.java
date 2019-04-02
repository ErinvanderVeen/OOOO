package se.umu.cs.backend.logic;

import se.umu.cs.backend.core.Stone;
import se.umu.cs.frontend.Move;

import java.util.ArrayList;
import java.util.List;
import java.util.Random;

public class RandomAI implements AI {
    @Override
    public Move makeMove(Stone[][] board) {
        List<Move> list = new ArrayList<>();
        for (int y = 0; y < board.length; y++) {
            for (int x = 0; x < board[y].length; x++) {
                if (board[y][x] == Stone.VALID) {
                    list.add(new Move().setX(x).setY(y));
                }
            }
        }
        int random;
        do {
            random = new Random().nextInt();
        } while (random < 0);
        return list.get(random % list.size());
    }
}
