package se.umu.cs.backend.logic;

import se.umu.cs.backend.core.Stone;
import se.umu.cs.frontend.Move;

public interface AI {
    Move makeMove(Stone[][] board);
}
