#define _DEFAULT_SOURCE
#define _GNU_SOURCE

#include "pe_exchange.h" 

// setting up signal handler

// volatile sig_atomic_t message_in = 0;
// volatile pid_t message_pid = 0;

pid_t *signal_array;
int signal_count = 0;

void sig_handler(int signo, siginfo_t* info, void* context) {
    // message_in = 1;
    signal_array[signal_count] = info->si_pid;
    signal_count++;
}

enum {
    ORDER_RECEIVED = 0,
    INVALID_MESSAGE,
    AMEND_REQUEST,
    CANCEL_REQUEST
};

Trader *find_trader(Trader** traders, int trader_count, pid_t pid) {

    for (int i = 0; i < trader_count; i++) {
        if (traders[i]->trader_pid == pid) {
            return traders[i];
        }
    }

    return NULL;
}

int read_message(char* message, char **market_argv, char **product_list, int product_count) {

    // check NULL input
    if (!strcmp(message, "\0")) {
        printf("Null input\n");
        return INVALID_MESSAGE;
    }

    // check leading and ending whitespace 
    if (message[0] == ' ' || message[strlen(message) - 1] == ' ') {
        printf("Invalid - leading/end\n");
        return INVALID_MESSAGE;
    }

    // check extra whitespace
    int i = 0;
    while (i < strlen(message) - 1) {

        if (message[i] == ' ' && message[i+1] == ' ') {
            printf("Invalid - consecutive whitespaces\n");
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

    if ((!strcmp(market_argv[0], "SELL")) || (!strcmp(market_argv[0], "BUY"))) {

        if (arg_count != 5) {
            return INVALID_MESSAGE;
        }

        char *product = market_argv[2];

        for (int i = 0; i < product_count; i++) {
            if (!strcmp(product,product_list[i])){
                return ORDER_RECEIVED;
            }
        }

        return INVALID_MESSAGE;
    
    } else if (!strcmp(market_argv[0], "AMEND")) {

        if (arg_count != 4) {
            return INVALID_MESSAGE;
        }

        return AMEND_REQUEST;
    } else if (!strcmp(market_argv[0], "CANCEL")) {

        if (arg_count != 2) {
            return INVALID_MESSAGE;
        }

        return CANCEL_REQUEST;
    
    } else {
        return INVALID_MESSAGE; 
    }

}

int amend(char* market_argv[], Market *market, Trader *trader) {

    // Example message: AMEND  <order_id> <quantity> <price>;

    if (!isNumeric(market_argv[1]) || !isNumeric(market_argv[2]) 
    || !isNumeric(market_argv[3])) {
        return 1;
    }

    int order_to_amend = atoi(market_argv[1]);
    int newQuantity = atoi(market_argv[2]);
    int newPrice = atoi(market_argv[3]);

    if (newQuantity <= 0 || newQuantity > MAX_QUANTITY) {
        return 1;
    }

    if (newPrice <= 0 || newPrice > MAX_UNIT_PRICE) {
        return 1;
    }

    for (int i = 0; i < market->order_count; i++) {
        if ((market->orderbook[i]->order_id == order_to_amend) && (
            market->orderbook[i]->trader->trader_id == trader->trader_id)) {

            if (market->orderbook[i]->completed) {
                return 1;
            }

            Order *order;
            char order_type[BUFFER_LEN_ITOA];

            if (market->orderbook[i]->order_type == BUY) {
                strcpy(order_type, "BUY");
            } else if (market->orderbook[i]->order_type == SELL){
                strcpy(order_type, "SELL");
            }

            char* newAgrv[] = {order_type, market_argv[1], market->orderbook[i]->item,
            market_argv[2], market_argv[3]};
            order = newOrder(newAgrv, trader, market->orderbook[i]->order_type);

            if (order != NULL) {
                marketAddOrder(market, order);
                traderAmendOrder(trader, order);
                closeOrder(market->orderbook[i]);
                return 0;
            }
        }
    }

    return 1;
}

void printOrderBook(Market *market, char** products, Trader** traders, int product_count, int trader_count) {
    printf("%s\t--ORDERBOOK--\n", LOG_PREFIX);
                        
    for (int i = 0; i < product_count; i++) {
        printOrderByProduct(market->orderbook, products[i], market->order_count);
    }

    printf("%s\t--POSITIONS--\n", LOG_PREFIX);

    // [PEX]    Trader 0: GPU 0 ($0), Router 0 ($0)

    for (int i = 0; i < trader_count; i++) {
        
        printf("%s\tTrader %d: ", LOG_PREFIX, i);

        for (int j = 0; j < product_count - 1; j++) {
            printf("%s %d ($%lld), ", traders[i]->inventory_products[j], 
            traders[i]->inventory_quantity[j],
            traders[i]->bank[j]);
        }

        printf("%s %d ($%lld)\n", traders[i]->inventory_products[product_count - 1], 
            traders[i]->inventory_quantity[product_count - 1],
            traders[i]->bank[product_count - 1]);
    }
}


int main(int argc, char *argv[]) {

    // initialize exchange
    printf("%s Starting\n", LOG_PREFIX);

    if (argc < 2) {
        printf("Not enough arguments\n");
        return 1;
    }

    // reading products

    char** products = NULL;
    char* product_buffer;
    char numlines_str[BUFFER_READ] = "\0";
    int product_count = -1;
    
    FILE *file = fopen(argv[1], "r");

    if (file == NULL) {

        perror("Error opening file\n");
        return -1;
    }

    fgets(numlines_str, BUFFER_READ, file);   
    numlines_str[strcspn(numlines_str, "\n")] = 0;

    product_count = atoi(numlines_str);

    products = malloc(product_count * sizeof(char*));
    product_buffer = malloc(product_count * BUFFER_READ * sizeof(char));

    sprintf(product_buffer, "%s Trading %d products:", LOG_PREFIX, product_count);

    for (int i = 0; i < product_count; i++) {
        products[i] = malloc(BUFFER_READ * sizeof(char));
        fgets(products[i], BUFFER_READ, file);
        products[i][strcspn(products[i], "\n")] = 0;
        strcat(product_buffer, " ");
        strcat(product_buffer, products[i]);
    }

    fclose(file);

    printf("%s\n",product_buffer);

    free(product_buffer);

    // setting up signal handler

    signal_array = calloc(MAX_SIGNALS, sizeof(pid_t));
    int sizeofSignalArray = MAX_SIGNALS;
    int sig_ptr = 0;

    struct sigaction sig = {
        .sa_sigaction = sig_handler,
        .sa_flags = 0
    };

    
    memset(&sig, 0, sizeof(struct sigaction)); 
    sig.sa_sigaction = sig_handler; //SETS Handler 
    sig.sa_flags = SA_SIGINFO;

    if(sigaction(SIGUSR1, &sig, NULL) == -1) { 
        perror("sigaction failed\n");
        return 1;
    }

    sigset_t signal_set;
    sigemptyset(&signal_set);
    sigaddset(&signal_set, SIGUSR1);

    // setting up traders

    Trader **traders;

    int trader_count = argc - 2;

    traders = malloc(sizeof(Trader*) * trader_count);

    for (int i = 0; i < trader_count; i++) {

        traders[i] = inst_trader(argv[i + 2], i, product_count, products);

        sprintf(traders[i]->exc2trader, FIFO_EXCHANGE, traders[i]->trader_id);
        sprintf(traders[i]->trader2exc, FIFO_TRADER, traders[i]->trader_id);
    }

    // creating FIFO pipes
    for (int i = 0; i < trader_count; i++) {
        
        char* trader_argv[3];
        char id[7];

        trader_argv[0] = traders[i]->name;
        sprintf(id, "%d", traders[i]->trader_id);
        trader_argv[1] = id;
        trader_argv[2] = NULL;


        mkfifo(traders[i]->exc2trader, 0666);
        printf("%s Created FIFO %s\n", LOG_PREFIX, traders[i]->exc2trader);

        mkfifo(traders[i]->trader2exc, 0666);
        printf("%s Created FIFO %s\n", LOG_PREFIX, traders[i]->trader2exc);


        // executing trader binary
        printf("%s Starting trader %d (%s)\n", LOG_PREFIX, traders[i]->trader_id, traders[i]->name);
        pid_t pid = fork();

        if (pid < 0) { 
            perror("unable to fork");  
            return 1;
        }

        if (pid == 0) {
            if (execvp(traders[i]->name, trader_argv) == -1) {
                printf("exec failed\n");
                return 1;
            }
        } else {
            traders[i]->trader_pid = pid;

            traders[i]->fd_w = open(traders[i]->exc2trader,O_WRONLY);
            printf("%s Connected to %s\n", LOG_PREFIX, traders[i]->exc2trader);
            
            traders[i]->fd_r = open(traders[i]->trader2exc, O_RDONLY);
            printf("%s Connected to %s\n", LOG_PREFIX, traders[i]->trader2exc);
        }
    }

    // send market open to all traders
    char *market_open = "MARKET OPEN;";

    for (int i = 0; i < trader_count; i++) {

        if (write(traders[i]->fd_w, market_open, strlen(market_open)) == -1) {
            printf("write failed: %d\n", traders[i]->fd_w);
        }
        kill(traders[i]->trader_pid, SIGUSR1);
    }

    // event loop

    int exchange_disconnect = 0;
    int traders_online = trader_count;
    
    char **market_argv;
    market_argv = malloc(COMMAND_ARGC * sizeof(char *));

    for (int i = 0; i < COMMAND_ARGC; i++) {
        market_argv[i] = malloc(BUFFER_READ * sizeof(char));
    }
    
    char buffer_read[BUFFER_READ];
    char buffer_write[BUFFER_WRITE];

    Market *market = inst_market();

    while (!exchange_disconnect) {

        if (sig_ptr < signal_count) {

            memset(buffer_read,0,strlen(buffer_read));

            pid_t message_pid = signal_array[sig_ptr];
            
            Trader *incoming_trader = find_trader(traders, trader_count, message_pid);
            read(incoming_trader->fd_r, buffer_read, BUFFER_READ);

            Order *order;
            int state = -1;
            
            buffer_read[strcspn(buffer_read, ";")] = 0;
            
            printf("%s [T%d] Parsing command: <%s>\n", LOG_PREFIX, incoming_trader->trader_id, buffer_read);

            state = read_message(buffer_read, market_argv, products, product_count);

            memset(buffer_read,0,strlen(buffer_read));

            switch (state) {
                
                case (ORDER_RECEIVED): {

                    int order_type = -1;

                    if (!strcmp(market_argv[0], "SELL")) {
                        order_type = SELL;
                    } else if (!strcmp(market_argv[0], "BUY")) {
                        order_type = BUY;
                    }

                    order = newOrder(market_argv, incoming_trader, order_type);

                    if (order != NULL) {

                        marketAddOrder(market, order);

                        traderAddOrder(incoming_trader, order);

                        memset(buffer_write,0,strlen(buffer_write));

                        sprintf(buffer_write, "ACCEPTED %d;", order->order_id);

                        if (write(incoming_trader->fd_w, buffer_write, strlen(buffer_write)) == -1) {
                            printf("write failed\n");
                            }
                        
                        kill(message_pid, SIGUSR1);
                        memset(buffer_write,0,strlen(buffer_write));

                        sprintf(buffer_write, "MARKET %s %s %d %ld;", market_argv[0], order->item,
                        order->quantity, order->unit_price);

                        for (int i = 0; i < trader_count; i++) {

                            if ((traders[i] -> trader_id != incoming_trader->trader_id) && (!traders[i]->disconnected)) {
                                if (write(traders[i]->fd_w, buffer_write, strlen(buffer_write)) == -1) {
                                    printf("write failed\n");
                                    }
                                
                                kill(traders[i]->trader_pid, SIGUSR1);
                            }
                        }

                        memset(buffer_write,0,strlen(buffer_write));

                        Trade *trade;

                        if (order_type == BUY) {
                            trade = placeBuyOrder(market, order);
                        } else if(order_type == SELL) {
                            trade = placeSellOrder(market, order);
                        }

                        // trade completed
                        while (trade != NULL) {
                            char *buffer_print;
                            buffer_print = calloc(sizeof(char), BUFFER_PRINT);

                            strcpy(buffer_print ,"");

                            long long int value = trade->transaction_quantity * trade->transaction_price;

                            sprintf(buffer_print, "%s Match: Order %d [T%d], New Order %d [T%d], value: $%lld, fee: $%lld.", 
                            LOG_PREFIX ,trade->order_in_book->order_id, trade->order_in_book->trader->trader_id,
                            trade->incoming_order->order_id, trade->incoming_order->trader->trader_id,
                            value, trade->tax);

                            printf("%s\n", buffer_print);

                            strcpy(buffer_print ,"");

                            sprintf(buffer_print, "FILL %d %d;", trade->incoming_order->order_id, trade->transaction_quantity);

                            if (write(trade->incoming_order->trader->fd_w, buffer_print, strlen(buffer_print)) == -1) {
                                printf("write failed\n");
                                }
                            
                            kill(trade->incoming_order->trader->trader_pid, SIGUSR1);

                            sprintf(buffer_print, "FILL %d %d;", trade->order_in_book->order_id, trade->transaction_quantity);

                            if (!trade->order_in_book->trader->disconnected) {
                                if (write(trade->order_in_book->trader->fd_w, buffer_print, strlen(buffer_print)) == -1) {
                                    printf("write failed\n");
                                    }
                            }
                            
                            kill(trade->incoming_order->trader->trader_pid, SIGUSR1);

                            free(buffer_print);

                            if (order_type == BUY) {
                                trade = placeBuyOrder(market, order);
                            } else if(order_type == SELL) {
                                trade = placeSellOrder(market, order);
                            }

                        }

                        sigprocmask(SIG_BLOCK, &signal_set, NULL);
                        printOrderBook(market, products, traders, product_count, trader_count);
                        sigprocmask(SIG_UNBLOCK, &signal_set, NULL);
                        
                    } else {
                        strcpy(buffer_write, "INVALID;");

                        if (write(incoming_trader->fd_w, buffer_write, strlen(buffer_write)) == -1) {
                            printf("write failed\n");
                            }
                        
                        kill(message_pid, SIGUSR1);
                        memset(buffer_write,0,strlen(buffer_write));
                    }

                    break;
                }

                case (AMEND_REQUEST): {

                    if (amend(market_argv, market, incoming_trader)) {

                        strcpy(buffer_write, "INVALID;");

                        if (write(incoming_trader->fd_w, buffer_write, strlen(buffer_write)) == -1) {
                            printf("write failed\n");
                            }
                        
                        kill(message_pid, SIGUSR1);
                        memset(buffer_write,0,strlen(buffer_write));

                    } else {

                        sprintf(buffer_write, "AMENDED %s;", market_argv[1]);

                        if (write(incoming_trader->fd_w, buffer_write, strlen(buffer_write)) == -1) {
                            printf("write failed\n");
                            }
                        
                        kill(message_pid, SIGUSR1);
                        memset(buffer_write,0,strlen(buffer_write));

                        int order_id = atoi(market_argv[1]);

                        Order *amended_order = getOrder(incoming_trader, order_id);

                        char order_type[BUFFER_LEN_ITOA];

                        if (amended_order->order_type == BUY) {
                            strcpy(order_type, "BUY");
                        } else if (amended_order->order_type == SELL) {
                            strcpy(order_type, "SELL");
                        }

                        sprintf(buffer_write, "MARKET %s %s %d %ld;", order_type, amended_order->item,
                        amended_order->quantity, amended_order->unit_price);

                        for (int i = 0; i < trader_count; i++) {
                            if ((traders[i] -> trader_id != incoming_trader->trader_id) && (!traders[i]->disconnected)) {
                                if (write(traders[i]->fd_w, buffer_write, strlen(buffer_write)) == -1) {
                                    printf("write failed\n");
                                    }
                                
                                kill(traders[i]->trader_pid, SIGUSR1);
                            }
                        }
                        memset(buffer_write,0,strlen(buffer_write));

                        Trade *trade;

                        if (amended_order->order_type == BUY) {
                            trade = placeBuyOrder(market, amended_order);
                        } else if (amended_order->order_type == SELL) {
                            trade = placeSellOrder(market, amended_order);
                        }

                        // trade completed
                        while (trade != NULL) {
                            char *buffer_print;
                            buffer_print = calloc(sizeof(char), BUFFER_PRINT);

                            strcpy(buffer_print ,"");

                            long long int value = trade->transaction_quantity * trade->transaction_price;

                            sprintf(buffer_print, "%s Match: Order %d [T%d], New Order %d [T%d], value: $%lld, fee: $%lld.", 
                            LOG_PREFIX ,trade->order_in_book->order_id, trade->order_in_book->trader->trader_id,
                            trade->incoming_order->order_id, trade->incoming_order->trader->trader_id,
                            value, trade->tax);

                            printf("%s\n", buffer_print);

                            strcpy(buffer_print ,"");

                            sprintf(buffer_print, "FILL %d %d;", trade->incoming_order->order_id, trade->transaction_quantity);

                            if (write(trade->incoming_order->trader->fd_w, buffer_print, strlen(buffer_print)) == -1) {
                                printf("write failed\n");
                                }
                            
                            kill(trade->incoming_order->trader->trader_pid, SIGUSR1);

                            sprintf(buffer_print, "FILL %d %d;", trade->order_in_book->order_id, trade->transaction_quantity);

                            if (!trade->order_in_book->trader->disconnected) {
                                if (write(trade->order_in_book->trader->fd_w, buffer_print, strlen(buffer_print)) == -1) {
                                    printf("write failed\n");
                                    }
                            }
                            
                            kill(trade->incoming_order->trader->trader_pid, SIGUSR1);

                            free(buffer_print);

                            if (amended_order->order_type == BUY) {
                                trade = placeBuyOrder(market, order);
                            } else if (amended_order->order_type == SELL) {
                                trade = placeSellOrder(market, order);
                            }

                        }

                        sigprocmask(SIG_BLOCK, &signal_set, NULL);
                        printOrderBook(market, products, traders, product_count, trader_count);
                        sigprocmask(SIG_UNBLOCK, &signal_set, NULL);

                    }
                    break;
                }

                case (CANCEL_REQUEST): {

                    if (!isNumeric(market_argv[1])) {
                        strcpy(buffer_write, "INVALID;");

                        if (write(incoming_trader->fd_w, buffer_write, strlen(buffer_write)) == -1) {
                            printf("write failed\n");
                            }
                        
                        kill(message_pid, SIGUSR1);
                        memset(buffer_write,0,strlen(buffer_write));

                    } else {

                        int order_id = atoi(market_argv[1]);
                        Order *order = getOrder(incoming_trader, order_id);
                        
                        if (order != NULL && !order->completed) {
                            
                            closeOrder(order);

                            sprintf(buffer_write, "CANCELLED %d;", order_id);

                            if (write(incoming_trader->fd_w, buffer_write, strlen(buffer_write)) == -1) {
                                printf("write failed\n");
                                }
                        
                            kill(message_pid, SIGUSR1);
                            memset(buffer_write,0,strlen(buffer_write));

                            char order_type[BUFFER_LEN_ITOA];

                            if (order->order_type == BUY) {
                                strcpy(order_type, "BUY");
                            } else if (order->order_type == SELL) {
                                strcpy(order_type, "SELL");
                            }

                            sprintf(buffer_write, "MARKET %s %s %d %d;", order_type, order->item,
                            0, 0);

                            for (int i = 0; i < trader_count; i++) {
                                if ((traders[i] -> trader_id != incoming_trader->trader_id) && (!traders[i]->disconnected)) {
                                    if (write(traders[i]->fd_w, buffer_write, strlen(buffer_write)) == -1) {
                                        printf("write failed\n");
                                    }
                                    
                                    kill(traders[i]->trader_pid, SIGUSR1);
                                }
                            }

                            memset(buffer_write,0,strlen(buffer_write));

                            sigprocmask(SIG_BLOCK, &signal_set, NULL);
                            printOrderBook(market, products, traders, product_count, trader_count);
                            sigprocmask(SIG_UNBLOCK, &signal_set, NULL);
                            
                            
                        } else {
                            strcpy(buffer_write, "INVALID;");

                            if (write(incoming_trader->fd_w, buffer_write, strlen(buffer_write)) == -1) {
                                printf("write failed\n");
                                }
                            
                            kill(message_pid, SIGUSR1);
                            memset(buffer_write,0,strlen(buffer_write));
                        }
                    }
                    break;
                }

                default: {
                    strcpy(buffer_write, "INVALID;");

                        if (write(incoming_trader->fd_w, buffer_write, strlen(buffer_write)) == -1) {
                            printf("write failed\n");
                            }
                        
                        kill(message_pid, SIGUSR1);
                        memset(buffer_write,0,strlen(buffer_write));
                    
                    break;

                }
            }

            sig_ptr++;

            if (signal_count == sizeofSignalArray - 10) {
                void *temp = realloc(signal_array, (sizeofSignalArray + MAX_SIGNALS) * sizeof(pid_t));

                if (temp == NULL) {
                    perror("realloc signal array failed\n");
                } else {
                    signal_array = temp;
                    sizeofSignalArray += MAX_SIGNALS;
                }
            }
        }


        int status = 0;
        pid_t pid = 0;

        pid = waitpid(-1, &status, WNOHANG);

        if (WIFEXITED(status)) {   

            for (int i = 0; i < trader_count; i++) {
                if (traders[i]->trader_pid == pid) {
                    traders[i]->disconnected = 1;
                    traders_online--;
                    printf("%s Trader %d disconnected\n", LOG_PREFIX, i);
                    close(traders[i]->fd_r);
                    close(traders[i]->fd_w);
                    unlink(traders[i]->exc2trader);
                    unlink(traders[i]->trader2exc);
                }

            }
        }

        if (traders_online == 0) {
            exchange_disconnect = 1;
            printf("%s Trading completed\n", LOG_PREFIX);
            printf("%s Exchange fees collected: $%lld\n", LOG_PREFIX, (long long int) market->transaction_fee);
        }

    }


    // freeing malloc resources

    for (int i = 0; i < product_count; i++) {
        free(products[i]);
    }
    free(products);
    
    for (int i = 0; i < COMMAND_ARGC; i++) {
        free(market_argv[i]);
    }
    free(market_argv);

    for (int i = 0; i < trader_count; i++) {
        delTrader(traders[i]);
    }
    free(traders);

    delMarket(market);
    free(market);

    free(signal_array);

    return 0;
}