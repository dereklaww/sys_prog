// name: Derek Law
// unikey: dlaw7849
// SID: 510226244

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define BUFFER_LEN 50
#define HISTORY_LEN 1500
#define ROWS 20
#define COLS 20
#define ARGV_LEN 2

int input_processing(char* player_input, char player_argv[][BUFFER_LEN]) {

        char* input_safe_check;

        // USYD CODE CITATION ACKNOWLEDGEMENT
        // I declare that the majority of the following code has been taken
        // from the website titled: "Removing trailing newline character from fgets() input" and it is not my
        // own work. 
        //  
        // Original URL
        // https://stackoverflow.com/questions/2693776/removing-trailing-newline-character-from-fgets-input
        // Last access March, 2023

        player_input[strcspn(player_input, "\n")] = 0;

        // check NULL input
        if (!strcmp(player_input, "\0")) {
            printf("Invalid!\n");
            return 1;  
        }

        // check leading and ending whitespace
        if (player_input[0] == ' ' || player_input[strlen(player_input) - 1] == ' ') {
            printf("Invalid!\n");
            return 1;
        }
        
        //check multiple arguments
        int whitespace_count = 0;
        for (int i = 0; i < strlen(player_input); i++) {
            if (player_input[i] == ' ') {
                whitespace_count++;
            }
        }

        if (whitespace_count > 1) {
            printf("Invalid!\n");
            return 1;
        }

        /* split string at space */ 
        strcpy(player_argv[0],strtok(player_input, " "));

        // check for second argument
        input_safe_check = strtok(NULL, " ");

        if (input_safe_check != NULL) {
            strcpy(player_argv[1], input_safe_check);
        }

        // check if only "place" command has 2 arguments
        if (strcmp(player_argv[0], "place") && (strcmp(player_argv[1], "\0"))) {
            printf("Invalid!\n");
            return 1;
        }

    return 0;

}

int place(char map[][COLS], int curr_player, char* coord_input, int mid_mist[]) {
    
    char key;

    /* convert coord_input to map coordinates*/
    int h_coord = (coord_input[0] - 'A');
    int v_coord = atoi(coord_input + 1);

    /* decode keys */
    if (curr_player ==  0) {
        key = '#';
    } else {
        key = 'o';
    }

    // check if tile is occupied
    if (map[ROWS - v_coord - 1][h_coord] != '.') {
        printf("Occupied coordinate\n");
        return 0;
    } else {
        map[ROWS - v_coord - 1][h_coord] = key;

        /* calculate centre of mist */
        mid_mist[0] = 1 + ((5 * (h_coord + 1) * (h_coord + 1) + 3 * (h_coord + 1) + 4) % 19);
        mid_mist[1] = 1 + ((4 * v_coord * v_coord + 2 * v_coord - 4) % 19);
    }

    return 1;
    
}

int view(char map[][COLS], int mid_mist[]) {

    char view_buffer[BUFFER_LEN] = "";
    int buffer_index = 0;

    // print coordinates of midpoint
    printf("%c", 'A' + (mid_mist[0] - 1));

    printf("%d,", mid_mist[1]);

    // +-3 of x-coordinate, +-3 of y-coordinate
    for (int i = (ROWS - mid_mist[1] - 1) - 3; i <= (ROWS - mid_mist[1] - 1) + 3; i++) {

        for (int j = (mid_mist[0] - 1) - 3; j <= (mid_mist[0] - 1) + 3; j++) {

            if (i < 0 || i > ROWS - 2 || j < 0 || j > COLS - 2) {
                view_buffer[buffer_index] = 'x';
            } else {
                view_buffer[buffer_index] = map[i][j];
            }

            buffer_index++;
        }
    }

    printf("%s\n", view_buffer);

    return 0;
    
}


int check_game_cond(char map[][20]) {

    
    /* win condition - horizontal, vertical, diagonal*/
    /* check horizontal */

    for (int i = 0; i < ROWS; i++) {
        if (strstr(map[i], "#####")) {
            /* black wins */
            return 1;
        } else if (strstr(map[i], "ooooo")) {
            /* white wins */
            return 2;
        }
    }

    /* check vertical */
    for (int h_coord = 0; h_coord < COLS - 1; h_coord++) {

        // row 15 onwards will have less than 5 keys
        for (int v_coord = 0; v_coord < 15; v_coord++) {
        
            if (map[v_coord][h_coord] == '#' &&
                map[v_coord + 1][h_coord] == '#' &&
                map[v_coord + 2][h_coord] == '#' &&
                map[v_coord + 3][h_coord] == '#' &&
                map[v_coord + 4][h_coord] == '#' ) {
                /* black wins */
                return 1;
            } else if (map[v_coord][h_coord] == 'o' &&
                map[v_coord + 1][h_coord] == 'o' &&
                map[v_coord + 2][h_coord] == 'o' &&
                map[v_coord + 3][h_coord] == 'o' &&
                map[v_coord + 4][h_coord] == 'o' ) {
                /* white wins */
                return 2;
                }

        }
    }

    /* check diagonal - upper left to bottom right */
    char buffer[BUFFER_LEN];

    // first set [(0, 4) -> (4, 0)]
    // last set [(0, 18) -> (18, 0)]

    for (int temp_y = 4; temp_y < ROWS - 1; temp_y++) {

        int buffer_index = 0;

        strcpy(buffer, "\0");

        for (int x = 0, y = temp_y; x <= temp_y; x++, y--, buffer_index++) { 
            buffer[buffer_index] = map[y][x];
        }


        if (strstr(buffer, "#####")) {
            /* black wins */
            return 1;
        } else if (strstr(buffer, "ooooo")) {
            /* white wins */
            return 2;
        }
    }

    // first set [(14, 18) -> (18, 14)]
    // last set [(0, 18) -> (18, 0)]

    for (int temp_x = 14; temp_x >= 0; temp_x--) {

        int buffer_index = 0;

        strcpy(buffer, "\0");

        for (int x = temp_x, y = 18; y <= temp_x; x++, y--, buffer_index++) {
            buffer[buffer_index] = map[y][x];
        }


        if (strstr(buffer, "#####")) {
            /* black wins */
            return 1;
        } else if (strstr(buffer, "ooooo")) {
            /* white wins */
            return 2;
        }
    }

    /* check diagonal - upper right to bottom left */

    // first set [(14, 0) -> (18, 4)]
    // last set [(0, 0) -> (18, 18)]

    for (int temp_x = 14; temp_x > 0; temp_x--) {

        int buffer_index = 0;

        strcpy(buffer, "\0");

        for (int x = temp_x, y = 0; x < ROWS - 1; x++, y++, buffer_index++) { 
            buffer[buffer_index] = map[y][x];
        }


        if (strstr(buffer, "#####")) {
            /* black wins */
            return 1;
        } else if (strstr(buffer, "ooooo")) {
            /* white wins */
            return 2;
        }
    }

    // first set [(0, 14) -> (4, 18)]
    // last set [(0, 0) -> (18, 18)]

    for (int temp_y = 14; temp_y > 0; temp_y--) {

        int buffer_index = 0;

        strcpy(buffer, "\0");

        for (int x = 0, y = temp_y; y < ROWS - 1; x++, y++, buffer_index++) {
            buffer[buffer_index] = map[y][x];
        }

        if (strstr(buffer, "#####")) {
            /* black wins */
            return 1;
        } else if (strstr(buffer, "ooooo")) {
            /* white wins */
            return 2;
        }
    }

    int tile_available = 0;

    /* tie condition - all tiles are occupied */

    for (int i = 0; i < ROWS - 1; i++) {

        for (int j = 0; j < COLS - 1; j++) {
            if (map[i][j] == '.') {
                tile_available = 1;
            }
        }
    }

    if (tile_available == 0) {
        /* no tile available - tie */
        return 3;
    }

    return 0;
}

int main() {
    
    /* map declaration */

    char game_map[ROWS][COLS] = 
    {
        "...................", 
        "...................", 
        "...................", 
        "...................", 
        "...................", 
        "...................", 
        "...................", 
        "...................", 
        "...................", 
        "...................", 
        "...................", 
        "...................", 
        "...................", 
        "...................", 
        "...................", 
        "...................", 
        "...................", 
        "...................", 
        "...................", 
    };

    /* game configurations */

    // curr_player: 0 = B, 1 = W
    int curr_player = 0;
    int next_round = 0;

    /* read command line arguments */
    char player_input[BUFFER_LEN] = "\0";
    char player_argv[ARGV_LEN][BUFFER_LEN] = {"\0", "\0"};
    char *input_safe_check;
    char *coord_input = "\0";

    /* buffer for history */
    char history[HISTORY_LEN] = "\0";

    /* mist initialisation */
    int mid_mist[2] = {10, 10};

    int valid_command = 0;
    int invalid_coord = 0;

    /* length of player input */
    int input_len = 0;
    /* variable to read remaining characters from stdin */
    int c;

    while (1) {

        /* reset buffer values */
        strcpy(player_input, "\0");
        strcpy(player_argv[0], "\0");
        strcpy(player_argv[1], "\0");
        valid_command = 0;
        invalid_coord = 0;
        
        /* read input from stdin */
        input_safe_check = fgets(player_input, BUFFER_LEN, stdin);

        /* check for NULL input */
        if (input_safe_check == NULL) {
            printf("Invalid!\n");
            continue;
        }

        // check for input length longer than buffer size
        input_len = strlen(input_safe_check);
        int coord_probability = 0;
        if (input_len == BUFFER_LEN - 1 && input_safe_check[BUFFER_LEN - 2] != '\n') {
            
            // check if input is a place command - different error handling
            if (!strcmp(strtok(input_safe_check, " "), "place")) {
                coord_probability = 1;
                strtok(NULL, " ");

                if (strtok(NULL, " ") != NULL) {
                    coord_probability = 0;
                }
            }

            // read remaining characters from stdin
            while ((c = getchar()) && (c != '\n') && (c != EOF)) {
                if (c == ' ') {
                    coord_probability = 0;
                }
            }

            // print error messages
            if (coord_probability == 1) {
                printf("Invalid coordinate\n");
            } else {
                printf("Invalid!\n");
            }
            continue;
        }
        
        // error checking for whitespaces, no. of arguments, splitting input at whitespace if valid input
        if (input_processing(player_input, player_argv)) {
            continue;
        } else {
        
            /* process player commands */
            
            if (!strcmp(player_argv[0], "who")) {
                valid_command = 1;
                if (curr_player ==  0) {
                    printf("B\n");
                } else {
                    printf("W\n");
                }
            }

            else if (!strcmp(player_argv[0], "term")) {
                return 1;
            }

            else if (!strcmp(player_argv[0], "resign")) {
                if (curr_player ==  0) {
                    printf("White wins!\n");
                    printf("%s\n", history);
                } else {
                    printf("Black wins!\n");
                    printf("%s\n", history);
                }
                printf("Thank you for playing!\n");
                return 0;
            }

            else if (!strcmp(player_argv[0], "place")) {

                valid_command = 1;

                if (!strcmp(player_argv[1] ,"\0")) {
                    printf("Invalid!\n");
                    continue;
                }

                coord_input = player_argv[1];

                // check length of coordinates fulfil 2/3 characters
                if (strlen(coord_input) != 2 && strlen(coord_input) != 3 ) {
                    invalid_coord = 1;
                }
                
                // check first character is an alphabet
                if (!isalpha(coord_input[0])) {
                    invalid_coord = 1;
                }
                
                // check remaining characters are digits
                for (int i = 1; i < strlen(coord_input); i++) {
                    if (!isdigit(coord_input[i])) {
                        invalid_coord = 1;
                    }
                }

                // check for leading zeros
                if (strlen(coord_input) == 3 && coord_input[1] == '0') {
                    invalid_coord = 1;

                // check for capital letters
                } else if (coord_input[0] % 'A' < 0 || coord_input[0] % 'A' > 18) {
                    invalid_coord = 1;

                // check digits in range 1-9
                } else if (atoi(coord_input + 1) < 1 || atoi(coord_input + 1) > 19) {
                    invalid_coord = 1;
                }
                
                // if valid coordinate, place character, update history, flag next turn
                if (invalid_coord) {
                    printf("Invalid coordinate\n");
                    continue;
                } else {
                    if (place(game_map, curr_player, coord_input, mid_mist)) {
                        strcat(history, player_argv[1]);
                        next_round = 1;
                    }
                }
            }

            else if (!strcmp(player_argv[0], "history")) {
                valid_command = 1;
                printf("%s\n", history);
            }

            else if (!strcmp(player_argv[0], "view")) {
                valid_command = 1;
                view(game_map, mid_mist);
            }
        }
        
        if (!valid_command) {
            printf("Invalid!\n");
            continue;
        }
    
        if (check_game_cond(game_map) == 1) {
            printf("Black wins!\n");
            printf("%s\n", history);
            printf("Thank you for playing!\n");
            return 0;
        } else if (check_game_cond(game_map) == 2) {
            printf("White wins!\n");
            printf("%s\n", history);
            printf("Thank you for playing!\n");
            return 0;
        } else if (check_game_cond(game_map) == 3) {
            printf("Wow, a tie!\n");
            printf("%s\n", history);
            printf("Thank you for playing!\n");
            return 0;
        }

        if (next_round) {
            if (curr_player) {
                curr_player = 0;
                next_round = 0;
            } else {
                curr_player = 1;
                next_round = 0;
            }
        }

    }
}




