#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>

#define BOARD_SIZE 9

typedef struct {
    char board[BOARD_SIZE];
    char current_player;
} TicTacToe;

void init_game(TicTacToe* game) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        game->board[i] = ' ';
    }
    game->current_player = 'X';
}

int make_move(TicTacToe* game, int position) {
    if (game->board[position] == ' ') {
        game->board[position] = game->current_player;
        game->current_player = (game->current_player == 'X') ? 'O' : 'X';
        return 1;
    }
    return 0;
}

void get_available_moves(TicTacToe* game, int* moves, int* count) {
    *count = 0;
    for (int i = 0; i < BOARD_SIZE; i++) {
        if (game->board[i] == ' ') {
            moves[(*count)++] = i;
        }
    }
}

int is_winner(TicTacToe* game, char player) {
    int win_conditions[8][3] = {
        {0, 1, 2}, {3, 4, 5}, {6, 7, 8},  // Rows
        {0, 3, 6}, {1, 4, 7}, {2, 5, 8},  // Columns
        {0, 4, 8}, {2, 4, 6}              // Diagonals
    };
    for (int i = 0; i < 8; i++) {
        if (game->board[win_conditions[i][0]] == player &&
            game->board[win_conditions[i][1]] == player &&
            game->board[win_conditions[i][2]] == player) {
            return 1;
        }
    }
    return 0;
}

int is_draw(TicTacToe* game) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        if (game->board[i] == ' ') {
            return 0;
        }
    }
    return !is_winner(game, 'X') && !is_winner(game, 'O');
}

void clone_game(TicTacToe* src, TicTacToe* dest) {
    memcpy(dest, src, sizeof(TicTacToe));
}

void display_board(TicTacToe* game) {
    for (int i = 0; i < 3; i++) {
        printf("%c|%c|%c\n", game->board[3 * i], game->board[3 * i + 1], game->board[3 * i + 2]);
        if (i < 2) {
            printf("-+-+-\n");
        }
    }
}

char get_game_result(TicTacToe* game) {
    if (is_winner(game, 'X')) {
        return 'X';
    } else if (is_winner(game, 'O')) {
        return 'O';
    } else if (is_draw(game)) {
        return 'D';
    }
    return ' ';  // Ongoing game
}

float random_rollout(TicTacToe* game) {
    TicTacToe cloned_game;
    clone_game(game, &cloned_game);
    int moves[BOARD_SIZE], move_count;
    while (get_game_result(&cloned_game) == ' ') {
        get_available_moves(&cloned_game, moves, &move_count);
        int move = moves[rand() % move_count];
        make_move(&cloned_game, move);
    }
    char result = get_game_result(&cloned_game);
    if (result == 'X') {
        return 1.0;
    } else if (result == 'O') {
        return -1.0;
    } else {
        return 0.0;
    }
}

float nmcs_search_with_rollout_recursive(TicTacToe* game, int depth) {
    if (depth == 0) {
        return random_rollout(game);
    }
    char result = get_game_result(game);
    if (result != ' ') {
        if (result == 'X') {
            return 1.0;
        } else if (result == 'O') {
            return -1.0;
        } else {
            return 0.0;
        }
    }
    float best_score = -INFINITY;
    int moves[BOARD_SIZE], move_count;
    get_available_moves(game, moves, &move_count);
    for (int i = 0; i < move_count; i++) {
        TicTacToe cloned_game;
        clone_game(game, &cloned_game);
        make_move(&cloned_game, moves[i]);
        float score = -nmcs_search_with_rollout_recursive(&cloned_game, depth - 1);
        if (score > best_score) {
            best_score = score;
        }
    }
    return best_score;
}

int select_best_move(TicTacToe* game, int depth) {
    int best_move = -1;
    float best_score = -INFINITY;
    int moves[BOARD_SIZE], move_count;
    get_available_moves(game, moves, &move_count);
    for (int i = 0; i < move_count; i++) {
        TicTacToe cloned_game;
        clone_game(game, &cloned_game);
        make_move(&cloned_game, moves[i]);
        float score = -nmcs_search_with_rollout_recursive(&cloned_game, depth - 1);
        if (score > best_score) {
            best_score = score;
            best_move = moves[i];
        }
    }
    return best_move;
}

void play_game() {
    TicTacToe game;
    init_game(&game);
    int depth = 3;
    while (1) {
        display_board(&game);
        char result = get_game_result(&game);
        if (result != ' ') {
            if (result == 'D') {
                printf("It's a draw!\n");
            } else {
                printf("Player %c wins!\n", result);
            }
            break;
        }
        if (game.current_player == 'X') {
            int move = -1;
            while (move < 0 || move >= BOARD_SIZE || game.board[move] != ' ') {
                printf("Enter your move (0-8): ");
                scanf("%d", &move);
                if (move < 0 || move >= BOARD_SIZE || game.board[move] != ' ') {
                    printf("Invalid move. Try again.\n");
                }
            }
            make_move(&game, move);
        } else {
            printf("AI is thinking...\n");
            int move = select_best_move(&game, depth);
            make_move(&game, move);
            printf("AI selects move %d\n", move);
        }
    }
}

int main() {
    srand(time(NULL));
    play_game();
    return 0;
}