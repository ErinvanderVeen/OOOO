// Might not be enough, update
#define POSSIBLE_MOVES_MAX 32

#include <inttypes.h>
#include <locale.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "state_t.h"
#include "ai.h"
#include "debug.h"

uint64_t nodes_explored = 0;


typedef enum {Human, Computer} player_t;

player_t white = Computer;
player_t black = Computer;

typedef enum {Black, White} color_t;

uint8_t* possible_moves;
uint8_t nr_possible_moves;
uint64_t to_flip[64];

color_t turn = Black;

bool finished;
bool white_skipped;
bool black_skipped;

board_t board;
uint64_t valid_moves;

void switch_players(void) {
	uint64_t temp = board.player;
	board.player = board.opponent;
	board.opponent = temp;
}

void perform_turn(void) {
	if (!any_move_valid(valid_moves)) {
		if (turn == White)
			white_skipped = true;
		else
			black_skipped = true;
		finished = white_skipped && black_skipped;
		return;
	}

	if (turn == White)
        white_skipped = false;
    else
		black_skipped = false;
        
    uint8_t choice = ai_turn(board);
	do_move(&board, choice, to_flip);
}

void setup(void) {
    memset(to_flip, 0, 64 * sizeof(uint64_t));
    to_flip[19] = 0b0000000000000000000000000001000000000000000000000000000000000000;
	to_flip[26] = 0b0000000000000000000000000001000000000000000000000000000000000000;
	to_flip[37] = 0b0000000000000000000000000000000000001000000000000000000000000000;
	to_flip[44] = 0b0000000000000000000000000000000000001000000000000000000000000000;
	
    if(possible_moves == NULL) {
        possible_moves = malloc(POSSIBLE_MOVES_MAX * sizeof(possible_moves[0]));
    } else {
        memset(possible_moves, 0, POSSIBLE_MOVES_MAX * sizeof(possible_moves[0]));
    }

    possible_moves[0] = 19;
	possible_moves[1] = 26;
	possible_moves[2] = 37;
	possible_moves[3] = 44;

    board.player = 0b0000000000000000000000000000100000010000000000000000000000000000;
	board.opponent = 0b0000000000000000000000000001000000001000000000000000000000000000;
    valid_moves = 0b0000000000000000000100000010000000000100000010000000000000000000;

    finished = false;
    white_skipped = false;
    black_skipped = false;
}

int main(void) {
	setlocale(LC_CTYPE, "");
    srand(time(NULL));

    uint8_t LIMIT = 10;

    uint64_t opponent_wins = 0;
    uint64_t ai_wins        = 0;

    uint64_t games_played   = 0;
    uint64_t moves_played   = 0;

    time_t last = time(NULL);
    while(time(NULL) - last < LIMIT){
        setup();
        while (!finished && (time(NULL) - last < LIMIT)) {
            perform_turn();
            switch_players();
            if (turn == White)
                turn = Black;
            else
                turn = White;
            update_valid_moves(board, &valid_moves, to_flip, possible_moves);
            moves_played++;
        }
        games_played++;

        if (turn == White)
			count_pieces(board.opponent) > count_pieces(board.player) ? opponent_wins++ : ai_wins++;
		else
            count_pieces(board.opponent) < count_pieces(board.player) ? opponent_wins++ : ai_wins++;
    }

    printf("Games/s:\t%ld\n", games_played / LIMIT);
    printf("Moves/s:\t%ld\n", moves_played / LIMIT);
    printf("Nodes/s:\t%.2f M\n", (double) nodes_explored / LIMIT / 1000000);
    printf("AI winrate:\t%.2f%%\n", (((double) ai_wins) / games_played) * 100);
	return 0;
}
