#include <stdio.h>
#include <string.h>

#define BUFFER_LEN 128
#define ARR_LEN 2

int main(){


    // char input[BUFFER_LEN];
    // char *arr[2];

    // char *ptr;

    // fgets(input, sizeof(input), stdin);

    // arr[0] = strtok(input, " ");

    // int i;

    // for (i = 1; i < ARR_LEN; i++){

    //     arr[1] = strtok(NULL, " ");
    // }

    // if (strtok(NULL, " ") != NULL) {
    //     printf("invalid input\n");
    //     return 0;
    // }

    // for (i = 0; i < ARR_LEN; i++) {
    //     printf("%s\n", arr[i]);
    // }

    // char* msg = "A10";
    // printf("%d\n", atoi(msg + 1));

    // char myarr[19][19] = {
    //     "...................", "...................", "...................", 
    //     "...................", "...................", "...................", 
    //     "...................", "...................",  "...................", "...................", 
    //     "...................", "...................", 
    //     "...................", "...................", 
    //     "...................", 
    //     "...................", 
    //     "...................", 
    //     "...................", 
    //     "..................."
    // };

    // // void func(char arr_in[][19]) {
    // //     myarr[1][2] = 'R';
    // // }

    // // func(myarr);
    // for (int i = 0; i < 3; i++)
    // {
    //     // printf("%s\n", myarr[i]);
    // }


    // char arr[20][20] = {"...................", "...................", "...................", 
    //     "...................", "...................", "...................", 
    //     "...................", "...................",  "...................", "...................", 
    //     "...................", "...................", 
    //     "...................", "...................", 
    //     "...................", 
    //     "...................", 
    //     "...................", 
    //     "...................", 
    //     "..................."};



    // func(arr);
   
    // for (int i = 0; i < 3; i++)
    // {
    //     printf("%s\n", arr[i]);
    // }
    // return 0;

    // char player_input[BUFFER_LEN];
    // char *player_argv[ARR_LEN];
    
    // fgets(player_input, BUFFER_LEN, stdin);

    // player_input[strcspn(player_input, "\n")] = 0;

    // player_argv[0] = strtok(player_input, " ");
    // player_argv[1] = strtok(NULL, " ");

    // // printf("%s\n", player_argv[0]);
    
    // if(!strcmp(player_argv[0], "term")) {
    //     printf("equal\n");
    // } else {
    //     printf("not equal\n");
    // }

    printf("%ld\n", strlen("veryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryverylongstring"));
}

void func(char* arr_in[][19]) {
        *arr_in[1][2] = 'R';
}




    /* check right-to-left diagonal */

    // for (int temp_y = 4; temp_y <= 18; temp_y++) {

    //     int buffer_index = 0;

    //     strcpy(buffer, "\0");

    //     for (int x = 18, y = temp_y; y > 0; x--, y--, buffer_index++) { 
    //         buffer[buffer_index] = map[y][x];
    //     }

    //     if (strstr(buffer, "#####")) {
    //         /* black wins */
    //         return 1;
    //     } else if (strstr(buffer, "ooooo")) {
    //         /* white wins */
    //         return 2;
    //     }
    // }

    // for (int temp_y = 0; temp_y < 15; temp_y++) {

    //     int buffer_index = 0;

    //     strcpy(buffer, "\0");

    //     for (int x = temp_y, y = temp_y; x <= 18 - temp_y; x++, y++, buffer_index++) {
    //         buffer[buffer_index] = map[y][x];
    //     }

    //     if (strstr(buffer, "#####")) {
    //         /* black wins */
    //         return 1;
    //     } else if (strstr(buffer, "ooooo")) {
    //         /* white wins */
    //         return 2;
    //     }
    // }
