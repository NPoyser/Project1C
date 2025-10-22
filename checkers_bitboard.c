#define _CRT_SECURE_NO_WARNINGS
#define MAX_UNDO 10
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include "c.h"




typedef uint64_t U64;

U64 SetBit(U64 value, int pos) { return value | (1ULL << pos); }
U64 ClearBit(U64 value, int pos) { return value & ~(1ULL << pos); }
int GetBit(U64 value, int pos) { return (value >> pos) & 1ULL; }

typedef struct {
    U64 red_pieces;
    U64 black_pieces;
    U64 red_kings;
    U64 black_kings;
    int red_turn; // 1 = Red, 0 = Black
    int force_capture;
} GameState;



typedef struct {
    GameState states[MAX_UNDO];
    int top;
} UndoStack;

void PushState(UndoStack* stack, GameState* g) {
    if (stack->top < MAX_UNDO - 1) {
        stack->top++;
        stack->states[stack->top] = *g;
    }
}

int PopState(UndoStack* stack, GameState* g) {
    if (stack->top >= 0) {
        *g = stack->states[stack->top];
        stack->top--;
        return 1;
    }
    return 0; // nothing to undo
}

void InitUndo(UndoStack* stack) {
    stack->top = -1;
}

//Initializing the board
void InitBoard(GameState* g) {
    g->red_pieces = 0;
    g->black_pieces = 0;
    g->red_kings = 0;
    g->black_kings = 0;
    g->red_turn = 1;

    g->red_pieces = (1ULL << 1) | (1ULL << 3) | (1ULL << 5) | (1ULL << 7)
        | (1ULL << 8) | (1ULL << 10) | (1ULL << 12) | (1ULL << 14);

    g->black_pieces = (1ULL << 49) | (1ULL << 51) | (1ULL << 53) | (1ULL << 55)
        | (1ULL << 56) | (1ULL << 58) | (1ULL << 60) | (1ULL << 62);
}



int BoardToIndex(char file, int rank) {
    file = toupper(file);
    int col = file - 'A';
    int row = rank - 1;

    if (col < 0 || col > 7 || row < 0 || row > 7) {
        return -1; // invalid index
    }

    return row * 8 + col;
}

int CheckWin(GameState* g) {
    if (g->red_pieces == 0 && g->red_kings == 0) {
        printf("Black wins!\n");
        return 1;
    }
    if (g->black_pieces == 0 && g->black_kings == 0) {
        printf("Red wins!\n");
        return 1;
    }
    return 0;
}

U64 GetLegalMoves(GameState* g, int pos) {
    U64 all = g->red_pieces | g->black_pieces | g->red_kings | g->black_kings;
    U64 opponent = g->red_turn ? (g->black_pieces | g->black_kings) : (g->red_pieces | g->red_kings);
    U64 legal = 0;



    int is_red = g->red_turn;
    int is_king = GetBit(is_red ? g->red_kings : g->black_kings, pos);
    int row = pos / 8;
    int col = pos % 8;

    // Directions: up-left, up-right, down-left, down-right
    int dirs[4][2] = { {1, -1}, {1, 1}, {-1, -1}, {-1, 1} };

    for (int i = 0; i < 4; i++) {
        int dr = dirs[i][0], dc = dirs[i][1];

        // Non-king pieces can only move forward
        if (!is_king && ((is_red && dr < 0) || (!is_red && dr > 0))) continue;

        int r1 = row + dr;
        int c1 = col + dc;
        int to1 = r1 * 8 + c1;

        if (r1 >= 0 && r1 < 8 && c1 >= 0 && c1 < 8) {
            if (!GetBit(all, to1)) {
                legal = SetBit(legal, to1); // normal move
            }
            else if (GetBit(opponent, to1)) {
                // Check jump over
                int r2 = r1 + dr;
                int c2 = c1 + dc;
                int to2 = r2 * 8 + c2;
                if (r2 >= 0 && r2 < 8 && c2 >= 0 && c2 < 8 && !GetBit(all, to2)) {
                    legal = SetBit(legal, to2); // capture move
                }
            }
        }
    }

    return legal;
}

int AnyCaptureAvailable(GameState* g) {
    U64 pieces = g->red_turn ? (g->red_pieces | g->red_kings)
        : (g->black_pieces | g->black_kings);

    for (int i = 0; i < 64; i++) {
        if (GetBit(pieces, i)) {
            U64 moves = GetLegalMoves(g, i);
            for (int j = 0; j < 64; j++) {
                if (GetBit(moves, j)) {
                    int row_diff = abs((j / 8) - (i / 8));
                    int col_diff = abs((j % 8) - (i % 8));
                    if (row_diff == 2 && col_diff == 2) {
                        int mid_row = (i / 8 + j / 8) / 2;
                        int mid_col = (i % 8 + j % 8) / 2;
                        int mid = mid_row * 8 + mid_col;
                        if (GetBit(g->red_turn? (g->black_pieces | g->black_kings): (g->red_pieces | g->red_kings), mid)) {
                            return 1; // real capture available
                        }
                    }
                }
            }
        }
    }
    return 0;
}
int PieceCanCapture(GameState* g, int index) {
    U64 moves = GetLegalMoves(g, index);
    for (int j = 0; j < 64; j++) {
        if (GetBit(moves, j)) {
            int row_diff = abs((j / 8) - (index / 8));
            int col_diff = abs((j % 8) - (index % 8));
            if (row_diff == 2 && col_diff == 2) {
                int mid_row = (index / 8 + j / 8) / 2;
                int mid_col = (index % 8 + j % 8) / 2;
                int mid = mid_row * 8 + mid_col;
                if (GetBit(g->red_turn ? g->black_pieces | g->black_kings : g->red_pieces | g->red_kings, mid))
                    return 1;
            }
        }
    }
    return 0;
}

int MovePiece(GameState* g, int from, int to) {
    U64* player_pieces = g->red_turn ? &g->red_pieces : &g->black_pieces;
    U64* opponent_pieces = g->red_turn ? &g->black_pieces : &g->red_pieces;
    U64* player_kings = g->red_turn ? &g->red_kings : &g->black_kings;
    U64* opponent_kings = g->red_turn ? &g->black_kings : &g->red_kings;

    if (!GetBit(*player_pieces | *player_kings, from)) {
        printf("No piece at that place!\n");
        return 0;
    }

    int is_king = GetBit(*player_kings, from);
    int from_row = from / 8, from_col = from % 8;
    int to_row = to / 8, to_col = to % 8;
    int row_diff = to_row - from_row;
    int col_diff = to_col - from_col;

    U64 all = g->red_pieces | g->red_kings | g->black_pieces | g->black_kings;

    if (abs(row_diff) == 1) { // normal move
        if (AnyCaptureAvailable(g)) {
            printf("You must capture!\n");
            return 0;
        }
    }


    int abs_row = abs(row_diff);
    int abs_col = abs(col_diff);

    if (abs_row != abs_col) {
        printf("Illegal move (must be diagonal).\n");
        return 0;
    }

    if (abs_row > 2 || abs_row == 0) {
        printf("Illegal move distance!\n");
        return 0;
    }

    // Capture move
    if (abs(row_diff) == 2) {
        int mid_row = (from_row + to_row) / 2;
        int mid_col = (from_col + to_col) / 2;
        int mid = mid_row * 8 + mid_col;

        if (!GetBit(*opponent_pieces | *opponent_kings, mid)) {
            printf("No enemy to capture!\n");
            return 0;
        }

        // Capture piece & print message
        if (GetBit(*opponent_pieces, mid)) {
            printf("Captured a %s mortalic piece!\n", g->red_turn ? "black" : "red");
            *opponent_pieces = ClearBit(*opponent_pieces, mid);
        }
        else if (GetBit(*opponent_kings, mid)) {
            printf("Captured a %s Immortal king!\n", g->red_turn ? "black" : "red");
            *opponent_kings = ClearBit(*opponent_kings, mid);
        }

        // Check for win immediately
        if ((*opponent_pieces | *opponent_kings) == 0) {
            printf("%s wins!\n", g->red_turn ? "Red" : "Black");
            return 2; // special code for game over
        }
    }
    // Normal move
    else if (abs(row_diff) == 1) {
        if (!is_king) {
            if (g->red_turn && row_diff < 0) {
                printf("Red can't move backward!\n");
                return 0;
            }
            if (!g->red_turn && row_diff > 0) {
                printf("Black can't move backward!\n");
                return 0;
            }
        }
        if (GetBit(all, to)) {
            printf("Destination occupied!\n");
            return 0;
        }
    }
    

    // Move the piece
    if (is_king) {
        *player_kings = ClearBit(*player_kings, from);
        *player_kings = SetBit(*player_kings, to);
    }
    else {
        *player_pieces = ClearBit(*player_pieces, from);
        *player_pieces = SetBit(*player_pieces, to);

        // Promotion from a mortal to immortal hehe
        if (g->red_turn && to_row == 7) {
            *player_pieces = ClearBit(*player_pieces, to);
            *player_kings = SetBit(*player_kings, to);
            printf("Red piece crowned as Immortal King!\n");
        }
        else if (!g->red_turn && to_row == 0) {
            *player_pieces = ClearBit(*player_pieces, to);
            *player_kings = SetBit(*player_kings, to);
            printf("Black piece crowned as Immortal King!\n");
        }
    }

    // Only switches turn if both sides still have pieces
    if ((g->red_pieces | g->red_kings) != 0 && (g->black_pieces | g->black_kings) != 0) {
        g->red_turn = !g->red_turn;
    }
    return 1;
}


// Display boards
void PrintBoardFull(GameState* g) {
    printf("\n    A B C D E F G H\n");
    printf("   -----------------\n");

    U64 highlight = 0;
    U64 pieces = g->red_turn ? g->red_pieces | g->red_kings : g->black_pieces | g->black_kings;
    for (int i = 0; i < 64; i++) {
        if (GetBit(pieces, i)) highlight |= GetLegalMoves(g, i);
    }

    for (int row = 7; row >= 0; row--) {
        printf("%d | ", row + 1);
        for (int col = 0; col < 8; col++) {
            int pos = row * 8 + col;
            char piece;

            if ((row + col) % 2 == 1) { // dark square
                if (GetBit(g->red_kings, pos)) piece = 'R';
                else if (GetBit(g->black_kings, pos)) piece = 'B';
                else if (GetBit(g->red_pieces, pos)) piece = 'r';
                else if (GetBit(g->black_pieces, pos)) piece = 'b';
                else if (GetBit(highlight, pos)) piece = '*';
                else piece = '.';
            }
            else { // light square
                piece = ' '; // show a symbol to make the grid look full
            }

            printf("%c ", piece);
        }
        printf("\n");
    }
    printf("   -----------------\n");
    printf("Turn: %s\n", g->red_turn ? "Red" : "Black");
}
void PrintBoardWithHighlight(GameState* g, int from_pos) {
    printf("\n    A B C D E F G H\n");
    printf("   -----------------\n");

    U64 highlight = 0;
    if (from_pos >= 0 && from_pos <= 63)
        highlight = GetLegalMoves(g, from_pos);

    for (int row = 7; row >= 0; row--) {
        printf("%d | ", row + 1);
        for (int col = 0; col < 8; col++) {
            int pos = row * 8 + col;
            char piece;

            if ((row + col) % 2 == 1) { // dark square
                if (GetBit(g->red_kings, pos)) piece = 'R';
                else if (GetBit(g->black_kings, pos)) piece = 'B';
                else if (GetBit(g->red_pieces, pos)) piece = 'r';
                else if (GetBit(g->black_pieces, pos)) piece = 'b';
                else if (GetBit(highlight, pos)) piece = '*';
                else piece = '.';
            }
            else { // light square
                piece = ' ';
            }

            printf("%c ", piece);
        }
        printf("\n");
    }
    printf("   -----------------\n");
    printf("Turn: %s\n", g->red_turn ? "Red" : "Black");
}

//extra save feature
void SaveGame(GameState* g, const char* filename) {
    FILE* f = fopen(filename, "wb");
    if (!f) {
        printf("Failed to save to '%s'\n", filename);
        return;
    }
    fwrite(g, sizeof(GameState), 1, f);
    fclose(f);
    printf("Game saved to %s\n", filename);
}
//extra load feature
void LoadGame(GameState* g, const char* filename) {
    FILE* f = fopen(filename, "rb");
    if (!f) {
        printf("Failed to load from '%s'\n", filename);
        return;
    }
    fread(g, sizeof(GameState), 1, f);
    fclose(f);
    printf("Loading...\n");
    PrintBoardFull(g);  // print the board immediately after loading
}

//a silly ai who makes terrific moves
void sillyAI(GameState* g) {
    srand((unsigned int)time(NULL));

    int original_turn = g->red_turn;
    g->red_turn = 0; // AI is Black

    // Build a list of all pieces with legal moves
    int pieces[12]; 
    int piece_count = 0;
    int captures_exist = AnyCaptureAvailable(g);

    for (int i = 0; i < 64; i++) {
        if (GetBit(g->black_pieces | g->black_kings, i)) {
            if (captures_exist) {
                if (PieceCanCapture(g, i)) {
                    pieces[piece_count++] = i;
                }
            }
            else {
                if (GetLegalMoves(g, i) != 0) {
                    pieces[piece_count++] = i;
                }
            }
        }
    }

    if (piece_count == 0) {
        printf("AI has no legal moves!\n");
        g->red_turn = original_turn;
        return;
    }

    // Pick a random piece
    int piece_index = pieces[rand() % piece_count];
    U64 moves = GetLegalMoves(g, piece_index);

    // Build a list of destinations
    int dests[4]; 
    int dest_count = 0;
    for (int i = 0; i < 64; i++) {
        if (GetBit(moves, i)) {
            if (captures_exist) {
                int row_diff = abs((i / 8) - (piece_index / 8));
                int col_diff = abs((i % 8) - (piece_index % 8));
                if (row_diff == 2 && col_diff == 2) { // only capture moves
                    dests[dest_count++] = i;
                }
            }
            else {
                dests[dest_count++] = i;
            }
        }
    }

    if (dest_count == 0) {
        printf("AI has no legal moves!\n");
        g->red_turn = original_turn;
        return;
    }

    // Pick a random destination
    int to_index = dests[rand() % dest_count];

    // Make the move
    printf("AI moves from %c%d to %c%d\n",
        'A' + piece_index % 8, piece_index / 8 + 1,
        'A' + to_index % 8, to_index / 8 + 1);

    MovePiece(g, piece_index, to_index);
}


int main() {
    GameState game;
    UndoStack undo_stack;
    InitBoard(&game);
    InitUndo(&undo_stack);


    int playAgainstAI = 0;
    char choice;
    printf("Play against AI? (y/n): ");
    scanf(" %c", &choice);
    getchar(); // consume leftover newline
    if (choice == 'y' || choice == 'Y') playAgainstAI = 1;

    if (playAgainstAI && !game.red_turn) {
        printf("AI is thinking...\n");
        sillyAI(&game);
    }

    int from = -1, to;
    char from_file, to_file;
    int from_rank, to_rank;
    while (1) {
        PrintBoardFull(&game);

        char line[16];

        // Piece selection
        while (1) {
            printf("Select a piece to move (e.g., A2, or 'quit', 'save', 'load'): ");
            if (!fgets(line, sizeof(line), stdin)) continue;
            if (strncmp(line, "quit", 4) == 0) exit(0);

            // Save
            if (strncmp(line, "save", 4) == 0) {
                char filename[64];
                printf("Enter a name to save: ");
                if (!fgets(filename, sizeof(filename), stdin)) continue;
                filename[strcspn(filename, "\n")] = 0; // remove newline
                SaveGame(&game, filename);
                continue;
            }

            // Load
            if (strncmp(line, "load", 4) == 0) {
                char filename[64];
                printf("Enter the saved name to load: ");
                if (!fgets(filename, sizeof(filename), stdin)) continue;
                filename[strcspn(filename, "\n")] = 0; // remove newline
                LoadGame(&game, filename);
                continue;
            }

            if (sscanf(line, " %c%d", &from_file, &from_rank) != 2) {
                printf("Invalid input.\n");
                continue; // ask again
            }
            from_file = toupper(from_file);
            from = BoardToIndex(from_file, from_rank);
            
            if (from == -1) {
                printf("There is no such square here!\n");
                continue;
            }
            if (!GetBit(game.red_turn ? game.red_pieces | game.red_kings : game.black_pieces | game.black_kings, from)) {
                printf("No piece at that place!\n");
                continue; // ask again
            }
            
            U64 moves = GetLegalMoves(&game, from);
            if (moves == 0) {
                printf("That piece has no legal moves!\n");
                continue;
            }
            
            if (AnyCaptureAvailable(&game) && !PieceCanCapture(&game, from)) {
                printf("You have a capturable opponent piece!\n");
                continue;
            }
            break; // valid input
        }

        // Highlight possible moves
        PrintBoardWithHighlight(&game, from);

        // Destination selection
        while (1) {
            printf("Note, '*' highlights the directions you can go.\n");
            printf("Select destination (e.g., B3, or 'undo', or 'quit'): ");
            if (!fgets(line, sizeof(line), stdin)) continue;

            if (strncmp(line, "quit", 4) == 0) exit(0);
            if (strncmp(line, "undo", 4) == 0) {
                printf("Undoing piece selection...\n");
                from = -1; // reset selection
                break;    // back to piece selection
            }
            if (sscanf(line, " %c%d", &to_file, &to_rank) != 2) {
                printf("Invalid input.\n");
                continue; // ask again
            }
            
            to_file = toupper(to_file);
            to = BoardToIndex(to_file, to_rank);
            if (to == -1) {
                printf("There is no such square here!\n");
                continue;
            }

            PushState(&undo_stack, &game);
            int result = MovePiece(&game, from, to);
            if (result == 2 || CheckWin(&game)) {
                return 0; // game over
            }
            U64 next_pieces = game.red_turn ? game.red_pieces | game.red_kings: game.black_pieces | game.black_kings;
            int has_moves = 0;
            for (int i = 0; i < 64; i++) {
                if (GetBit(next_pieces, i) && GetLegalMoves(&game, i)) {
                    has_moves = 1;
                    break;
                }
            }
            if (!has_moves) {
                printf("%s has no legal moves. %s wins!\n",
                    game.red_turn ? "Red" : "Black",
                    game.red_turn ? "Black" : "Red");
                return 0;
            }

            if (result == 1) {
                // Normal player move succeeded, switch turn
                if (playAgainstAI && !game.red_turn) {  // AI is Black
                    PrintBoardFull(&game);
                    printf("AI is thinking...\n");
                    sillyAI(&game);
                    if (CheckWin(&game)) return 0;
                }
                break; 
            }

            // Move failed
            printf("Try again.\n");
            continue;
        }
    }

    return 0;
}