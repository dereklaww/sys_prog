#define _DEFAULT_SOURCE
#define _GNU_SOURCE

#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>

#include "pe_trader.h"

// volatile sig_atomic_t interrupted = 0;
volatile sig_atomic_t message_in = 0;

struct Order {

    int order_type; // BUY - 0, SELL - 1
    char item[BUFFER_READ];
    int quantity;
    int unit_price;
};

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

void sig_handler(int signo, siginfo_t* info, void* context) {
    message_in = 1;
}

int process_order(char* market_argv[], struct Order *order_ptr) {

     // check valid arguments
    if ((strcmp(market_argv[1], "BUY") && strcmp(market_argv[1], "SELL"))
        || !isNumeric(market_argv[3]) || !isNumeric(market_argv[4])) {

        printf("%d %d %d\n", (strcmp(market_argv[1], "BUY") && strcmp(market_argv[1], "SELL"))
        , !isNumeric(market_argv[3]) , !isNumeric(market_argv[4]));
        return 1;
    }

    int order_quantity = atoi(market_argv[3]);

    // check valid values 
    if (order_quantity < 0 || order_quantity >= 1000) {
        return 2; // invalid buying order
    }

    if (!strcmp(market_argv[1], "SELL")) {
        order_ptr->order_type = 1;
    // } else {if (!strcmp(market_argv[1], "SELL")) {
    //     order_ptr->order_type = 1;
    // }}

    } else {
        return 1;
    }
    
    strcpy(order_ptr->item , market_argv[2]);
    
    order_ptr->quantity = order_quantity;
    order_ptr->unit_price = atoi(market_argv[4]);

    return 0;
}

int read_from_exc(char* buffer, struct Order *order) {

    // return 3 - order accepted
    // return 2 - invalid quantity
    // return 1 - invalid message
    // return 0 - market order received

    // printf("%s\n", buffer);

    buffer[strcspn(buffer, ";")] = 0;

    // printf("[PEX-Milestone] Exchange -> Trader: %s;\n", buffer);

    if (!strcmp(buffer, "MARKET OPEN"))
        return 4;

    // check NULL input
    if (!strcmp(buffer, "\0")) {
        printf("Null input\n");
        return 1;  
    }

    // check leading and ending whitespace 
    if (buffer[0] == ' ' || buffer[strlen(buffer) - 1] == ' ') {
        printf("Invalid - leading/end\n");
        return 1;
    }

    // check extra whitespace
    int i = 0;
    while (i < strlen(buffer) - 1) {

        if (buffer[i] == ' ' && buffer[i+1] == ' ') {
            printf("Invalid - consecutive whitespaces\n");
            return 1;
        }
        i++;
    }

    char *ptr = strtok(buffer, " ");
    char *market_argv[5] = {"\0"};

    int arg_count = 0;
    int status = -1;

    while (ptr != NULL) {

        market_argv[arg_count++] = ptr;
        ptr = strtok(NULL, " ");
    }

    // check valid arguments
    if (!strcmp(market_argv[0], "MARKET")) {

        if (arg_count != 5) {
            printf("Market - Too many arguments: %d\n", arg_count);
            return 1;
        }

        status = process_order(market_argv, order);
        // printf("order status: %d\n", status);       
        return status;

    } else if (!strcmp(market_argv[0], "ACCEPTED")) {

        if (arg_count != 2) {
            printf("Accepted - Too many arguments\n");
            return 1;
        }
        return 3;

    } else if (!strcmp(market_argv[0], "INVALID")) {
        printf("Exchange message - Invalid\n");
        return 1;

    } else {
        return 4;
    }

}

// int write_to_exc(char* trader2exc, struct Order *order_ptr, int order_id, int fd_W) {



//     return 0;
// }

int main(int argc, char ** argv) {

    printf("trader - %d\n", argc);

    // check arguments - trader ID
    if (argc < 2) {
        printf("Trader - Not enough arguments\n");
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

    printf("%s %s\n", exc2trader, trader2exc);

    int fd_R = open(exc2trader, O_RDONLY);
    int fd_W = open(trader2exc,O_WRONLY);

    printf("[PEX-Milestone] Opened Named Pipes\n");
    // event loop:

    // wait for exchange update (MARKET message)
    // send order
    // wait for exchange confirmation (ACCEPTED message)

    struct Order order;

    memset(&order, 0, sizeof(order));

    struct Order *order_ptr = &order;

    order.order_type = -1;
    order.quantity = -1;
    order.unit_price = -1;

    int orderID = 0;
    int disconnect = 0;

    pid_t exchange_pid = getppid();


    char buffer_read[BUFFER_READ];
    char buffer_write[128];
    char order_type[5];

    strcpy(buffer_read, "\0");
    strcpy(buffer_write, "\0");
    strcpy(buffer_write, "\0");



    int state = -1;

    while (!disconnect) {
        
        if (message_in) {

            printf("message_in\n");

            strcpy(buffer_read, "");
            
            read(fd_R, buffer_read, BUFFER_READ);

            state = read_from_exc(buffer_read, order_ptr);

            // printf("message_in\n");
            // printf("state = %d\n", state);
            

            switch(state) {

                //market order received
                case(0): {

                    if (order_ptr->order_type == 0) {
                        strcpy(order_type, "SELL"); 
                    } else {
                        strcpy(order_type, "BUY");
                    }

                    sprintf(buffer_write, "%s %d %s %d %d;", order_type, orderID, 
                    order_ptr->item, order_ptr->quantity, order_ptr->unit_price);

                    if (write(fd_W, buffer_write, strlen(buffer_write)) == -1) {
                        printf("write failed\n");
                    }
                    kill(exchange_pid, SIGUSR1);
                    // printf("%s\n", buffer_write);

                    orderID++;

                    break;
                }

                // case(1): {
                //     printf("Market order failed\n");
                //     break;
                // }

                case(2): {
                    disconnect = 1;
                    break;
                }

                // case(3): {
                //     printf("order accepted\n");
                //     break;
                // }

                default:
                    break;

            }

            message_in = 0;
        }
        
    }

    close(fd_R);
    close(fd_W);

    return 0;
}


