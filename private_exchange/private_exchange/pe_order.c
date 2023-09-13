#include "pe_mytrader.h"

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

Order *newOrder(char* market_argv[], Trader *trader, int order_type) {
    
    // Example message: SELL 20 CPU 2 300;

    if (order_type == -1 || !isNumeric(market_argv[1]) 
    || !isNumeric(market_argv[3]) || !isNumeric(market_argv[4])) {
        return NULL;
    }

    int order_id = atoi(market_argv[1]);
    int order_quantity = atoi(market_argv[3]);
    long int unit_price = (long int) atoi(market_argv[4]);
    
    if (order_id > trader->order_count) {
        return NULL;
    }

    if (order_quantity <= 0 || order_quantity > MAX_QUANTITY) {
        return NULL;
    }

    if (unit_price <= 0 || unit_price > MAX_UNIT_PRICE) {
        return NULL;
    }

    Order *order;
    order = malloc(sizeof(Order));

    if (order == NULL) {
        perror("new order failed\n");
        return NULL;
    }


    strcpy(order->item , market_argv[2]);
    
    order->quantity = order_quantity;
    order->order_type = order_type;
    order->unit_price = unit_price;
    order->order_id = order_id;
    order->completed = 0;
    order->trader = trader;
    order->order_index = -1;

    return order;
}

void closeOrder(Order *order) {
    if (order != NULL) {
        order->completed = 1;
    }
}

void adjustAmount(Order *order, int amount) {
    order->quantity = order->quantity - amount;
}


// int main(){};