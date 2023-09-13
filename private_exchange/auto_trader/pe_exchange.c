#define _DEFAULT_SOURCE
#define _GNU_SOURCE

#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>

#include "pe_exchange.h"

volatile sig_atomic_t message_in = 0;

struct Trader {

    char* name;
    char *exc2trader;
    char *trader2exc;
    int trader_id;
    int fd_w;
    int fd_r;
    pid_t trader_pid;

};

struct Trader *newTrader(int name_len) {

    struct Trader *trader = malloc(sizeof( struct Trader ));

    if (trader == NULL) {
        return NULL;
    }

    trader->name = malloc(name_len * sizeof(char));
    trader->exc2trader = malloc((sizeof(FIFO_EXCHANGE) + 6 + 1) * sizeof(char));
    trader->exc2trader = malloc((sizeof(FIFO_EXCHANGE) + 6 + 1) * sizeof(char));
    trader->trader_id = malloc(sizeof(int));
    trader->fd_w = malloc(sizeof(int));
    trader->fd_r = malloc(sizeof(int));
    trader->trader_pid = malloc(sizeof(pid_t));

    return trader;
}

void rmTrader(struct Trader *trader) {

    if (trader != NULL) {
        free(trader->name);
        free(trader->exc2trader);
        free(trader->exc2trader);
        free(trader->trader_id);
        free(trader->fd_w);
        free(trader->fd_r);
        free(trader->trader_pid);
    }
}

int main(int argc, char *argv[]) {

    printf("%d\n", argc);

    if (argc < 2) {
        printf("Not enough arguments\n");
        return 1;
    }

    

    // reading products

    FILE *file = fopen("products.txt", "r");

    if (file == NULL) {

        perror("Error opening file\n");
        return -1;
    }

    char numlines_str[BUFFER_READ] = "\0";

    fgets(numlines_str, BUFFER_READ, file);   
    numlines_str[strcspn(numlines_str, "\n")] = 0;

    int numlines = atoi(numlines_str);

    char** products;

    products = malloc(numlines * sizeof(char*));

    for (int i = 0; i < numlines; i++) {
        products[i] = malloc(BUFFER_READ * sizeof(char));
        fgets(products[i], BUFFER_READ, file);
        products[i][strcspn(products[i], "\n")] = 0;
    }

    fclose(file);

    // setting up traders

    struct Trader **trader_arr = malloc((argc - 1) * sizeof(**trader_arr));



    for (int i = 0; i < argc - 2; i++) {

        trader_arr[i] = newTrader(strlen(argv[i + 2]));

        trader_arr[i]->name = argv[i + 2]; //argv starts with 1
        trader_arr[i]->trader_id = i;

        sprintf(trader_arr[i]->exc2trader, FIFO_EXCHANGE, trader_arr[i]->trader_id);
        sprintf(trader_arr[i]->trader2exc, FIFO_TRADER, trader_arr[i]->trader_id);


        printf("%s\n", trader_arr[i]->exc2trader);
        printf("%s\n", trader_arr[i]->trader2exc);

    }
    printf("launching traders\n");

    char* buffer = "MARKET OPEN;";

    // opening FIFOs and initiating trader binaries

    for (int i = 0; i < argc - 2; i++) {

        struct Trader *trader = trader_arr[i];
        
        char* trader_argv[3];
        char id[7];

        trader_argv[0] = trader->name;
        sprintf(id, "%d", trader->trader_id);
        trader_argv[1] = id;
        trader_argv[2] = NULL;

        printf("%s, %s\n",trader->exc2trader, trader->trader2exc);

        mkfifo(trader->exc2trader, 0666);
        mkfifo(trader->trader2exc, 0666);

        pid_t pid = fork();

        if (pid < 0) { 
            perror("unable to fork"); return 1;
        }

        if (pid == 0) {
            if (execvp(trader->name, trader_argv) == -1) {
                printf("exec failed\n");
            }
        } else {
            printf("parent is now running\n");
            trader->trader_pid = pid;

            printf("exchange - %s %s\n", trader->exc2trader, trader->trader2exc);

            trader->fd_r = open(trader->trader2exc, O_RDONLY);
            trader->fd_w = open(trader->exc2trader,O_WRONLY);

            printf("open success\n");


            if (write(trader->fd_w, buffer, strlen(buffer) + 1) > 0) {
                printf("success\n");
            }

            kill(trader->trader_pid, SIGUSR1);
        }

    }


    for (int i = 0; i < numlines; i++) {
        free(products[i]);
    }
    free(products);

    for (int i = 0; i < argc - 1; i++) {
        rmTrader(trader_arr[i]);
    }
    free(trader_arr);
    return 0;
}