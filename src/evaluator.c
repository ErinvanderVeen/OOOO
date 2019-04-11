// Might not be enough, update
#define POSSIBLE_MOVES_MAX 32

#include <inttypes.h>
#include <locale.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "state_t.h"
#include "ai.h"
#include "debug.h"

typedef enum {Human, Computer} player_t;

player_t white = Human;
player_t black = Computer;

typedef enum {Black, White} color_t;

coordinate_t* possible_moves;
uint8_t nr_possible_moves;
uint64_t to_flip[8][8];

color_t turn;

bool finished;
bool white_skipped;
bool black_skipped;

uint64_t player_b;
uint64_t opponent_b;
uint64_t valid_moves;

void setup(void) {
	memset(to_flip, 0, 8*8*sizeof(uint64_t));
	turn = Black;

	finished = false;
	white_skipped = false;
	black_skipped = false;


	player_b = 0b0000000000000000000000000000100000010000000000000000000000000000;
	opponent_b = 0b0000000000000000000000000001000000001000000000000000000000000000;
	valid_moves = 0b0000000000000000000100000010000000000100000010000000000000000000;

	to_flip[2][3] = 0b0000000000000000000000000001000000000000000000000000000000000000;
	to_flip[3][2] = 0b0000000000000000000000000001000000000000000000000000000000000000;
	to_flip[4][5] = 0b0000000000000000000000000000000000001000000000000000000000000000;
	to_flip[5][4] = 0b0000000000000000000000000000000000001000000000000000000000000000;
	possible_moves = malloc(POSSIBLE_MOVES_MAX * sizeof(coordinate_t));
	possible_moves[0].column = 3;
	possible_moves[0].row = 2;
	possible_moves[1].column = 5;
	possible_moves[1].row = 4;
	possible_moves[2].column = 4;
	possible_moves[2].row = 5;
	possible_moves[3].column = 2;
	possible_moves[3].row = 3;
	nr_possible_moves = 4;

}

/**
 * Swiches the current player with its opponent
 */
void switch_players(void) {
	uint64_t temp = player_b;
	player_b = opponent_b;
	opponent_b = temp;

	if (turn == White)
		turn = Black;
	else
		turn = White;
}

coordinate_t random_turn(void) {
	uint8_t choice = (uint8_t) rand() % nr_possible_moves;
	return possible_moves[choice];
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

	coordinate_t choice;

	if ((turn == White && white == Human) || (turn == Black && black == Human)) {
		choice = random_turn();
	} else {
		uint8_t best_move = ai_turn(player_b, opponent_b);
		// TODO: See issue #10
		choice.column = best_move % 8;
		choice.row = best_move / 8;
	}

	do_move(&player_b, &opponent_b, choice.column, choice.row, to_flip);
}

int main(void) {
	// Print settings for unicode characters
	setlocale(LC_CTYPE, "");

	// For the AI
	srand(time(NULL));

	setup();

	uint8_t random_wins = 0;
	uint8_t ai_wins = 0;
	uint8_t games = 0;
	while (games++ < 10) {
		while (!finished) {
			if(turn == Black)
				print_state(player_b, opponent_b, valid_moves, false);
			else
				print_state(opponent_b, player_b, valid_moves, false);
			perform_turn();
			switch_players();
			update_valid_moves(player_b, opponent_b, &valid_moves, to_flip, possible_moves);
		}
		if (turn == White)
			count_pieces(player_b) > count_pieces(opponent_b) ? random_wins++ : ai_wins++;
		else
			count_pieces(player_b) < count_pieces(opponent_b) ? random_wins++ : ai_wins++;
		printf("GAME %d\n", games);
		printf("AI score:   %d\n", turn == Black ? count_pieces(player_b) : count_pieces(opponent_b));
		printf("AI winrate: %.2lf%%\n\n", ((double) ai_wins / (double) games) * 100);
		setup();
	}

	return 1;
}
