#define _DEFAULT_SOURCE
#define _GNU_SOURCE

#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>

#include "pe_trader.h"

int signal_count = 0;

void sig_handler(int signo, siginfo_t* info, void* context) {
    signal_count++;
    
}

int isNumeric (char* str) {

    int i = 0;

    while (i < strlen(str)) {
        if (!isdigit(str[i])) {
            return 0;
        }
        i++;
    }
    return 1;
}


struct Order* process_order(char* market_argv[]) {

    struct Order* order;
    order = malloc(sizeof(struct Order));

    if (order == NULL) {
        perror("new order failed\n");
        return NULL;
    }
    
     // check valid arguments
    if ((strcmp(market_argv[1], "BUY") && strcmp(market_argv[1], "SELL"))
        || !isNumeric(market_argv[3]) || !isNumeric(market_argv[4])) {
        return NULL;
    }

    int order_quantity = atoi(market_argv[3]);
    long int unit_price = (long int) atoi(market_argv[4]);

    if (order_quantity <= 0 || order_quantity > MAX_QUANTITY) {
        return NULL;
    }

    if (unit_price <= 0 || unit_price > MAX_UNIT_PRICE) {
        return NULL;
    }

    int order_type = -1;

    if (!strcmp(market_argv[1], "SELL")) {
        order_type = SELL;
    } else if (!strcmp(market_argv[1], "BUY")) {
        order_type = BUY;
    }

    strcpy(order->item , market_argv[2]);

    order->quantity = order_quantity;
    order->order_type = order_type;
    order->unit_price = unit_price;

    return order;

}
int read_from_exc(char* message, char **market_argv) {

    // return 3 - order accepted
    // return 2 - invalid quantity
    // return 1 - invalid message
    // return 0 - market order received

    // printf("%s\n", message);

    message[strcspn(message, ";")] = 0;

    // printf("[PEX-Milestone] Exchange -> Trader: %s;\n", message);

    if (!strcmp(message, "MARKET OPEN"))
        return MARKET_OPEN;

    // check NULL input
    if (!strcmp(message, "\0")) {
        return INVALID_MESSAGE;  
    }

    // check leading and ending whitespace 
    if (message[0] == ' ' || message[strlen(message) - 1] == ' ') {
        return INVALID_MESSAGE;
    }

    // check extra whitespace
    int i = 0;
    while (i < strlen(message) - 1) {

        if (message[i] == ' ' && message[i+1] == ' ') {
            return INVALID_MESSAGE;
        }
        i++;
    }

    // splitting message into arguments
    char *ptr = strtok(message, " ");
    int arg_count = 0;

    while (ptr != NULL) {
        strcpy(market_argv[arg_count++], ptr);
        ptr = strtok(NULL, " ");

        if (arg_count > 5) {
            return INVALID_MESSAGE;
        }
    }

    if (market_argv == NULL) {
        return INVALID_MESSAGE;
    }

    // check valid arguments
    if (!strcmp(market_argv[0], "MARKET")) {

        if (arg_count != 5) {
            return INVALID_MESSAGE;
        }

        return ORDER_RECEIVED;

    } else if (!strcmp(market_argv[0], "ACCEPTED")) {

        if (arg_count != 2) {
            return INVALID_MESSAGE;
        }
        return VALID_MESSAGE;

    } else if (!strcmp(market_argv[0], "FILL")) {
        
        if (arg_count != 3) {
            return INVALID_MESSAGE;
        }
        return VALID_MESSAGE;

    } else {
        return INVALID_MESSAGE;
    }

}

int main(int argc, char ** argv) {

    // check arguments - trader ID
    if (argc < 2) {
        printf("Not enough arguments\n");
        return 1;
    }

    if (!isNumeric(argv[1])) {
        printf("Invalid Trader ID\n");
        return 1;
    }

    int trader_id = atoi(argv[1]);

    if ((trader_id < 0) || (trader_id >= MAX_TRADERS)) {
        printf("Invalid Trader ID\n");
        return 1;
    }

    // printf("[PEX-Milestone] Launching trader pe_trader\n");

    struct sigaction sig = {
        .sa_sigaction = sig_handler,
        .sa_flags = 0
    };

    memset(&sig, 0, sizeof(struct sigaction)); 
    sig.sa_sigaction = sig_handler; //SETS Handler sig.sa_flags = SA_SIGINFO;
    sig.sa_flags = 0;

    if(sigaction(SIGUSR1, &sig, NULL) == -1) { 
        perror("sigaction failed\n");
        return 1;
    }

    // connect to named pipes
    //pe_exchange_* named pipes are for the exchange write to each trader 
    //pe_trader_* named pipes are for each trader to write to the exchange.

    char exc2trader[sizeof(FIFO_EXCHANGE) + 6 + 1];
    char trader2exc[sizeof(FIFO_TRADER) + 6 + 1];

    sprintf(exc2trader, FIFO_EXCHANGE, trader_id);
    sprintf(trader2exc, FIFO_TRADER, trader_id);

    int fd_R = open(exc2trader, O_RDONLY);
    int fd_W = open(trader2exc,O_WRONLY);

    // printf("[PEX-Milestone] Opened Named Pipes\n");
    // event loop:

    // wait for exchange update (MARKET message)
    // send order
    // wait for exchange confirmation (ACCEPTED message)

    int disconnect = 0;

    pid_t exchange_pid = getppid();

    char **market_argv;
    market_argv = malloc(COMMAND_ARGC * sizeof(char *));

    for (int i = 0; i < COMMAND_ARGC; i++) {
        market_argv[i] = malloc(BUFFER_READ * sizeof(char));
    }

    char buffer_read[BUFFER_READ];
    char buffer_write[BUFFER_WRITE];

    int state = -1;
    int orderID = 0;
    int signal_ptr = 0;

    while (!disconnect) {
        
        if (signal_ptr < signal_count) {

            // printf("message_in\n");

            memset(buffer_read,0,strlen(buffer_read));

            strcpy(buffer_read, "");
            
            read(fd_R, buffer_read, BUFFER_READ);

            state = read_from_exc(buffer_read, market_argv);
            memset(buffer_read,0,strlen(buffer_read));

            // printf("message_in\n");
            // printf("state = %d\n", state);
            

            switch(state) {

                //market order received
                case(ORDER_RECEIVED): {
                    
                    struct Order* newOrder;
                    newOrder = process_order(market_argv);

                    if (newOrder != NULL) {

                        if (newOrder->quantity >= 1000) { 
                            disconnect = 1;
                            break;
                        }

                        char order_type[5];

                        if (newOrder->order_type == SELL) {
                            strcpy(order_type, "BUY");

                            memset(buffer_write,0,strlen(buffer_write));
                            sprintf(buffer_write, "%s %d %s %d %ld;", order_type, orderID, 
                            newOrder->item, newOrder->quantity, newOrder->unit_price);

                            if (write(fd_W, buffer_write, strlen(buffer_write)) == -1) {
                                printf("write failed\n");
                            }
                            kill(exchange_pid, SIGUSR1);
                            memset(buffer_write,0,strlen(buffer_write));

                            orderID++;

                            free(newOrder);

                            break;
                        }
                    }

                }

                case(INVALID_MESSAGE): {
                    orderID++;
                    break;
                }

                default:
                    break;

            }

            signal_ptr++;
        }
        
    }

    free(market_argv);

    close(fd_R);
    close(fd_W);

    return 0;
}
