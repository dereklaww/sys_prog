#include "pe_mytrader.h"

Trader* inst_trader(char*name, int trader_id, int product_count, 
char** product_list) {

    Trader* trader;
    trader = malloc(sizeof(Trader));

    if (trader == NULL) {
        perror("trader - malloc failed\n");
        return NULL;
    }

    trader->name = name;
    trader->trader_id = trader_id;
    trader->product_count = product_count;
    trader->inventory_products = product_list;
    trader->order_count = 0;

    trader->inventory_quantity = calloc(product_count, sizeof(int));
    // trader->bank = calloc(product_count, sizeof(int));

    trader->bank = calloc(product_count, sizeof(long long int));

    if(trader->bank == NULL) {
        perror("trader - malloc failed\n");
    }

    trader->orders = malloc(INITIAL_CAPACITY * sizeof(Order*));
    trader->disconnected = 0;

    return trader;
}

void delTrader(Trader *trader) { 
    free(trader->orders);
    free(trader->inventory_quantity);
    free(trader->bank);
    free(trader);
}

int traderAddOrder(Trader *trader, Order *order) {

    if (trader->order_count >= INITIAL_CAPACITY) {
        
        void *temp = realloc(trader->orders, ((trader->order_count + 1) * sizeof(Order*)));

        if (temp == NULL) {
            perror("realloc failed");
            return 1;
        } else {
            trader->orders = (Order **)temp;
        }
    
        trader->orders[trader->order_count] = order;
    } else {
        trader->orders[trader->order_count] = order;
    }

    trader->order_count++;
    return 0;
}

void traderAmendOrder(Trader *trader, Order *order) {
    for (int i = 0; i < trader->order_count; i++) {
        if (trader->orders[i]->order_id == order->order_id) {
            trader->orders[i] = order;
        }
    }
}

Order *getOrder(Trader *trader, int order_id) {

    for (int i = 0; i < trader->order_count; i++) {
        if ((trader->orders)[i]->order_id == order_id) {
            return trader->orders[i];
        }
    }

    return NULL;
}

int buy(Trader *trader, char* product, int quantity, long int buy_price) {

    // adjust inventory amount
    if (product == NULL) {
        perror("buy - invalid product");
        return 1;
    }

    for (int i = 0; i < trader->product_count; i++) {
        if (!strcmp(trader->inventory_products[i], product)) {
            trader->inventory_quantity[i] = trader->inventory_quantity[i] + quantity;
            trader->bank[i] = trader->bank[i] - (buy_price * quantity);
            return 0;
        }
    }

    perror("buy - product not found");
    return 1;
}

int sell(Trader *trader, char* product, int quantity, long int sell_price) {

    // adjust inventory amount
    if (product == NULL) {
        perror("sell - invalid product");
        return 1;
    }

    for (int i = 0; i < trader->product_count; i++) {
        if (!strcmp(trader->inventory_products[i], product)) {
            trader->inventory_quantity[i] = trader->inventory_quantity[i] - quantity;
            trader->bank[i] = trader->bank[i] + (sell_price * quantity);
            return 0;
        }
    }

    perror("sell - product not found");
    return 1;
}

void tax(Trader *trader, long long int tax_amount, char* product) {
    
    for (int i = 0; i < trader->product_count; i++) {
        if (!strcmp(trader->inventory_products[i], product)) {
            trader->bank[i] = trader->bank[i] - tax_amount;
        }
    }
}

void disconnectTrader(Trader* trader) {

    if (trader != NULL) {
        free(trader);
    }
}



// int main(){


//     char* product_list[] = {"GPU", "Router"};
//     Trader *trader = inst_trader("trader1", 1, 2, product_list);

//     char* product = "Router";
//     int buy_amount = 1000;
//     int buy_price = 1000;

//     for (int i = 0; i < trader->product_count; i++) {
//         if (!strcmp(trader->inventory_products[i], product)) {
//             trader->bank[i] = trader->bank[i] - (buy_amount * buy_price);
//         }
//     }

//     for (int i = 0; i < trader -> product_count; i++) {
//         printf("%ld\n", trader->bank[i]);
//     }

//     return 0;
    
// }