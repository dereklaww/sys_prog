#ifndef PE_MYTRADER
#define PE_MYTRADER

#include "pe_common.h"
// #include "pe_order.h"

enum order_type{
    BUY  = 0,
    SELL
};

typedef struct trader Trader;
typedef struct order_struct Order;

struct trader{
    char* name;
    char exc2trader[sizeof(FIFO_EXCHANGE) + 6 + 1];
    char trader2exc[sizeof(FIFO_TRADER) + 6 + 1];
    int fd_w;
    int fd_r;
    pid_t trader_pid;
    int trader_id;
    int product_count;
    char **inventory_products;
    int *inventory_quantity;
    long long int *bank;
    Order** orders;
    int order_count;
    int disconnected;
};

struct order_struct{
    
    int order_type; // BUY - 0, SELL - 1
    char item[BUFFER_READ];
    int quantity;
    long int unit_price;
    Trader *trader;
    int order_id;
    int order_index;
    int completed;

};

int isNumeric (char* str);
Order *newOrder(char* market_argv[], Trader *trader, int order_type);
Order *getOrder(Trader *trader, int order_id);
void closeOrder(Order *order);
void adjustAmount(Order *order, int amount);

Trader* inst_trader(char*name, int trader_id, int product_count, char** product_list); 
void delTrader(Trader *trader);
int buy(Trader *trader, char* product, int quantity, long int buy_price);
int sell(Trader *trader, char* product, int quantity, long int sell_price);
void disconnectTrader(Trader* trader);
int traderAddOrder(Trader *trader, Order *order);
void traderAmendOrder(Trader *trader, Order *order);
void tax(Trader *trader, long long int tax_amount, char* product);

#endif