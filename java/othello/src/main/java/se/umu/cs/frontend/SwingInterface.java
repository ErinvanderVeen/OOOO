package se.umu.cs.frontend;

import se.umu.cs.backend.core.Stone;

import javax.swing.*;
import javax.swing.border.CompoundBorder;
import javax.swing.border.EmptyBorder;
import javax.swing.border.LineBorder;
import java.awt.*;
import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class SwingInterface extends AbstractInterface {
    private JFrame frame;
    private JTextPane statusPane;
    private JTextPane transcript;
    private JPanel board;
    private JButton[][] cells;

    private Stone[][] buffer;

    private Lock lock;
    private Condition cv;
    private Move currentMove;

    public SwingInterface(int boardSize) {
        super(boardSize);

        buffer = new Stone[boardSize][boardSize];
        for (int y = 0; y < boardSize; y++) {
            for (int x = 0; x < boardSize; x++) {
                buffer[y][x] = Stone.INVALID;
            }
        }

        lock = new ReentrantLock();
        cv = lock.newCondition();

        frame = new JFrame("Othello");
        frame.setLayout(new BorderLayout());
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

        statusPane = new JTextPane();
        statusPane.setEditable(false);
        statusPane.setBorder(new CompoundBorder(new LineBorder(Color.DARK_GRAY), new EmptyBorder(4, 4, 4, 4)));
        frame.add(statusPane, BorderLayout.NORTH);

        transcript = new JTextPane();
        transcript.setEditable(false);
        transcript.setBorder(new CompoundBorder(new LineBorder(Color.DARK_GRAY), new EmptyBorder(4, 4, 4, 4)));
        frame.add(transcript, BorderLayout.SOUTH);

        board = new JPanel();
        board.setDoubleBuffered(true);
        board.setLayout(new GridLayout(boardSize + 1, boardSize + 1));
        frame.add(board, BorderLayout.CENTER);

        JButton b = new JButton();
        b.setEnabled(false);
        board.add(b);
        for (int x = 0; x < boardSize; x++) {
            b = new JButton((char) (x + 'A') + "");
            b.setEnabled(false);
            board.add(b);
        }

        cells = new JButton[boardSize + 1][boardSize + 1];
        for (int y = 0; y < boardSize; y++) {
            b = new JButton((y + 1) + "");
            b.setEnabled(false);
            board.add(b);
            for (int x = 0; x < boardSize; x++) {
                cells[y][x] = new JButton();
                int finalY = y;
                int finalX = x;
                cells[y][x].addActionListener(e -> {
                    lock.lock();
                    if (currentMove == null) {
                        currentMove = new Move();
                        currentMove.setX(finalX).setY(finalY);
                    }
                    cv.signal();
                    lock.unlock();
                });
                board.add(cells[y][x]);
            }
        }

        frame.setSize(600, 600);
        frame.setVisible(true);
    }

    @Override
    public Move getMoveFromPlayer() {
        Move move;
        lock.lock();
        while (currentMove == null) {
            try {
                cv.await();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
        move = currentMove;
        currentMove = null;
        lock.unlock();

        return move;
    }

    @Override
    public void drawGame() {
        int wScore = othello.getWhitePlayer().getScore();
        int bScore = othello.getBlackPlayer().getScore();
        int wWin = othello.getWhitePlayer().getWins();
        int bWin = othello.getBlackPlayer().getWins();
        int round = othello.getRound();
        Stone color = othello.getCurrentPlayer().getColor();

        statusPane.setText(String.format("Round: %02d White: %02d Black %02d Current: %s\t White wins: %d\tBlack wins: %d", round, wScore, bScore, color, wWin, bWin));
        transcript.setText(othello.getTranscript());

        for (int y = 0; y < boardSize; y++) {
            for (int x = 0; x < boardSize; x++) {
                if (buffer[x][y] == othello.getStonePositions()[y][x]) {
                    continue;
                } else {
                    buffer[x][y] = othello.getStonePositions()[y][x];
                }

                Stone current = buffer[x][y];
                switch (current) {
                    case BLACK:
                        cells[y][x].setBackground(Color.BLACK);
                        break;
                    case WHITE:
                        cells[y][x].setBackground(Color.WHITE);
                        break;
                    case UNUSED:
                        cells[y][x].setBackground(Color.GRAY);
                        break;
                    case VALID:
                        cells[y][x].setBackground(Color.GREEN);
                        break;
                    case INVALID:
                        break;
                }
            }
        }
    }

    @Override
    public boolean endGame() {
        drawGame();
        Object[] options = {"Yes", "No"};
        int n = JOptionPane.showOptionDialog(frame, "Would you like to play again?", "The game is over.",
                JOptionPane.YES_NO_CANCEL_OPTION, JOptionPane.QUESTION_MESSAGE, null, options, options[0]);
        return n != 0;
    }
}
